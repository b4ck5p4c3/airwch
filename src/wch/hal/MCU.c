/********************************** (C) COPYRIGHT
 ******************************** File Name          : MCU.c Author : WCH
 * Version            : V1.2
 * Date               : 2022/01/18
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
#include "HAL.h"

tmosTaskID halTaskID;

/*******************************************************************************
 * @fn      Lib_Calibration_LSI
 *
 * @param   None.
 *
 * @return  None.
 */
void Lib_Calibration_LSI(void) { Calibration_LSI(Level_64); }

#if (defined(BLE_SNV)) && (BLE_SNV == TRUE)
/*******************************************************************************
 * @fn      Lib_Read_Flash
 *
 * @brief   Callback function used for BLE lib.
 *
 * @param   addr - Read start address
 * @param   num - Number of units to read (unit: 4 bytes)
 * @param   pBuf - Buffer to store read data
 *
 * @return  None.
 */
uint32_t Lib_Read_Flash(uint32_t addr, uint32_t num, uint32_t *pBuf) {
  EEPROM_READ(addr, pBuf, num * 4);
  return 0;
}

/*******************************************************************************
 * @fn      Lib_Write_Flash
 *
 * @brief   Callback function used for BLE lib.
 *
 * @param   addr - Write start address
 * @param   num - Number of units to write (unit: 4 bytes)
 * @param   pBuf - Buffer with data to be written
 *
 * @return  None.
 */
uint32_t Lib_Write_Flash(uint32_t addr, uint32_t num, uint32_t *pBuf) {
  EEPROM_ERASE(addr, num * 4);
  EEPROM_WRITE(addr, pBuf, num * 4);
  return 0;
}
#endif

/*******************************************************************************
 * @fn      CH57X_BLEInit
 *
 * @param   None.
 *
 * @return  None.
 */
void CH58X_BLEInit(void) {
  uint8_t i;
  bleConfig_t cfg;
  if (tmos_memcmp(VER_LIB, VER_FILE, strlen(VER_FILE)) == FALSE) {
    PRINT("head file error...\n");
    while (1)
      ;
  }
  SysTick_Config(SysTick_LOAD_RELOAD_Msk);
  PFIC_DisableIRQ(SysTick_IRQn);

  tmos_memset(&cfg, 0, sizeof(bleConfig_t));
  cfg.MEMAddr = (uint32_t)mem_buffer;
  cfg.MEMLen = (uint32_t)BLE_MEMHEAP_SIZE;
  cfg.BufMaxLen = (uint32_t)BLE_BUFF_MAX_LEN;
  cfg.BufNumber = (uint32_t)BLE_BUFF_NUM;
  cfg.TxNumEvent = (uint32_t)BLE_TX_NUM_EVENT;
  cfg.TxPower = (uint32_t)BLE_TX_POWER;
#if (defined(BLE_SNV)) && (BLE_SNV == TRUE)
  cfg.SNVAddr = (uint32_t)BLE_SNV_ADDR;
  cfg.readFlashCB = Lib_Read_Flash;
  cfg.writeFlashCB = Lib_Write_Flash;
#endif
#if (CLK_OSC32K)
  cfg.SelRTCClock = (uint32_t)CLK_OSC32K;
#endif
  cfg.ConnectNumber =
      (PERIPHERAL_MAX_CONNECTION & 3) | (CENTRAL_MAX_CONNECTION << 2);
  cfg.srandCB = SYS_GetSysTickCnt;
#if (defined TEM_SAMPLE) && (TEM_SAMPLE == TRUE)
  cfg.tsCB = HAL_GetInterTempValue;
#if (CLK_OSC32K)
  cfg.rcCB = Lib_Calibration_LSI;
#endif
#endif
#if (defined(HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  cfg.WakeUpTime = WAKE_UP_RTC_MAX_TIME;
  cfg.sleepCB = CH58X_LowPower;
#endif
#if (defined(BLE_MAC)) && (BLE_MAC == TRUE)
  for (i = 0; i < 6; i++) {
    cfg.MacAddr[i] = kUnusedMacAddress[5 - i];
  }
#else
  {
    uint8_t kMacAddress[6];
    GetMACAddress(kMacAddress);
    for (i = 0; i < 6; i++) {
      cfg.kMacAddress[i] = kMacAddress[i];
    }
  }
#endif
  if (!cfg.MEMAddr || cfg.MEMLen < 4 * 1024) {
    while (1)
      ;
  }
  i = BLE_LibInit(&cfg);
  if (i) {
    PRINT("LIB init error code: %x ...\n", i);
    while (1)
      ;
  }
}

/*******************************************************************************
 * @fn      HAL_ProcessEvent
 *
 * @param   task_id - The TMOS assigned task ID.
 * @param   events  - events to process.  This is a bit map and can
 *                      contain more than one event.
 *
 * @return  events.
 */
tmosEvents HAL_ProcessEvent(tmosTaskID task_id, tmosEvents events) {
  uint8_t *msgPtr;

  if (events & SYS_EVENT_MSG) {
    msgPtr = tmos_msg_receive(task_id);
    if (msgPtr) {
      /* De-allocate */
      tmos_msg_deallocate(msgPtr);
    }
    return events ^ SYS_EVENT_MSG;
  }
  if (events & LED_BLINK_EVENT) {
#if (defined HAL_LED) && (HAL_LED == TRUE)
    HalLedUpdate();
#endif // HAL_LED
    return events ^ LED_BLINK_EVENT;
  }
  if (events & HAL_KEY_EVENT) {
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
    HAL_KeyPoll(); /* Check for keys */
    tmos_start_task(halTaskID, HAL_KEY_EVENT, MS1_TO_SYSTEM_TIME(100));
    return events ^ HAL_KEY_EVENT;
#endif
  }
  if (events & HAL_REG_INIT_EVENT) {
#if (defined BLE_CALIBRATION_ENABLE) && (BLE_CALIBRATION_ENABLE == TRUE)
    BLE_RegInit();
#if (CLK_OSC32K)
    Lib_Calibration_LSI();
#endif
    tmos_start_task(halTaskID, HAL_REG_INIT_EVENT,
                    MS1_TO_SYSTEM_TIME(BLE_CALIBRATION_PERIOD));
    return events ^ HAL_REG_INIT_EVENT;
#endif
  }
  if (events & HAL_TEST_EVENT) {
    PRINT("* \n");
    tmos_start_task(halTaskID, HAL_TEST_EVENT, MS1_TO_SYSTEM_TIME(1000));
    return events ^ HAL_TEST_EVENT;
  }
  return 0;
}

/*******************************************************************************
 * @fn      HAL_Init
 *
 * @param   None.
 *
 * @return  None.
 */
void HAL_Init() {
  halTaskID = TMOS_ProcessEventRegister(HAL_ProcessEvent);
  HAL_TimeInit();
#if (defined HAL_SLEEP) && (HAL_SLEEP == TRUE)
  HAL_SleepInit();
#endif
#if (defined HAL_LED) && (HAL_LED == TRUE)
  HAL_LedInit();
#endif
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
  HAL_KeyInit();
#endif
#if (defined BLE_CALIBRATION_ENABLE) && (BLE_CALIBRATION_ENABLE == TRUE)
  tmos_start_task(halTaskID, HAL_REG_INIT_EVENT,
                  MS1_TO_SYSTEM_TIME(BLE_CALIBRATION_PERIOD));
#endif
  //  tmos_start_task( halTaskID, HAL_TEST_EVENT, 1600 );
}

uint16_t HAL_GetInterTempValue(void) {
  uint8_t sensor, channel, config, tkey_cfg;
  uint16_t adc_data;

  tkey_cfg = R8_TKEY_CFG;
  sensor = R8_TEM_SENSOR;
  channel = R8_ADC_CHANNEL;
  config = R8_ADC_CFG;
  ADC_InterTSSampInit();
  R8_ADC_CONVERT |= RB_ADC_START;
  while (R8_ADC_CONVERT & RB_ADC_START)
    ;
  adc_data = R16_ADC_DATA;
  R8_TEM_SENSOR = sensor;
  R8_ADC_CHANNEL = channel;
  R8_ADC_CFG = config;
  R8_TKEY_CFG = tkey_cfg;
  return (adc_data);
}

/******************************** endfile @ mcu ******************************/
