/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH58x_SPI0.c Author : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH58x_common.h"

void SPI0_MasterDefInit(void) {
  R8_SPI0_CLOCK_DIV = 4;
  R8_SPI0_CTRL_MOD = RB_SPI_ALL_CLEAR;
  R8_SPI0_CTRL_MOD = RB_SPI_MOSI_OE | RB_SPI_SCK_OE;
  R8_SPI0_CTRL_CFG |= RB_SPI_AUTO_IF;
  R8_SPI0_CTRL_CFG &= ~RB_SPI_DMA_ENABLE;
}

void SPI0_CLKCfg(uint8_t c) {
  if (c == 2) {
    R8_SPI0_CTRL_CFG |= RB_SPI_MST_DLY_EN;
  } else {
    R8_SPI0_CTRL_CFG &= ~RB_SPI_MST_DLY_EN;
  }
  R8_SPI0_CLOCK_DIV = c;
}

void SPI0_DataMode(ModeBitOrderTypeDef m) {
  switch (m) {
  case Mode0_LowBitINFront:
    R8_SPI0_CTRL_MOD &= ~RB_SPI_MST_SCK_MOD;
    R8_SPI0_CTRL_CFG |= RB_SPI_BIT_ORDER;
    break;
  case Mode0_HighBitINFront:
    R8_SPI0_CTRL_MOD &= ~RB_SPI_MST_SCK_MOD;
    R8_SPI0_CTRL_CFG &= ~RB_SPI_BIT_ORDER;
    break;
  case Mode3_LowBitINFront:
    R8_SPI0_CTRL_MOD |= RB_SPI_MST_SCK_MOD;
    R8_SPI0_CTRL_CFG |= RB_SPI_BIT_ORDER;
    break;
  case Mode3_HighBitINFront:
    R8_SPI0_CTRL_MOD |= RB_SPI_MST_SCK_MOD;
    R8_SPI0_CTRL_CFG &= ~RB_SPI_BIT_ORDER;
    break;
  default:
    break;
  }
}

void SPI0_MasterSendByte(uint8_t d) {
  R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
  R8_SPI0_BUFFER = d;
  while (!(R8_SPI0_INT_FLAG & RB_SPI_FREE))
    ;
}

uint8_t SPI0_MasterRecvByte(void) {
  R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
  R8_SPI0_BUFFER = 0xFF; // ��������
  while (!(R8_SPI0_INT_FLAG & RB_SPI_FREE))
    ;
  return (R8_SPI0_BUFFER);
}

void SPI0_MasterTrans(uint8_t *pbuf, uint16_t len) {
  uint16_t sendlen;

  sendlen = len;
  R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
  R16_SPI0_TOTAL_CNT = sendlen;
  R8_SPI0_INT_FLAG = RB_SPI_IF_CNT_END;
  while (sendlen) {
    if (R8_SPI0_FIFO_COUNT < SPI_FIFO_SIZE) {
      R8_SPI0_FIFO = *pbuf;
      pbuf++;
      sendlen--;
    }
  }
  while (R8_SPI0_FIFO_COUNT != 0)
    ;
}

void SPI0_MasterRecv(uint8_t *pbuf, uint16_t len) {
  uint16_t readlen;

  readlen = len;
  R8_SPI0_CTRL_MOD |= RB_SPI_FIFO_DIR;
  R16_SPI0_TOTAL_CNT = len;
  R8_SPI0_INT_FLAG = RB_SPI_IF_CNT_END;
  while (readlen) {
    if (R8_SPI0_FIFO_COUNT) {
      *pbuf = R8_SPI0_FIFO;
      pbuf++;
      readlen--;
    }
  }
}

void SPI0_MasterDMATrans(uint8_t *pbuf, uint16_t len) {
  R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
  R16_SPI0_DMA_BEG = (uint32_t)pbuf;
  R16_SPI0_DMA_END = (uint32_t)(pbuf + len);
  R16_SPI0_TOTAL_CNT = len;
  R8_SPI0_INT_FLAG = RB_SPI_IF_CNT_END | RB_SPI_IF_DMA_END;
  R8_SPI0_CTRL_CFG |= RB_SPI_DMA_ENABLE;
  while (!(R8_SPI0_INT_FLAG & RB_SPI_IF_CNT_END))
    ;
  R8_SPI0_CTRL_CFG &= ~RB_SPI_DMA_ENABLE;
}

void SPI0_MasterDMARecv(uint8_t *pbuf, uint16_t len) {
  R8_SPI0_CTRL_MOD |= RB_SPI_FIFO_DIR;
  R16_SPI0_DMA_BEG = (uint32_t)pbuf;
  R16_SPI0_DMA_END = (uint32_t)(pbuf + len);
  R16_SPI0_TOTAL_CNT = len;
  R8_SPI0_INT_FLAG = RB_SPI_IF_CNT_END | RB_SPI_IF_DMA_END;
  R8_SPI0_CTRL_CFG |= RB_SPI_DMA_ENABLE;
  while (!(R8_SPI0_INT_FLAG & RB_SPI_IF_CNT_END))
    ;
  R8_SPI0_CTRL_CFG &= ~RB_SPI_DMA_ENABLE;
}

void SPI0_SlaveInit(void) {
  R8_SPI0_CTRL_MOD = RB_SPI_ALL_CLEAR;
  R8_SPI0_CTRL_MOD = RB_SPI_MISO_OE | RB_SPI_MODE_SLAVE;
  R8_SPI0_CTRL_CFG |= RB_SPI_AUTO_IF;
}

uint8_t SPI0_SlaveRecvByte(void) {
  R8_SPI0_CTRL_MOD |= RB_SPI_FIFO_DIR;
  while (R8_SPI0_FIFO_COUNT == 0)
    ;
  return R8_SPI0_FIFO;
}

void SPI0_SlaveSendByte(uint8_t d) {
  R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
  R8_SPI0_FIFO = d;
  while (R8_SPI0_FIFO_COUNT != 0)
    ;
}

void SPI0_SlaveRecv(uint8_t *pbuf, uint16_t len) {
  uint16_t revlen;

  revlen = len;
  R8_SPI0_CTRL_MOD |= RB_SPI_FIFO_DIR;
  R8_SPI0_INT_FLAG = RB_SPI_IF_CNT_END;
  while (revlen) {
    if (R8_SPI0_FIFO_COUNT) {
      *pbuf = R8_SPI0_FIFO;
      pbuf++;
      revlen--;
    }
  }
}

void SPI0_SlaveTrans(uint8_t *pbuf, uint16_t len) {
  uint16_t sendlen;

  sendlen = len;
  R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
  R8_SPI0_INT_FLAG = RB_SPI_IF_CNT_END;
  while (sendlen) {
    if (R8_SPI0_FIFO_COUNT < SPI_FIFO_SIZE) {
      R8_SPI0_FIFO = *pbuf;
      pbuf++;
      sendlen--;
    }
  }
  while (R8_SPI0_FIFO_COUNT != 0)
    ;
}

void SPI0_SlaveDMARecv(uint8_t *pbuf, uint16_t len) {
  R8_SPI0_CTRL_MOD |= RB_SPI_FIFO_DIR;
  R16_SPI0_DMA_BEG = (uint32_t)pbuf;
  R16_SPI0_DMA_END = (uint32_t)(pbuf + len);
  R16_SPI0_TOTAL_CNT = len;
  R8_SPI0_INT_FLAG = RB_SPI_IF_CNT_END | RB_SPI_IF_DMA_END;
  R8_SPI0_CTRL_CFG |= RB_SPI_DMA_ENABLE;
  while (!(R8_SPI0_INT_FLAG & RB_SPI_IF_CNT_END))
    ;
  R8_SPI0_CTRL_CFG &= ~RB_SPI_DMA_ENABLE;
}

void SPI0_SlaveDMATrans(uint8_t *pbuf, uint16_t len) {
  R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
  R16_SPI0_DMA_BEG = (uint32_t)pbuf;
  R16_SPI0_DMA_END = (uint32_t)(pbuf + len);
  R16_SPI0_TOTAL_CNT = len;
  R8_SPI0_INT_FLAG = RB_SPI_IF_CNT_END | RB_SPI_IF_DMA_END;
  R8_SPI0_CTRL_CFG |= RB_SPI_DMA_ENABLE;
  while (!(R8_SPI0_INT_FLAG & RB_SPI_IF_CNT_END))
    ;
  R8_SPI0_CTRL_CFG &= ~RB_SPI_DMA_ENABLE;
}
