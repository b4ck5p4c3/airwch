/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH57x_adc.h Author : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH58x_ADC_H__
#define __CH58x_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ROM_CFG_TMP_25C 0x7F014

/**
 * @brief  adc single channel define
 */
typedef enum {
  CH_EXTIN_0 = 0,
  CH_EXTIN_1,
  CH_EXTIN_2,
  CH_EXTIN_3,
  CH_EXTIN_4,
  CH_EXTIN_5,
  CH_EXTIN_6,
  CH_EXTIN_7,
  CH_EXTIN_8,
  CH_EXTIN_9,
  CH_EXTIN_10,
  CH_EXTIN_11,
  CH_EXTIN_12,
  CH_EXTIN_13,

  CH_INTE_VBAT = 14,
  CH_INTE_VTEMP = 15,

} ADC_SingleChannelTypeDef;

/**
 * @brief  adc differential channel define
 */
typedef enum {
  CH_DIFF_0_2 = 0,
  CH_DIFF_1_3,

} ADC_DiffChannelTypeDef;

/**
 * @brief  adc sampling clock
 */
typedef enum {
  SampleFreq_3_2 = 0,
  SampleFreq_8,
  SampleFreq_5_33,
  SampleFreq_4,
} ADC_SampClkTypeDef;

/**
 * @brief  adc signal PGA
 */
typedef enum {
  ADC_PGA_1_4 = 0,
  ADC_PGA_1_2,
  ADC_PGA_0,
  ADC_PGA_2,
} ADC_SignalPGATypeDef;

/**
 * @brief  Configuration DMA mode
 */
typedef enum {
  ADC_Mode_Single = 0,
  ADC_Mode_LOOP,
} ADC_DMAModeTypeDef;

#define ADC_ChannelCfg(d) (R8_ADC_CHANNEL = d)

#define ADC_SampClkCfg(d)                                                      \
  (R8_ADC_CFG = R8_ADC_CFG & (~RB_ADC_CLK_DIV) | (d << 6))

#define ADC_PGACfg(d) (R8_ADC_CFG = R8_ADC_CFG & (~RB_ADC_PGA_GAIN) | (d << 4))

#define ADC_TempCalibCfg(d)                                                    \
  (R8_TEM_SENSOR = R8_TEM_SENSOR & (~RB_TEM_SEN_CALIB) | d)

void ADC_ExtSingleChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga);

void ADC_ExtDiffChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga);

void TouchKey_ChSampInit(void);

#define TouchKey_DisableTSPower() (R8_TKEY_CFG &= ~RB_TKEY_PWR_ON)

void ADC_InterTSSampInit(void);

#define ADC_DisableTSPower() (R8_TEM_SENSOR = 0)

void ADC_InterBATSampInit(void);

uint16_t ADC_ExcutSingleConver(void);

signed short ADC_DataCalib_Rough(void);

uint16_t TouchKey_ExcutSingleConver(uint8_t charg, uint8_t disch);

void ADC_AutoConverCycle(uint8_t cycle);

void ADC_DMACfg(uint8_t s, uint16_t startAddr, uint16_t endAddr,
                ADC_DMAModeTypeDef m);

/**
 * @brief   Convert ADC value to temperature(Celsius)
 *
 * @param   adc_val - adc value
 *
 * @return  temperature (Celsius)
 */
int adc_to_temperature_celsius(uint16_t adc_val);

#define ADC_ReadConverValue() (R16_ADC_DATA)

#define ADC_StartUp() (R8_ADC_CONVERT = RB_ADC_START)

#define ADC_GetITStatus() (R8_ADC_INT_FLAG & RB_ADC_IF_EOC)

#define ADC_ClearITFlag() (R8_ADC_CONVERT = 0)

#define ADC_GetDMAStatus() (R8_ADC_DMA_IF & RB_ADC_IF_DMA_END)

#define ADC_ClearDMAFlag() (R8_ADC_DMA_IF |= RB_ADC_IF_DMA_END)

#define ADC_StartDMA() (R8_ADC_CTRL_DMA |= RB_ADC_AUTO_EN)

#define ADC_StopDMA() (R8_ADC_CTRL_DMA &= ~RB_ADC_AUTO_EN)

#define TouchKey_GetITStatus() (R8_ADC_INT_FLAG & RB_ADC_IF_EOC)

#define TouchKey_ClearITFlag() (R8_TKEY_CTRL |= RB_TKEY_PWR_ON)

#ifdef __cplusplus
}
#endif

#endif // __CH58x_ADC_H__
