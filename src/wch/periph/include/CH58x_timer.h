/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH57x_timer.h Author :
 *WCH Version            : V1.2 Date               : 2021/11/17 Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH58x_TIMER_H__
#define __CH58x_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define DataBit_25 (1 << 25)

/**
 * @brief  TMR0 interrupt bit define
 */

#define TMR0_3_IT_CYC_END 0x01
#define TMR0_3_IT_DATA_ACT 0x02
#define TMR0_3_IT_FIFO_HF 0x04
#define TMR1_2_IT_DMA_END 0x08
#define TMR0_3_IT_FIFO_OV 0x10

/**
 * @brief  Configuration PWM effective level repeat times
 */
typedef enum {
  PWM_Times_1 = 0,
  PWM_Times_4,
  PWM_Times_8,
  PWM_Times_16,
} PWM_RepeatTsTypeDef;

/**
 * @brief  Configuration Cap mode
 */
typedef enum {
  CAP_NULL = 0,
  Edge_To_Edge,
  FallEdge_To_FallEdge,
  RiseEdge_To_RiseEdge,
} CapModeTypeDef;

/**
 * @brief  Configuration DMA mode
 */
typedef enum {
  Mode_Single = 0,
  Mode_LOOP,
} DMAModeTypeDef;

void TMR0_TimerInit(uint32_t t);

#define TMR0_GetCurrentTimer() R32_TMR0_COUNT

void TMR0_EXTSingleCounterInit(CapModeTypeDef cap);

#define TMR0_CountOverflowCfg(cyc) (R32_TMR0_CNT_END = (cyc + 2))

#define TMR0_GetCurrentCount() R32_TMR0_COUNT

#define TMR0_PWMCycleCfg(cyc) (R32_TMR0_CNT_END = cyc)

void TMR0_PWMInit(PWMX_PolarTypeDef pr, PWM_RepeatTsTypeDef ts);

#define TMR0_PWMActDataWidth(d) (R32_TMR0_FIFO = d)

#define TMR0_CAPTimeoutCfg(cyc) (R32_TMR0_CNT_END = cyc)

void TMR0_CapInit(CapModeTypeDef cap);

#define TMR0_CAPGetData() R32_TMR0_FIFO

#define TMR0_CAPDataCounter() R8_TMR0_FIFO_COUNT

#define TMR0_Disable() (R8_TMR0_CTRL_MOD &= ~RB_TMR_COUNT_EN)

#define TMR0_Enable() (R8_TMR0_CTRL_MOD |= RB_TMR_COUNT_EN)

#define TMR0_ITCfg(s, f)                                                       \
  ((s) ? (R8_TMR0_INTER_EN |= f) : (R8_TMR0_INTER_EN &= ~f))

#define TMR0_ClearITFlag(f) (R8_TMR0_INT_FLAG = f)

#define TMR0_GetITFlag(f) (R8_TMR0_INT_FLAG & f)

void TMR1_TimerInit(uint32_t t);

#define TMR1_GetCurrentTimer() R32_TMR1_COUNT

void TMR1_EXTSingleCounterInit(CapModeTypeDef cap);

#define TMR1_CountOverflowCfg(cyc) (R32_TMR1_CNT_END = (cyc + 2))

#define TMR1_GetCurrentCount() R32_TMR1_COUNT

#define TMR1_PWMCycleCfg(cyc) (R32_TMR1_CNT_END = cyc)

void TMR1_PWMInit(PWMX_PolarTypeDef pr, PWM_RepeatTsTypeDef ts);

#define TMR1_PWMActDataWidth(d) (R32_TMR1_FIFO = d)

#define TMR1_CAPTimeoutCfg(cyc) (R32_TMR1_CNT_END = cyc)

void TMR1_CapInit(CapModeTypeDef cap);

#define TMR1_CAPGetData() R32_TMR1_FIFO

#define TMR1_CAPDataCounter() R8_TMR1_FIFO_COUNT

void TMR1_DMACfg(uint8_t s, uint16_t startAddr, uint16_t endAddr,
                 DMAModeTypeDef m);

#define TMR1_Disable() (R8_TMR1_CTRL_MOD &= ~RB_TMR_COUNT_EN)

#define TMR1_Enable() (R8_TMR1_CTRL_MOD |= RB_TMR_COUNT_EN)

#define TMR1_ITCfg(s, f)                                                       \
  ((s) ? (R8_TMR1_INTER_EN |= f) : (R8_TMR1_INTER_EN &= ~f))

#define TMR1_ClearITFlag(f) (R8_TMR1_INT_FLAG = f)

#define TMR1_GetITFlag(f) (R8_TMR1_INT_FLAG & f)

void TMR2_TimerInit(uint32_t t);

#define TMR2_GetCurrentTimer() R32_TMR2_COUNT

void TMR2_EXTSingleCounterInit(CapModeTypeDef cap);

#define TMR2_CountOverflowCfg(cyc) (R32_TMR2_CNT_END = (cyc + 2))

#define TMR2_GetCurrentCount() R32_TMR2_COUNT

#define TMR2_PWMCycleCfg(cyc) (R32_TMR2_CNT_END = cyc)

void TMR2_PWMInit(PWMX_PolarTypeDef pr, PWM_RepeatTsTypeDef ts);

#define TMR2_PWMActDataWidth(d) (R32_TMR2_FIFO = d)

#define TMR2_CAPTimeoutCfg(cyc) (R32_TMR2_CNT_END = cyc)

void TMR2_CapInit(CapModeTypeDef cap);

#define TMR2_CAPGetData() R32_TMR2_FIFO

#define TMR2_CAPDataCounter() R8_TMR2_FIFO_COUNT

void TMR2_DMACfg(uint8_t s, uint16_t startAddr, uint16_t endAddr,
                 DMAModeTypeDef m);

#define TMR2_Disable() (R8_TMR2_CTRL_MOD &= ~RB_TMR_COUNT_EN)

#define TMR2_Enable() (R8_TMR2_CTRL_MOD |= RB_TMR_COUNT_EN)

#define TMR2_ITCfg(s, f)                                                       \
  ((s) ? (R8_TMR2_INTER_EN |= f) : (R8_TMR2_INTER_EN &= ~f))

#define TMR2_ClearITFlag(f) (R8_TMR2_INT_FLAG = f)

#define TMR2_GetITFlag(f) (R8_TMR2_INT_FLAG & f)

void TMR3_TimerInit(uint32_t t);

#define TMR3_GetCurrentTimer() R32_TMR3_COUNT

void TMR3_EXTSingleCounterInit(CapModeTypeDef cap);

#define TMR3_CountOverflowCfg(cyc) (R32_TMR3_CNT_END = (cyc + 2))

#define TMR3_GetCurrentCount() R32_TMR3_COUNT

#define TMR3_PWMCycleCfg(cyc) (R32_TMR3_CNT_END = cyc)

void TMR3_PWMInit(PWMX_PolarTypeDef pr, PWM_RepeatTsTypeDef ts);

#define TMR3_PWMActDataWidth(d) (R32_TMR3_FIFO = d)

#define TMR3_CAPTimeoutCfg(cyc) (R32_TMR3_CNT_END = cyc)

void TMR3_CapInit(CapModeTypeDef cap);

#define TMR3_CAPGetData() R32_TMR3_FIFO

#define TMR3_CAPDataCounter() R8_TMR3_FIFO_COUNT

#define TMR3_Disable() (R8_TMR3_CTRL_MOD &= ~RB_TMR_COUNT_EN)

#define TMR3_Enable() (R8_TMR3_CTRL_MOD |= RB_TMR_COUNT_EN)

#define TMR3_ITCfg(s, f)                                                       \
  ((s) ? (R8_TMR3_INTER_EN |= f) : (R8_TMR3_INTER_EN &= ~f))

#define TMR3_ClearITFlag(f) (R8_TMR3_INT_FLAG = f)

#define TMR3_GetITFlag(f) (R8_TMR3_INT_FLAG & f)

#ifdef __cplusplus
}
#endif

#endif // __CH58x_TIMER_H__
