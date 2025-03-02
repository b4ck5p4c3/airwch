/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH57x_SPI.h Author : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH58x_SPI_H__
#define __CH58x_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  SPI0 interrupt bit define
 */
#define SPI0_IT_FST_BYTE RB_SPI_IE_FST_BYTE
#define SPI0_IT_FIFO_OV RB_SPI_IE_FIFO_OV
#define SPI0_IT_DMA_END RB_SPI_IE_DMA_END
#define SPI0_IT_FIFO_HF RB_SPI_IE_FIFO_HF
#define SPI0_IT_BYTE_END RB_SPI_IE_BYTE_END
#define SPI0_IT_CNT_END RB_SPI_IE_CNT_END

/**
 * @brief  Configuration data mode
 */
typedef enum {
  Mode0_LowBitINFront = 0,
  Mode0_HighBitINFront,
  Mode3_LowBitINFront,
  Mode3_HighBitINFront,
} ModeBitOrderTypeDef;

/**
 * @brief  Configuration SPI0 slave mode
 */
typedef enum {
  Mode_DataStream = 0,
  Mose_FirstCmd,
} Slave_ModeTypeDef;

void SPI0_MasterDefInit(void);

void SPI0_CLKCfg(uint8_t c);

void SPI0_DataMode(ModeBitOrderTypeDef m);

void SPI0_MasterSendByte(uint8_t d);

uint8_t SPI0_MasterRecvByte(void);

void SPI0_MasterTrans(uint8_t *pbuf, uint16_t len);

void SPI0_MasterRecv(uint8_t *pbuf, uint16_t len);

void SPI0_MasterDMATrans(uint8_t *pbuf, uint16_t len);

void SPI0_MasterDMARecv(uint8_t *pbuf, uint16_t len);

void SPI1_MasterDefInit(void);
void SPI1_CLKCfg(UINT8 c);
void SPI1_DataMode(ModeBitOrderTypeDef m);

void SPI1_MasterSendByte(UINT8 d);
UINT8 SPI1_MasterRecvByte(void);

void SPI1_MasterTrans(UINT8 *pbuf, UINT16 len);
void SPI1_MasterRecv(UINT8 *pbuf, UINT16 len);

void SPI0_SlaveInit(void);

#define SetFirstData(d) (R8_SPI0_SLAVE_PRE = d)

void SPI0_SlaveSendByte(uint8_t d);

uint8_t SPI0_SlaveRecvByte(void);

void SPI0_SlaveTrans(uint8_t *pbuf, uint16_t len);

void SPI0_SlaveRecv(uint8_t *pbuf, uint16_t len);

void SPI0_SlaveDMATrans(uint8_t *pbuf, uint16_t len);

void SPI0_SlaveDMARecv(uint8_t *pbuf, uint16_t len);

#define SPI0_ITCfg(s, f)                                                       \
  ((s) ? (R8_SPI0_INTER_EN |= f) : (R8_SPI0_INTER_EN &= ~f))

#define SPI0_GetITFlag(f) (R8_SPI0_INT_FLAG & f)

#define SPI0_ClearITFlag(f) (R8_SPI0_INT_FLAG = f)

#ifdef __cplusplus
}
#endif

#endif // __CH58x_SPI_H__
