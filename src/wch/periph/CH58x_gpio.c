/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH58x_gpio.c Author : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH58x_common.h"

void GPIOA_ModeCfg(uint32_t pin, GPIOModeTypeDef mode) {
  switch (mode) {
  case GPIO_ModeIN_Floating:
    R32_PA_PD_DRV &= ~pin;
    R32_PA_PU &= ~pin;
    R32_PA_DIR &= ~pin;
    break;

  case GPIO_ModeIN_PU:
    R32_PA_PD_DRV &= ~pin;
    R32_PA_PU |= pin;
    R32_PA_DIR &= ~pin;
    break;

  case GPIO_ModeIN_PD:
    R32_PA_PD_DRV |= pin;
    R32_PA_PU &= ~pin;
    R32_PA_DIR &= ~pin;
    break;

  case GPIO_ModeOut_PP_5mA:
    R32_PA_PD_DRV &= ~pin;
    R32_PA_DIR |= pin;
    break;

  case GPIO_ModeOut_PP_20mA:
    R32_PA_PD_DRV |= pin;
    R32_PA_DIR |= pin;
    break;

  default:
    break;
  }
}

void GPIOB_ModeCfg(uint32_t pin, GPIOModeTypeDef mode) {
  switch (mode) {
  case GPIO_ModeIN_Floating:
    R32_PB_PD_DRV &= ~pin;
    R32_PB_PU &= ~pin;
    R32_PB_DIR &= ~pin;
    break;

  case GPIO_ModeIN_PU:
    R32_PB_PD_DRV &= ~pin;
    R32_PB_PU |= pin;
    R32_PB_DIR &= ~pin;
    break;

  case GPIO_ModeIN_PD:
    R32_PB_PD_DRV |= pin;
    R32_PB_PU &= ~pin;
    R32_PB_DIR &= ~pin;
    break;

  case GPIO_ModeOut_PP_5mA:
    R32_PB_PD_DRV &= ~pin;
    R32_PB_DIR |= pin;
    break;

  case GPIO_ModeOut_PP_20mA:
    R32_PB_PD_DRV |= pin;
    R32_PB_DIR |= pin;
    break;

  default:
    break;
  }
}

void GPIOA_ITModeCfg(uint32_t pin, GPIOITModeTpDef mode) {
  switch (mode) {
  case GPIO_ITMode_LowLevel:
    R16_PA_INT_MODE &= ~pin;
    R32_PA_CLR |= pin;
    break;

  case GPIO_ITMode_HighLevel:
    R16_PA_INT_MODE &= ~pin;
    R32_PA_OUT |= pin;
    break;

  case GPIO_ITMode_FallEdge:
    R16_PA_INT_MODE |= pin;
    R32_PA_CLR |= pin;
    break;

  case GPIO_ITMode_RiseEdge:
    R16_PA_INT_MODE |= pin;
    R32_PA_OUT |= pin;
    break;

  default:
    break;
  }
  R16_PA_INT_IF = pin;
  R16_PA_INT_EN |= pin;
}

void GPIOB_ITModeCfg(uint32_t pin, GPIOITModeTpDef mode) {
  uint32_t Pin = pin | ((pin & (GPIO_Pin_22 | GPIO_Pin_23)) >> 14);
  switch (mode) {
  case GPIO_ITMode_LowLevel:
    R16_PB_INT_MODE &= ~Pin;
    R32_PB_CLR |= pin;
    break;

  case GPIO_ITMode_HighLevel:
    R16_PB_INT_MODE &= ~Pin;
    R32_PB_OUT |= pin;
    break;

  case GPIO_ITMode_FallEdge:
    R16_PB_INT_MODE |= Pin;
    R32_PB_CLR |= pin;
    break;

  case GPIO_ITMode_RiseEdge:
    R16_PB_INT_MODE |= Pin;
    R32_PB_OUT |= pin;
    break;

  default:
    break;
  }
  R16_PB_INT_IF = Pin;
  R16_PB_INT_EN |= Pin;
}

void GPIOPinRemap(FunctionalState s, uint16_t perph) {
  if (s) {
    R16_PIN_ALTERNATE |= perph;
  } else {
    R16_PIN_ALTERNATE &= ~perph;
  }
}

void GPIOAGPPCfg(FunctionalState s, uint16_t perph) {
  if (s) {
    R16_PIN_ANALOG_IE |= perph;
  } else {
    R16_PIN_ANALOG_IE &= ~perph;
  }
}
