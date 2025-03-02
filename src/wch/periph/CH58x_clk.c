/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH58x_clk.c Author : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH58x_common.h"

void LClk32K_Select(LClk32KTypeDef hc) {
  uint8_t cfg = R8_CK32K_CONFIG;

  if (hc == Clk32K_LSI) {
    cfg &= ~RB_CLK_OSC32K_XT;
  } else {
    cfg |= RB_CLK_OSC32K_XT;
  }

  sys_safe_access_enable();
  R8_CK32K_CONFIG = cfg;
  sys_safe_access_disable();
}

void HSECFG_Current(HSECurrentTypeDef c) {
  uint8_t x32M_c;

  x32M_c = R8_XT32M_TUNE;
  x32M_c = (x32M_c & 0xfc) | (c & 0x03);

  sys_safe_access_enable();
  R8_XT32M_TUNE = x32M_c;
  sys_safe_access_disable();
}

void HSECFG_Capacitance(HSECapTypeDef c) {
  uint8_t x32M_c;

  x32M_c = R8_XT32M_TUNE;
  x32M_c = (x32M_c & 0x8f) | (c << 4);

  sys_safe_access_enable();
  R8_XT32M_TUNE = x32M_c;
  sys_safe_access_disable();
}

void LSECFG_Current(LSECurrentTypeDef c) {
  uint8_t x32K_c;

  x32K_c = R8_XT32K_TUNE;
  x32K_c = (x32K_c & 0xfc) | (c & 0x03);

  sys_safe_access_enable();
  R8_XT32K_TUNE = x32K_c;
  sys_safe_access_disable();
}

void LSECFG_Capacitance(LSECapTypeDef c) {
  uint8_t x32K_c;

  x32K_c = R8_XT32K_TUNE;
  x32K_c = (x32K_c & 0x0f) | (c << 4);

  sys_safe_access_enable();
  R8_XT32K_TUNE = x32K_c;
  sys_safe_access_disable();
}

void Calibration_LSI(Cali_LevelTypeDef cali_Lv) {
  UINT32 i;
  INT32 cnt_offset;
  UINT8 retry = 0;
  INT32 freq_sys;

  freq_sys = GetSysClock();

  sys_safe_access_enable();
  R8_CK32K_CONFIG |= RB_CLK_OSC32K_FILT;
  R8_CK32K_CONFIG &= ~RB_CLK_OSC32K_FILT;
  sys_safe_access_enable();
  R8_XT32K_TUNE &= ~3;
  R8_XT32K_TUNE |= 1;

  sys_safe_access_enable();
  R8_OSC_CAL_CTRL &= ~RB_OSC_CNT_TOTAL;
  R8_OSC_CAL_CTRL |= 1;
  sys_safe_access_enable();
  R8_OSC_CAL_CTRL |= RB_OSC_CNT_EN;
  R16_OSC_CAL_CNT |= RB_OSC_CAL_OV_CLR;
  while ((R8_OSC_CAL_CTRL & RB_OSC_CNT_EN) != RB_OSC_CNT_EN) {
    sys_safe_access_enable();
    R8_OSC_CAL_CTRL |= RB_OSC_CNT_EN;
  }
  while (1) {
    while (!(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT))
      ;
    i = R16_OSC_CAL_CNT;
    while (R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT)
      ;
    R16_OSC_CAL_CNT |= RB_OSC_CAL_OV_CLR;
    while (!(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT))
      ;
    i = R16_OSC_CAL_CNT;
    cnt_offset = (i & 0x3FFF) + R8_OSC_CAL_OV_CNT * 0x3FFF -
                 2000 * (freq_sys / 1000) / CAB_LSIFQ;
    if (((cnt_offset > -37 * (freq_sys / 1000) / CAB_LSIFQ) &&
         (cnt_offset < 37 * (freq_sys / 1000) / CAB_LSIFQ)) ||
        retry > 2)
      break;
    retry++;
    cnt_offset =
        (cnt_offset > 0)
            ? (((cnt_offset * 2) / (37 * (freq_sys / 1000) / CAB_LSIFQ)) + 1) /
                  2
            : (((cnt_offset * 2) / (37 * (freq_sys / 1000) / CAB_LSIFQ)) - 1) /
                  2;
    sys_safe_access_enable();
    R16_INT32K_TUNE += cnt_offset;
  }

  while (!(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT))
    ;
  i = R16_OSC_CAL_CNT;
  R16_OSC_CAL_CNT |= RB_OSC_CAL_OV_CLR;
  sys_safe_access_enable();
  R8_OSC_CAL_CTRL &= ~RB_OSC_CNT_TOTAL;
  R8_OSC_CAL_CTRL |= cali_Lv;
  while (R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT)
    ;
  while (!(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT))
    ;
  i = R16_OSC_CAL_CNT;
  cnt_offset = (i & 0x3FFF) + R8_OSC_CAL_OV_CNT * 0x3FFF -
               4000 * (1 << cali_Lv) * (freq_sys / 1000000) / CAB_LSIFQ * 1000;
  cnt_offset = (cnt_offset > 0) ? ((((cnt_offset * (3200 / (1 << cali_Lv))) /
                                     (1366 * (freq_sys / 1000) / CAB_LSIFQ)) +
                                    1) /
                                   2) << 5
                                : ((((cnt_offset * (3200 / (1 << cali_Lv))) /
                                     (1366 * (freq_sys / 1000) / CAB_LSIFQ)) -
                                    1) /
                                   2) << 5;
  sys_safe_access_enable();
  R16_INT32K_TUNE += cnt_offset;
  R8_OSC_CAL_CTRL &= ~RB_OSC_CNT_EN;
}

void RTC_InitTime(uint16_t y, uint16_t mon, uint16_t d, uint16_t h, uint16_t m,
                  uint16_t s) {
  uint32_t t;
  uint16_t year, month, day, sec2, t32k;
  volatile uint8_t clk_pin;

  year = y;
  month = mon;
  day = 0;
  while (year > BEGYEAR) {
    day += YearLength(year - 1);
    year--;
  }
  while (month > 1) {
    day += monthLength(IsLeapYear(y), month - 2);
    month--;
  }

  day += d - 1;
  sec2 = (h % 24) * 1800 + m * 30 + s / 2;
  t32k = (s & 1) ? (0x8000) : (0);
  t = sec2;
  t = t << 16 | t32k;

  do {
    clk_pin = (R8_CK32K_CONFIG & RB_32K_CLK_PIN);
  } while (clk_pin != (R8_CK32K_CONFIG & RB_32K_CLK_PIN));
  if (!clk_pin) {
    while (!clk_pin) {
      do {
        clk_pin = (R8_CK32K_CONFIG & RB_32K_CLK_PIN);
      } while (clk_pin != (R8_CK32K_CONFIG & RB_32K_CLK_PIN));
    }
  }

  sys_safe_access_enable();
  R32_RTC_TRIG = day;
  R8_RTC_MODE_CTRL |= RB_RTC_LOAD_HI;
  while ((R32_RTC_TRIG & 0x3FFF) != (R32_RTC_CNT_DAY & 0x3FFF))
    ;
  sys_safe_access_enable();
  R32_RTC_TRIG = t;
  R8_RTC_MODE_CTRL |= RB_RTC_LOAD_LO;
  sys_safe_access_disable();
}

void RTC_GetTime(uint16_t *py, uint16_t *pmon, uint16_t *pd, uint16_t *ph,
                 uint16_t *pm, uint16_t *ps) {
  uint32_t t;
  uint16_t day, sec2, t32k;

  day = R32_RTC_CNT_DAY & 0x3FFF;
  sec2 = R16_RTC_CNT_2S;
  t32k = R16_RTC_CNT_32K;

  t = sec2 * 2 + ((t32k < 0x8000) ? 0 : 1);

  *py = BEGYEAR;
  while (day >= YearLength(*py)) {
    day -= YearLength(*py);
    (*py)++;
  }

  *pmon = 0;
  while (day >= monthLength(IsLeapYear(*py), *pmon)) {
    day -= monthLength(IsLeapYear(*py), *pmon);
    (*pmon)++;
  }
  (*pmon)++;
  *pd = day + 1;
  *ph = t / 3600;
  *pm = t % 3600 / 60;
  *ps = t % 60;
}

void RTC_SetCycle32k(uint32_t cyc) {
  volatile uint8_t clk_pin;

  do {
    clk_pin = (R8_CK32K_CONFIG & RB_32K_CLK_PIN);
  } while ((clk_pin != (R8_CK32K_CONFIG & RB_32K_CLK_PIN)) || (!clk_pin));

  sys_safe_access_enable();
  R32_RTC_TRIG = cyc;
  R8_RTC_MODE_CTRL |= RB_RTC_LOAD_LO;
  sys_safe_access_disable();
}

uint32_t RTC_GetCycle32k(void) {
  volatile uint32_t i;

  do {
    i = R32_RTC_CNT_32K;
  } while (i != R32_RTC_CNT_32K);

  return (i);
}

void RTC_TMRFunCfg(RTC_TMRCycTypeDef t) {
  sys_safe_access_enable();
  R8_RTC_MODE_CTRL &= ~(RB_RTC_TMR_EN | RB_RTC_TMR_MODE);
  sys_safe_access_enable();
  R8_RTC_MODE_CTRL |= RB_RTC_TMR_EN | (t);
  sys_safe_access_disable();
}

void RTC_TRIGFunCfg(uint32_t cyc) {
  uint32_t t;

  t = RTC_GetCycle32k() + cyc;
  if (t > 0xA8C00000) {
    t -= 0xA8C00000;
  }

  sys_safe_access_enable();
  R32_RTC_TRIG = t;
  R8_RTC_MODE_CTRL |= RB_RTC_TRIG_EN;
  sys_safe_access_disable();
}

void RTC_ModeFunDisable(RTC_MODETypeDef m) {
  uint8_t i = 0;

  if (m == RTC_TRIG_MODE) {
    i |= RB_RTC_TRIG_EN;
  } else if (m == RTC_TMR_MODE) {
    i |= RB_RTC_TMR_EN;
  }

  sys_safe_access_enable();
  R8_RTC_MODE_CTRL &= ~(i);
  sys_safe_access_disable();
}

uint8_t RTC_GetITFlag(RTC_EVENTTypeDef f) {
  if (f == RTC_TRIG_EVENT) {
    return (R8_RTC_FLAG_CTRL & RB_RTC_TRIG_FLAG);
  } else {
    return (R8_RTC_FLAG_CTRL & RB_RTC_TMR_FLAG);
  }
}

void RTC_ClearITFlag(RTC_EVENTTypeDef f) {
  switch (f) {
  case RTC_TRIG_EVENT:
    R8_RTC_FLAG_CTRL = RB_RTC_TRIG_CLR;
    break;
  case RTC_TMR_EVENT:
    R8_RTC_FLAG_CTRL = RB_RTC_TMR_CLR;
    break;
  default:
    break;
  }
}
