/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH57x_pwm.h Author : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH58x_PWM_H__
#define __CH58x_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  channel of PWM define
 */
#define CH_PWM4 0x01
#define CH_PWM5 0x02
#define CH_PWM6 0x04
#define CH_PWM7 0x08
#define CH_PWM8 0x10
#define CH_PWM9 0x20
#define CH_PWM10 0x40
#define CH_PWM11 0x80

/**
 * @brief  channel of PWM define
 */
typedef enum {
  High_Level = 0,
  Low_Level,
} PWMX_PolarTypeDef;

/**
 * @brief  Configuration PWM4_11 Cycle size
 */
typedef enum {
  PWMX_Cycle_256 = 0,
  PWMX_Cycle_255,
  PWMX_Cycle_128,
  PWMX_Cycle_127,
  PWMX_Cycle_64,
  PWMX_Cycle_63,
  PWMX_Cycle_32,
  PWMX_Cycle_31,
} PWMX_CycleTypeDef;

#define PWMX_CLKCfg(d) (R8_PWM_CLOCK_DIV = d)

void PWMX_CycleCfg(PWMX_CycleTypeDef cyc);

#define PWM4_ActDataWidth(d) (R8_PWM4_DATA = d)

#define PWM5_ActDataWidth(d) (R8_PWM5_DATA = d)

#define PWM6_ActDataWidth(d) (R8_PWM6_DATA = d)

#define PWM7_ActDataWidth(d) (R8_PWM7_DATA = d)

#define PWM8_ActDataWidth(d) (R8_PWM8_DATA = d)

#define PWM9_ActDataWidth(d) (R8_PWM9_DATA = d)

#define PWM10_ActDataWidth(d) (R8_PWM10_DATA = d)

#define PWM11_ActDataWidth(d) (R8_PWM11_DATA = d)

void PWMX_ACTOUT(uint8_t ch, uint8_t da, PWMX_PolarTypeDef pr,
                 FunctionalState s);

void PWMX_AlterOutCfg(uint8_t ch, FunctionalState s);

#ifdef __cplusplus
}
#endif

#endif // __CH58x_PWM_H__
