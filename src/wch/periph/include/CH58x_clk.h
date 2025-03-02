/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH57x_clk.h Author : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH58x_CLK_H__
#define __CH58x_CLK_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  CLK_SOURCE_LSI = 0x00,
  CLK_SOURCE_LSE,

  CLK_SOURCE_HSE_16MHz = 0x22,
  CLK_SOURCE_HSE_8MHz = 0x24,
  CLK_SOURCE_HSE_6_4MHz = 0x25,
  CLK_SOURCE_HSE_4MHz = 0x28,
  CLK_SOURCE_HSE_2MHz = (0x20 | 16),
  CLK_SOURCE_HSE_1MHz = (0x20 | 0),

  CLK_SOURCE_PLL_80MHz = 0x46,
  CLK_SOURCE_PLL_60MHz = 0x48,
  CLK_SOURCE_PLL_48MHz = (0x40 | 10),
  CLK_SOURCE_PLL_40MHz = (0x40 | 12),
  CLK_SOURCE_PLL_36_9MHz = (0x40 | 13),
  CLK_SOURCE_PLL_32MHz = (0x40 | 15),
  CLK_SOURCE_PLL_30MHz = (0x40 | 16),
  CLK_SOURCE_PLL_24MHz = (0x40 | 20),
  CLK_SOURCE_PLL_20MHz = (0x40 | 24),
  CLK_SOURCE_PLL_15MHz = (0x40 | 0),
} SYS_CLKTypeDef;

typedef enum {
  Clk32K_LSI = 0,
  Clk32K_LSE,

} LClk32KTypeDef;

typedef enum {
  HSE_RCur_75 = 0,
  HSE_RCur_100,
  HSE_RCur_125,
  HSE_RCur_150

} HSECurrentTypeDef;

typedef enum {
  HSECap_10p = 0,
  HSECap_12p,
  HSECap_14p,
  HSECap_16p,
  HSECap_18p,
  HSECap_20p,
  HSECap_22p,
  HSECap_24p

} HSECapTypeDef;

typedef enum {
  LSE_RCur_70 = 0,
  LSE_RCur_100,
  LSE_RCur_140,
  LSE_RCur_200

} LSECurrentTypeDef;

typedef enum {
  LSECap_2p = 0,
  LSECap_13p,
  LSECap_14p,
  LSECap_15p,
  LSECap_16p,
  LSECap_17p,
  LSECap_18p,
  LSECap_19p,
  LSECap_20p,
  LSECap_21p,
  LSECap_22p,
  LSECap_23p,
  LSECap_24p,
  LSECap_25p,
  LSECap_26p,
  LSECap_27p

} LSECapTypeDef;

#define MAX_DAY 0x00004000
#define MAX_2_SEC 0x0000A8C0
// #define	 MAX_SEC		0x545FFFFF

#define BEGYEAR 2020
#define IsLeapYear(yr) (!((yr) % 400) || (((yr) % 100) && !((yr) % 4)))
#define YearLength(yr) (IsLeapYear(yr) ? 366 : 365)
#define monthLength(lpyr, mon)                                                 \
  (((mon) == 1)                                                                \
       ? (28 + (lpyr))                                                         \
       : (((mon) > 6) ? (((mon) & 1) ? 31 : 30) : (((mon) & 1) ? 30 : 31)))

/**
 * @brief  rtc timer mode period define
 */
typedef enum {
  Period_0_125_S = 0,
  Period_0_25_S,
  Period_0_5_S,
  Period_1_S,
  Period_2_S,
  Period_4_S,
  Period_8_S,
  Period_16_S,
} RTC_TMRCycTypeDef;

/**
 * @brief  rtc interrupt event define
 */
typedef enum {
  RTC_TRIG_EVENT = 0,
  RTC_TMR_EVENT,

} RTC_EVENTTypeDef;

/**
 * @brief  rtc interrupt event define
 */
typedef enum {
  RTC_TRIG_MODE = 0,
  RTC_TMR_MODE,

} RTC_MODETypeDef;

typedef enum {
  Level_32 = 3,
  Level_64,
  Level_128,

} Cali_LevelTypeDef;

void LClk32K_Select(LClk32KTypeDef hc);

void HSECFG_Current(HSECurrentTypeDef c);

void HSECFG_Capacitance(HSECapTypeDef c);

void LSECFG_Current(LSECurrentTypeDef c);

void LSECFG_Capacitance(LSECapTypeDef c);

void Calibration_LSI(Cali_LevelTypeDef cali_Lv); /* ����ƵУ׼�ڲ�32Kʱ�� */

void RTC_InitTime(uint16_t y, uint16_t mon, uint16_t d, uint16_t h, uint16_t m,
                  uint16_t s);

void RTC_GetTime(uint16_t *py, uint16_t *pmon, uint16_t *pd, uint16_t *ph,
                 uint16_t *pm, uint16_t *ps);

void RTC_SetCycle32k(uint32_t cyc);

uint32_t RTC_GetCycle32k(void);

void RTC_TRIGFunCfg(uint32_t cyc);

void RTC_TMRFunCfg(RTC_TMRCycTypeDef t);

void RTC_ModeFunDisable(RTC_MODETypeDef m);

uint8_t RTC_GetITFlag(RTC_EVENTTypeDef f);

void RTC_ClearITFlag(RTC_EVENTTypeDef f);

#ifdef __cplusplus
}
#endif

#endif // __CH58x_CLK_H__
