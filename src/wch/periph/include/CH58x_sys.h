/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH57x_SYS.h Author : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH58x_SYS_H__
#define __CH58x_SYS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  rtc interrupt event define
 */
typedef enum {
  RST_STATUS_SW = 0,
  RST_STATUS_RPOR,
  RST_STATUS_WTR,
  RST_STATUS_MR,
  RST_STATUS_LRM0,
  RST_STATUS_GPWSM,
  RST_STATUS_LRM1,
  RST_STATUS_LRM2,

} SYS_ResetStaTypeDef;

/**
 * @brief  rtc interrupt event define
 */
typedef enum {
  INFO_ROM_READ = 0,
  INFO_RESET_EN = 2,
  INFO_BOOT_EN,
  INFO_DEBUG_EN,
  INFO_LOADER,
  STA_SAFEACC_ACT,

} SYS_InfoStaTypeDef;

#define SYS_GetChipID() R8_CHIP_ID

#define SYS_GetAccessID() R8_SAFE_ACCESS_ID

void SetSysClock(SYS_CLKTypeDef sc);

uint32_t GetSysClock(void);

uint8_t SYS_GetInfoSta(SYS_InfoStaTypeDef i);

#define SYS_GetLastResetSta() (R8_RESET_STATUS & RB_RESET_FLAG)

void SYS_ResetExecute(void);

#define SYS_ResetKeepBuf(d) (R8_GLOB_RESET_KEEP = d)

void SYS_DisableAllIrq(uint32_t *pirqv);

void SYS_RecoverIrq(uint32_t irq_status);

uint32_t SYS_GetSysTickCnt(void);

#define WWDG_SetCounter(c) (R8_WDOG_COUNT = c)

void WWDG_ITCfg(FunctionalState s);

void WWDG_ResetCfg(FunctionalState s);

#define WWDG_GetFlowFlag() (R8_RST_WDOG_CTRL & RB_WDOG_INT_FLAG)

void WWDG_ClearFlag(void);

void mDelayuS(uint16_t t);

void mDelaymS(uint16_t t);

/* Safe access */
__attribute__((always_inline)) static inline void sys_safe_access_enable(void) {
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
  SAFEOPERATE;
}

__attribute__((always_inline)) static inline void
sys_safe_access_disable(void) {
  R8_SAFE_ACCESS_SIG = 0;
}

#ifdef __cplusplus
}
#endif

#endif // __CH58x_SYS_H__
