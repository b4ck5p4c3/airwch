/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH57x_gpio.h Author : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH58x_GPIO_H__
#define __CH58x_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	GPIO_pins_define
 */
#define GPIO_Pin_0 (0x00000001)   /*!< Pin 0 selected */
#define GPIO_Pin_1 (0x00000002)   /*!< Pin 1 selected */
#define GPIO_Pin_2 (0x00000004)   /*!< Pin 2 selected */
#define GPIO_Pin_3 (0x00000008)   /*!< Pin 3 selected */
#define GPIO_Pin_4 (0x00000010)   /*!< Pin 4 selected */
#define GPIO_Pin_5 (0x00000020)   /*!< Pin 5 selected */
#define GPIO_Pin_6 (0x00000040)   /*!< Pin 6 selected */
#define GPIO_Pin_7 (0x00000080)   /*!< Pin 7 selected */
#define GPIO_Pin_8 (0x00000100)   /*!< Pin 8 selected */
#define GPIO_Pin_9 (0x00000200)   /*!< Pin 9 selected */
#define GPIO_Pin_10 (0x00000400)  /*!< Pin 10 selected */
#define GPIO_Pin_11 (0x00000800)  /*!< Pin 11 selected */
#define GPIO_Pin_12 (0x00001000)  /*!< Pin 12 selected */
#define GPIO_Pin_13 (0x00002000)  /*!< Pin 13 selected */
#define GPIO_Pin_14 (0x00004000)  /*!< Pin 14 selected */
#define GPIO_Pin_15 (0x00008000)  /*!< Pin 15 selected */
#define GPIO_Pin_16 (0x00010000)  /*!< Pin 16 selected */
#define GPIO_Pin_17 (0x00020000)  /*!< Pin 17 selected */
#define GPIO_Pin_18 (0x00040000)  /*!< Pin 18 selected */
#define GPIO_Pin_19 (0x00080000)  /*!< Pin 19 selected */
#define GPIO_Pin_20 (0x00100000)  /*!< Pin 20 selected */
#define GPIO_Pin_21 (0x00200000)  /*!< Pin 21 selected */
#define GPIO_Pin_22 (0x00400000)  /*!< Pin 22 selected */
#define GPIO_Pin_23 (0x00800000)  /*!< Pin 23 selected */
#define GPIO_Pin_All (0xFFFFFFFF) /*!< All pins selected */

/**
 * @brief  Configuration GPIO Mode
 */
typedef enum {
  GPIO_ModeIN_Floating,
  GPIO_ModeIN_PU,
  GPIO_ModeIN_PD,
  GPIO_ModeOut_PP_5mA,
  GPIO_ModeOut_PP_20mA,

} GPIOModeTypeDef;

/**
 * @brief  Configuration GPIO IT Mode
 */
typedef enum {
  GPIO_ITMode_LowLevel,
  GPIO_ITMode_HighLevel,
  GPIO_ITMode_FallEdge,
  GPIO_ITMode_RiseEdge,

} GPIOITModeTpDef;

void GPIOA_ModeCfg(uint32_t pin, GPIOModeTypeDef mode);

void GPIOB_ModeCfg(uint32_t pin, GPIOModeTypeDef mode);

#define GPIOA_ResetBits(pin) (R32_PA_CLR |= pin)

#define GPIOA_SetBits(pin) (R32_PA_OUT |= pin)

#define GPIOB_ResetBits(pin) (R32_PB_CLR |= pin)

#define GPIOB_SetBits(pin) (R32_PB_OUT |= pin)

#define GPIOA_InverseBits(pin) (R32_PA_OUT ^= pin)

#define GPIOB_InverseBits(pin) (R32_PB_OUT ^= pin)

#define GPIOA_ReadPort() (R32_PA_PIN)

#define GPIOB_ReadPort() (R32_PB_PIN)

#define GPIOA_ReadPortPin(pin) (R32_PA_PIN & (pin))

#define GPIOB_ReadPortPin(pin) (R32_PB_PIN & (pin))

void GPIOA_ITModeCfg(uint32_t pin, GPIOITModeTpDef mode);

void GPIOB_ITModeCfg(uint32_t pin, GPIOITModeTpDef mode);

#define GPIOA_ReadITFlagPort() (R16_PA_INT_IF)

#define GPIOB_ReadITFlagPort()                                                 \
  ((R16_PB_INT_IF & (~((GPIO_Pin_22 | GPIO_Pin_23) >> 14))) |                  \
   ((R16_PB_INT_IF << 14) & (GPIO_Pin_22 | GPIO_Pin_23)))

#define GPIOA_ReadITFlagBit(pin) (R16_PA_INT_IF & (pin))

#define GPIOB_ReadITFlagBit(pin)                                               \
  (R16_PB_INT_IF & ((pin) | (((pin) & (GPIO_Pin_22 | GPIO_Pin_23)) >> 14)))

#define GPIOA_ClearITFlagBit(pin) (R16_PA_INT_IF = pin)

#define GPIOB_ClearITFlagBit(pin)                                              \
  (R16_PB_INT_IF = ((pin) | (((pin) & (GPIO_Pin_22 | GPIO_Pin_23)) >> 14)))

void GPIOPinRemap(FunctionalState s, uint16_t perph);

void GPIOAGPPCfg(FunctionalState s, uint16_t perph);

#ifdef __cplusplus
}
#endif

#endif // __CH58x_GPIO_H__
