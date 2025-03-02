/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH58x_uart2.c Author :
 *WCH Version            : V1.2 Date               : 2021/11/17 Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH58x_common.h"

void UART2_DefInit(void) {
  UART2_BaudRateCfg(115200);
  R8_UART2_FCR = (2 << 6) | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR |
                 RB_FCR_FIFO_EN; // FIFO�򿪣�������4�ֽ�
  R8_UART2_LCR = RB_LCR_WORD_SZ;
  R8_UART2_IER = RB_IER_TXD_EN;
  R8_UART2_DIV = 1;
}

void UART2_BaudRateCfg(uint32_t baudrate) {
  uint32_t x;

  x = 10 * GetSysClock() / 8 / baudrate;
  x = (x + 5) / 10;
  R16_UART2_DL = (uint16_t)x;
}

void UART2_ByteTrigCfg(UARTByteTRIGTypeDef b) {
  R8_UART2_FCR = (R8_UART2_FCR & ~RB_FCR_FIFO_TRIG) | (b << 6);
}

void UART2_INTCfg(FunctionalState s, uint8_t i) {
  if (s) {
    R8_UART2_IER |= i;
    R8_UART2_MCR |= RB_MCR_INT_OE;
  } else {
    R8_UART2_IER &= ~i;
  }
}

void UART2_Reset(void) { R8_UART2_IER = RB_IER_RESET; }

void UART2_SendString(uint8_t *buf, uint16_t l) {
  uint16_t len = l;

  while (len) {
    if (R8_UART2_TFC != UART_FIFO_SIZE) {
      R8_UART2_THR = *buf++;
      len--;
    }
  }
}

uint16_t UART2_RecvString(uint8_t *buf) {
  uint16_t len = 0;

  while (R8_UART2_RFC) {
    *buf++ = R8_UART2_RBR;
    len++;
  }

  return (len);
}
