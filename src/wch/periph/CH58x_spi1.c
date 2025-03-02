/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH58x_SPI1.c Author : WCH
 * Version            : V1.0
 * Date               : 2018/12/15
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH58x_common.h"

void SPI1_MasterDefInit(void) {
  R8_SPI1_CLOCK_DIV = 4;
  R8_SPI1_CTRL_MOD = RB_SPI_ALL_CLEAR;
  R8_SPI1_CTRL_MOD = RB_SPI1_SDO_OE | RB_SPI_SCK_OE;
  R8_SPI1_CTRL_CFG |= RB_SPI_AUTO_IF;
}

void SPI1_CLKCfg(uint8_t c) {
  if (c == 2) {
    R8_SPI1_CTRL_CFG |= RB_SPI_MST_DLY_EN;
  } else {
    R8_SPI1_CTRL_CFG &= ~RB_SPI_MST_DLY_EN;
  }
  R8_SPI1_CLOCK_DIV = c;
}

void SPI1_DataMode(ModeBitOrderTypeDef m) {
  switch (m) {
  case Mode0_LowBitINFront:
    R8_SPI1_CTRL_MOD &= ~RB_SPI_MST_SCK_MOD;
    R8_SPI1_CTRL_CFG |= RB_SPI_BIT_ORDER;
    break;
  case Mode0_HighBitINFront:
    R8_SPI1_CTRL_MOD &= ~RB_SPI_MST_SCK_MOD;
    R8_SPI1_CTRL_CFG &= ~RB_SPI_BIT_ORDER;
    break;
  case Mode3_LowBitINFront:
    R8_SPI1_CTRL_MOD |= RB_SPI_MST_SCK_MOD;
    R8_SPI1_CTRL_CFG |= RB_SPI_BIT_ORDER;
    break;
  case Mode3_HighBitINFront:
    R8_SPI1_CTRL_MOD |= RB_SPI_MST_SCK_MOD;
    R8_SPI1_CTRL_CFG &= ~RB_SPI_BIT_ORDER;
    break;
  default:
    break;
  }
}

void SPI1_MasterSendByte(uint8_t d) {
  R8_SPI1_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
  R8_SPI1_BUFFER = d;
  while (!(R8_SPI1_INT_FLAG & RB_SPI_FREE))
    ;
}

uint8_t SPI1_MasterRecvByte(void) {
  R8_SPI1_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
  R8_SPI1_BUFFER = 0xFF; // ��������
  while (!(R8_SPI1_INT_FLAG & RB_SPI_FREE))
    ;
  return (R8_SPI1_BUFFER);
}

void SPI1_MasterTrans(uint8_t *pbuf, uint16_t len) {
  uint16_t sendlen;

  sendlen = len;
  R8_SPI1_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
  R16_SPI1_TOTAL_CNT = sendlen;
  R8_SPI1_INT_FLAG = RB_SPI_IF_CNT_END;
  while (sendlen) {
    if (R8_SPI1_FIFO_COUNT < SPI_FIFO_SIZE) {
      R8_SPI1_FIFO = *pbuf;
      pbuf++;
      sendlen--;
    }
  }
  while (R8_SPI1_FIFO_COUNT != 0)
    ;
}

void SPI1_MasterRecv(uint8_t *pbuf, uint16_t len) {
  uint16_t readlen;

  readlen = len;
  R8_SPI1_CTRL_MOD |= RB_SPI_FIFO_DIR;
  R16_SPI1_TOTAL_CNT = len;
  R8_SPI1_INT_FLAG = RB_SPI_IF_CNT_END;
  while (readlen) {
    if (R8_SPI1_FIFO_COUNT) {
      *pbuf = R8_SPI1_FIFO;
      pbuf++;
      readlen--;
    }
  }
}
