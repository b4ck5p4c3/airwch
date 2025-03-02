/********************************** (C) COPYRIGHT
 ******************************** File Name          : HAL.h Author : WCH
 * Version            : V1.0
 * Date               : 2016/05/05
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
#ifndef __HAL_H
#define __HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "KEY.h"
#include "LED.h"
#include "RTC.h"
#include "SLEEP.h"
#include "config.h"

/* hal task Event */
#define LED_BLINK_EVENT 0x0001
#define HAL_KEY_EVENT 0x0002
#define HAL_REG_INIT_EVENT 0x2000
#define HAL_TEST_EVENT 0x4000

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern tmosTaskID halTaskID;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

extern void HAL_Init(void);

extern tmosEvents HAL_ProcessEvent(tmosTaskID task_id, tmosEvents events);

extern void CH58X_BLEInit(void);

extern uint16_t HAL_GetInterTempValue(void);

extern void Lib_Calibration_LSI(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
