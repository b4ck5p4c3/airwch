/********************************** (C) COPYRIGHT
 ******************************** File Name          : RTC.c Author : WCH
 * Version            : V1.2
 * Date               : 2022/01/18
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
#include "HAL.h"

/*********************************************************************
 * CONSTANTS
 */
#define RTC_INIT_TIME_HOUR 0
#define RTC_INIT_TIME_MINUTE 0
#define RTC_INIT_TIME_SECEND 0

/***************************************************
 * Global variables
 */
volatile uint32_t RTCTigFlag;

void RTC_SetTignTime(uint32_t time) {
  sys_safe_access_enable();
  R32_RTC_TRIG = time;
  sys_safe_access_disable();
  RTCTigFlag = 0;
}

__INTERRUPT
__HIGH_CODE
void RTC_IRQHandler(void) {
  R8_RTC_FLAG_CTRL = (RB_RTC_TMR_CLR | RB_RTC_TRIG_CLR);
  RTCTigFlag = 1;
}

void HAL_TimeInit(void) {
#if (CLK_OSC32K)
  sys_safe_access_enable();
  R8_CK32K_CONFIG &= ~(RB_CLK_OSC32K_XT | RB_CLK_XT32K_PON);
  sys_safe_access_enable();
  R8_CK32K_CONFIG |= RB_CLK_INT32K_PON;
  sys_safe_access_disable();
  Lib_Calibration_LSI();
#else
  sys_safe_access_enable();
  R8_CK32K_CONFIG |= RB_CLK_OSC32K_XT | RB_CLK_INT32K_PON | RB_CLK_XT32K_PON;
  sys_safe_access_disable();
#endif
  RTC_InitTime(2020, 1, 1, 0, 0, 0);
  TMOS_TimerInit(0);
}

/******************************** endfile @ time ******************************/
