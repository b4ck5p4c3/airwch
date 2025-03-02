/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH58x_adc.c Author : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH58x_common.h"

/*********************************************************************
 * @fn      ADC_DataCalib_Rough
 *
 * @param   none
 */
signed short ADC_DataCalib_Rough(void) {
  uint16_t i;
  uint32_t sum = 0;
  uint8_t ch = 0;
  uint8_t ctrl = 0;

  ch = R8_ADC_CHANNEL;
  ctrl = R8_ADC_CFG;
  R8_ADC_CFG = 0;

  ADC_ChannelCfg(1);
  R8_ADC_CFG |= RB_ADC_OFS_TEST | RB_ADC_POWER_ON | (2 << 4);
  R8_ADC_CONVERT = RB_ADC_START;
  while (R8_ADC_CONVERT & RB_ADC_START)
    ;
  for (i = 0; i < 16; i++) {
    R8_ADC_CONVERT = RB_ADC_START;
    while (R8_ADC_CONVERT & RB_ADC_START)
      ;
    sum += (~R16_ADC_DATA) & RB_ADC_DATA;
  }
  sum = (sum + 8) >> 4;
  R8_ADC_CFG &= ~RB_ADC_OFS_TEST;

  R8_ADC_CHANNEL = ch;
  R8_ADC_CFG = ctrl;
  return (2048 - sum);
}

void ADC_ExtSingleChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga) {
  R8_TKEY_CFG &= ~RB_TKEY_PWR_ON;
  R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_BUF_EN | (sp << 6) | (ga << 4);
}

void ADC_ExtDiffChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga) {
  R8_TKEY_CFG &= ~RB_TKEY_PWR_ON;
  R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_DIFF_EN | (sp << 6) | (ga << 4);
}

void ADC_InterTSSampInit(void) {
  R8_TKEY_CFG &= ~RB_TKEY_PWR_ON;
  R8_TEM_SENSOR = RB_TEM_SEN_PWR_ON;
  R8_ADC_CHANNEL = CH_INTE_VTEMP;
  R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_DIFF_EN | (3 << 4);
}

void ADC_InterBATSampInit(void) {
  R8_TKEY_CFG &= ~RB_TKEY_PWR_ON;
  R8_ADC_CHANNEL = CH_INTE_VBAT;
  R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_BUF_EN | (0 << 4); // ʹ��-12dBģʽ��
}

void TouchKey_ChSampInit(void) {
  R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_BUF_EN | (2 << 4);
  R8_TKEY_CFG |= RB_TKEY_PWR_ON;
}

uint16_t ADC_ExcutSingleConver(void) {
  R8_ADC_CONVERT = RB_ADC_START;
  while (R8_ADC_CONVERT & RB_ADC_START)
    ;

  return (R16_ADC_DATA & RB_ADC_DATA);
}

uint16_t TouchKey_ExcutSingleConver(uint8_t charg, uint8_t disch) {
  R8_TKEY_COUNT = (disch << 5) | (charg & 0x1f);
  R8_TKEY_CONVERT = RB_TKEY_START;
  while (R8_TKEY_CONVERT & RB_TKEY_START)
    ;
  return (R16_ADC_DATA & RB_ADC_DATA);
}

void ADC_AutoConverCycle(uint8_t cycle) { R8_ADC_AUTO_CYCLE = cycle; }

void ADC_DMACfg(uint8_t s, uint16_t startAddr, uint16_t endAddr,
                ADC_DMAModeTypeDef m) {
  if (s == DISABLE) {
    R8_ADC_CTRL_DMA &= ~RB_ADC_DMA_ENABLE;
  } else {
    R16_ADC_DMA_BEG = startAddr;
    R16_ADC_DMA_END = endAddr;
    if (m) {
      R8_ADC_CTRL_DMA |=
          RB_ADC_DMA_LOOP | RB_ADC_IE_DMA_END | RB_ADC_DMA_ENABLE;
    } else {
      R8_ADC_CTRL_DMA &= ~RB_ADC_DMA_LOOP;
      R8_ADC_CTRL_DMA |= RB_ADC_IE_DMA_END | RB_ADC_DMA_ENABLE;
    }
  }
}

/*********************************************************************
 * @fn      adc_to_temperature_celsius
 *
 * @brief   Convert ADC value to temperature(Celsius)
 *
 * @param   adc_val - adc value
 *
 * @return  temperature (Celsius)
 */

int adc_to_temperature_celsius(uint16_t adc_val) {
  uint32_t C25 = 0;
  int temp;

  C25 = (*((PUINT32)ROM_CFG_TMP_25C));

  /* current temperature = standard temperature + (adc deviation * adc linearity
   * coefficient) */
  temp = (((C25 >> 16) & 0xFFFF) ? ((C25 >> 16) & 0xFFFF) : 25) +
         (adc_val - ((int)(C25 & 0xFFFF))) * 10 / 27;

  return (temp);
}
