/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH58x_SYS.c Author : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH58x_common.h"

/*********************************************************************
 * @fn      SetSysClock
 *
 * @return  none
 */
__HIGH_CODE
void SetSysClock(SYS_CLKTypeDef sc) {
  uint32_t i;
  sys_safe_access_enable();
  R8_PLL_CONFIG &= ~(1 << 5); //
  sys_safe_access_disable();
  if (sc & 0x20) { // HSE div
    if (!(R8_HFCK_PWR_CTRL & RB_CLK_XT32M_PON)) {
      sys_safe_access_enable();
      R8_HFCK_PWR_CTRL |= RB_CLK_XT32M_PON; // HSE power on
      for (i = 0; i < 1200; i++) {
        __nop();
        __nop();
      }
    }

    sys_safe_access_enable();
    R16_CLK_SYS_CFG = (0 << 6) | (sc & 0x1f);
    __nop();
    __nop();
    __nop();
    __nop();
    sys_safe_access_disable();
    sys_safe_access_enable();
    SAFEOPERATE;
    R8_FLASH_CFG = 0X51;
    sys_safe_access_disable();
  }

  else if (sc & 0x40) { // PLL div
    if (!(R8_HFCK_PWR_CTRL & RB_CLK_PLL_PON)) {
      sys_safe_access_enable();
      R8_HFCK_PWR_CTRL |= RB_CLK_PLL_PON; // PLL power on
      for (i = 0; i < 2000; i++) {
        __nop();
        __nop();
      }
    }
    sys_safe_access_enable();
    R16_CLK_SYS_CFG = (1 << 6) | (sc & 0x1f);
    __nop();
    __nop();
    __nop();
    __nop();
    sys_safe_access_disable();
    if (sc == CLK_SOURCE_PLL_80MHz) {
      sys_safe_access_enable();
      R8_FLASH_CFG = 0X02;
      sys_safe_access_disable();
    } else {
      sys_safe_access_enable();
      R8_FLASH_CFG = 0X52;
      sys_safe_access_disable();
    }
  } else {
    sys_safe_access_enable();
    R16_CLK_SYS_CFG |= RB_CLK_SYS_MOD;
  }

  sys_safe_access_enable();
  R8_PLL_CONFIG |= 1 << 7;
  sys_safe_access_disable();
}

uint32_t GetSysClock(void) {
  uint16_t rev;

  rev = R16_CLK_SYS_CFG & 0xff;
  if ((rev & 0x40) == (0 << 6)) {
    return (32000000 / (rev & 0x1f));
  } else if ((rev & RB_CLK_SYS_MOD) == (1 << 6)) {
    return (480000000 / (rev & 0x1f));
  } else {
    return (32000);
  }
}

uint8_t SYS_GetInfoSta(SYS_InfoStaTypeDef i) {
  if (i == STA_SAFEACC_ACT) {
    return (R8_SAFE_ACCESS_SIG & RB_SAFE_ACC_ACT);
  } else {
    return (R8_GLOB_CFG_INFO & (1 << i));
  }
}

__HIGH_CODE
void SYS_ResetExecute(void) {
  FLASH_ROM_SW_RESET();
  sys_safe_access_enable();
  R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
  sys_safe_access_disable();
}

void SYS_DisableAllIrq(uint32_t *pirqv) {
  *pirqv = (PFIC->ISR[0] >> 8) | (PFIC->ISR[1] << 24);
  PFIC->IRER[0] = 0xffffffff;
  PFIC->IRER[1] = 0xffffffff;
}

void SYS_RecoverIrq(uint32_t irq_status) {
  PFIC->IENR[0] = (irq_status << 8);
  PFIC->IENR[1] = (irq_status >> 24);
}

uint32_t SYS_GetSysTickCnt(void) {
  uint32_t val;

  val = SysTick->CNT;
  return (val);
}

void WWDG_ITCfg(FunctionalState s) {
  uint8_t ctrl = R8_RST_WDOG_CTRL;

  if (s == DISABLE) {
    ctrl &= ~RB_WDOG_INT_EN;
  } else {
    ctrl |= RB_WDOG_INT_EN;
  }

  sys_safe_access_enable();
  R8_RST_WDOG_CTRL = ctrl;
  sys_safe_access_disable();
}

void WWDG_ResetCfg(FunctionalState s) {
  uint8_t ctrl = R8_RST_WDOG_CTRL;

  if (s == DISABLE) {
    ctrl &= ~RB_WDOG_RST_EN;
  } else {
    ctrl |= RB_WDOG_RST_EN;
  }

  sys_safe_access_enable();
  R8_RST_WDOG_CTRL = ctrl;
  sys_safe_access_disable();
}

void WWDG_ClearFlag(void) {
  sys_safe_access_enable();
  R8_RST_WDOG_CTRL |= RB_WDOG_INT_FLAG;
  sys_safe_access_disable();
}

__INTERRUPT
__HIGH_CODE
__attribute__((weak)) void HardFault_Handler(void) {
  FLASH_ROM_SW_RESET();
  sys_safe_access_enable();
  R16_INT32K_TUNE = 0xFFFF;
  sys_safe_access_enable();
  R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
  sys_safe_access_disable();
  while (1)
    ;
}

__HIGH_CODE
void mDelayuS(uint16_t t) {
  uint32_t i;
#if (FREQ_SYS == 80000000)
  i = t * 20;
#elif (FREQ_SYS == 60000000)
  i = t * 15;
#elif (FREQ_SYS == 48000000)
  i = t * 12;
#elif (FREQ_SYS == 40000000)
  i = t * 10;
#elif (FREQ_SYS == 32000000)
  i = t << 3;
#elif (FREQ_SYS == 24000000)
  i = t * 6;
#elif (FREQ_SYS == 16000000)
  i = t << 2;
#elif (FREQ_SYS == 8000000)
  i = t << 1;
#elif (FREQ_SYS == 4000000)
  i = t;
#elif (FREQ_SYS == 2000000)
  i = t >> 1;
#elif (FREQ_SYS == 1000000)
  i = t >> 2;
#else
  i = t << 1;
#endif
  do {
    __nop();
  } while (--i);
}

__HIGH_CODE
void mDelaymS(uint16_t t) {
  uint16_t i;

  for (i = 0; i < t; i++) {
    mDelayuS(1000);
  }
}

#ifdef DEBUG
int _write(int fd, char *buf, int size) {
  int i;
  for (i = 0; i < size; i++) {
#if DEBUG == Debug_UART0
    while (R8_UART0_TFC == UART_FIFO_SIZE)
      ;
    R8_UART0_THR = *buf++;
#elif DEBUG == Debug_UART1
    while (R8_UART1_TFC == UART_FIFO_SIZE)
      ;
    R8_UART1_THR = *buf++;
#elif DEBUG == Debug_UART2
    while (R8_UART2_TFC == UART_FIFO_SIZE)
      ;
    R8_UART2_THR = *buf++;
#elif DEBUG == Debug_UART3
    while (R8_UART3_TFC == UART_FIFO_SIZE)
      ;
    R8_UART3_THR = *buf++;
#endif
  }
  return size;
}

#endif
