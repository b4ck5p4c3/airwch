/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH58x_uart3.c Author :
 *WCH Version            : V1.2 Date               : 2021/11/17 Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH58x_common.h"

void UART3_DefInit(void) {
  UART3_BaudRateCfg(115200);
  R8_UART3_FCR = (2 << 6) | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR |
                 RB_FCR_FIFO_EN; // FIFO�򿪣�������4�ֽ�
  R8_UART3_LCR = RB_LCR_WORD_SZ;
  R8_UART3_IER = RB_IER_TXD_EN;
  R8_UART3_DIV = 1;
}

void UART3_BaudRateCfg(uint32_t baudrate) {
  uint32_t x;

  x = 10 * GetSysClock() / 8 / baudrate;
  x = (x + 5) / 10;
  R16_UART3_DL = (uint16_t)x;
}

void UART3_ByteTrigCfg(UARTByteTRIGTypeDef b) {
  R8_UART3_FCR = (R8_UART3_FCR & ~RB_FCR_FIFO_TRIG) | (b << 6);
}

void UART3_INTCfg(FunctionalState s, uint8_t i) {
  if (s) {
    R8_UART3_IER |= i;
    R8_UART3_MCR |= RB_MCR_INT_OE;
  } else {
    R8_UART3_IER &= ~i;
  }
}

void UART3_Reset(void) { R8_UART3_IER = RB_IER_RESET; }

void UART3_SendString(uint8_t *buf, uint16_t l) {
  uint16_t len = l;

  while (len) {
    if (R8_UART3_TFC != UART_FIFO_SIZE) {
      R8_UART3_THR = *buf++;
      len--;
    }
  }
}

uint16_t UART3_RecvString(uint8_t *buf) {
  uint16_t len = 0;

  while (R8_UART3_RFC) {
    *buf++ = R8_UART3_RBR;
    len++;
  }

  return (len);
}
