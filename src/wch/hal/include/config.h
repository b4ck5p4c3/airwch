/********************************** (C) COPYRIGHT
 ******************************** File Name          : CONFIG.h Author : WCH
 * Version            : V1.2
 * Date               : 2022/01/18
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
#ifndef __CONFIG_H
#define __CONFIG_H

#define ID_CH583 0x83

#define CHIP_ID ID_CH583

#ifdef CH58xBLE_ROM
#include "CH58xBLE_ROM.H"
#else
#include "CH58xBLE_LIB.h"
#endif

#include "CH58x_common.h"

#ifndef BLE_MAC
#define BLE_MAC TRUE
#endif
#ifndef DCDC_ENABLE
#define DCDC_ENABLE FALSE
#endif
#ifndef HAL_SLEEP
#define HAL_SLEEP TRUE
#endif
#ifndef SLEEP_RTC_MIN_TIME
#define SLEEP_RTC_MIN_TIME (30U)
#endif
#ifndef WAKE_UP_RTC_MAX_TIME
#define WAKE_UP_RTC_MAX_TIME (45U)
#endif
#ifndef HAL_KEY
#define HAL_KEY FALSE
#endif
#ifndef HAL_LED
#define HAL_LED FALSE
#endif
#ifndef TEM_SAMPLE
#define TEM_SAMPLE TRUE
#endif
#ifndef BLE_CALIBRATION_ENABLE
#define BLE_CALIBRATION_ENABLE TRUE
#endif
#ifndef BLE_CALIBRATION_PERIOD
#define BLE_CALIBRATION_PERIOD 120000
#endif
#ifndef BLE_SNV
#define BLE_SNV TRUE
#endif
#ifndef BLE_SNV_ADDR
#define BLE_SNV_ADDR 0x77E00 - FLASH_ROM_MAX_SIZE
#endif
#ifndef CLK_OSC32K
#define CLK_OSC32K 1
#endif
#ifndef BLE_MEMHEAP_SIZE
#define BLE_MEMHEAP_SIZE (1024 * 6)
#endif
#ifndef BLE_BUFF_MAX_LEN
#define BLE_BUFF_MAX_LEN 27
#endif
#ifndef BLE_BUFF_NUM
#define BLE_BUFF_NUM 5
#endif
#ifndef BLE_TX_NUM_EVENT
#define BLE_TX_NUM_EVENT 1
#endif
#ifndef BLE_TX_POWER
#define BLE_TX_POWER LL_TX_POWEER_0_DBM
#endif
#ifndef PERIPHERAL_MAX_CONNECTION
#define PERIPHERAL_MAX_CONNECTION 1
#endif
#ifndef CENTRAL_MAX_CONNECTION
#define CENTRAL_MAX_CONNECTION 3
#endif

extern uint32_t mem_buffer[BLE_MEMHEAP_SIZE / 4];
extern const uint8_t kUnusedMacAddress[6];

#endif
