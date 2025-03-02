/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH58x_pwm.c Author : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH58x_common.h"

void PWMX_CycleCfg(PWMX_CycleTypeDef cyc) {
  switch (cyc) {
  case PWMX_Cycle_256:
    R8_PWM_CONFIG = R8_PWM_CONFIG & 0xf0;
    break;

  case PWMX_Cycle_255:
    R8_PWM_CONFIG = (R8_PWM_CONFIG & 0xf0) | 0x01;
    break;

  case PWMX_Cycle_128:
    R8_PWM_CONFIG = (R8_PWM_CONFIG & 0xf0) | (1 << 2);
    break;

  case PWMX_Cycle_127:
    R8_PWM_CONFIG = (R8_PWM_CONFIG & 0xf0) | (1 << 2) | 0x01;
    break;

  case PWMX_Cycle_64:
    R8_PWM_CONFIG = (R8_PWM_CONFIG & 0xf0) | (2 << 2);
    break;

  case PWMX_Cycle_63:
    R8_PWM_CONFIG = (R8_PWM_CONFIG & 0xf0) | (2 << 2) | 0x01;
    break;

  case PWMX_Cycle_32:
    R8_PWM_CONFIG = (R8_PWM_CONFIG & 0xf0) | (3 << 2);
    break;

  case PWMX_Cycle_31:
    R8_PWM_CONFIG = (R8_PWM_CONFIG & 0xf0) | (3 << 2) | 0x01;
    break;

  default:
    break;
  }
}

void PWMX_ACTOUT(uint8_t ch, uint8_t da, PWMX_PolarTypeDef pr,
                 FunctionalState s) {
  uint8_t i;

  if (s == DISABLE) {
    R8_PWM_OUT_EN &= ~(ch);
  } else {
    (pr) ? (R8_PWM_POLAR |= (ch)) : (R8_PWM_POLAR &= ~(ch));
    for (i = 0; i < 8; i++) {
      if ((ch >> i) & 1) {
        *((volatile uint8_t *)((&R8_PWM4_DATA) + i)) = da;
      }
    }
    R8_PWM_OUT_EN |= (ch);
  }
}

void PWMX_AlterOutCfg(uint8_t ch, FunctionalState s) {
  if (s == DISABLE) {
    R8_PWM_CONFIG &= ~(ch);
  } else {
    R8_PWM_CONFIG |= (ch);
  }
}
