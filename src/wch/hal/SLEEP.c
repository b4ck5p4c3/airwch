/********************************** (C) COPYRIGHT
 ******************************** File Name          : SLEEP.c Author : WCH
 * Version            : V1.2
 * Date               : 2022/01/18
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
#include "HAL.h"

uint32_t CH58X_LowPower(uint32_t time) {
#if (defined(HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  uint32_t time_sleep, time_curr, irq_status;

  SYS_DisableAllIrq(&irq_status);
  time_curr = RTC_GetCycle32k();
  if (time < time_curr) {
    time_sleep = time + (RTC_TIMER_MAX_VALUE - time_curr);
  } else {
    time_sleep = time - time_curr;
  }

  if ((time_sleep < SLEEP_RTC_MIN_TIME) ||
      (time_sleep > (RTC_TIMER_MAX_VALUE - TMOS_TIME_VALID))) {
    SYS_RecoverIrq(irq_status);
    return 2;
  }

  RTC_SetTignTime(time);
  SYS_RecoverIrq(irq_status);
#if (DEBUG == Debug_UART1)
  while ((R8_UART1_LSR & RB_LSR_TX_ALL_EMP) == 0) {
    __nop();
  }
#endif
  if (!RTCTigFlag) {
    LowPower_Sleep(RB_PWR_RAM2K | RB_PWR_RAM30K | RB_PWR_EXTEND);
    if (RTCTigFlag) {
      time += WAKE_UP_RTC_MAX_TIME;
      if (time > 0xA8C00000) {
        time -= 0xA8C00000;
      }
      RTC_SetTignTime(time);
      LowPower_Idle();
    }
    HSECFG_Current(HSE_RCur_100);
  } else {
    return 3;
  }
#endif
  return 0;
}

/*******************************************************************************
 * @fn      HAL_SleepInit
 *
 * @param   None.
 *
 * @return  None.
 */
void HAL_SleepInit(void) {
#if (defined(HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  sys_safe_access_enable();
  R8_SLP_WAKE_CTRL |= RB_SLP_RTC_WAKE;
  sys_safe_access_enable();
  R8_RTC_MODE_CTRL |= RB_RTC_TRIG_EN;
  sys_safe_access_disable();
  PFIC_EnableIRQ(RTC_IRQn);
#endif
}
