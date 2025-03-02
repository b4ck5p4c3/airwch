/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH57x_timer0.c Author :
 *WCH Version            : V1.2 Date               : 2021/11/17 Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH58x_common.h"

void TMR0_TimerInit(uint32_t t) {
  R32_TMR0_CNT_END = t;
  R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
  R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN;
}

void TMR0_EXTSingleCounterInit(CapModeTypeDef cap) {
  R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
  R8_TMR0_CTRL_MOD =
      RB_TMR_COUNT_EN | RB_TMR_CAP_COUNT | RB_TMR_MODE_IN | (cap << 6);
}

void TMR0_PWMInit(PWMX_PolarTypeDef pr, PWM_RepeatTsTypeDef ts) {
  //    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
  R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN | RB_TMR_OUT_EN | (pr << 4) | (ts << 6);
}

void TMR0_CapInit(CapModeTypeDef cap) {
  R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
  R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN | RB_TMR_MODE_IN | (cap << 6);
}
