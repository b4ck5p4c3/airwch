/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH57x_uart.h Author : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH58x_UART_H__
#define __CH58x_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	LINE error and status define
 */
#define STA_ERR_BREAK RB_LSR_BREAK_ERR
#define STA_ERR_FRAME RB_LSR_FRAME_ERR
#define STA_ERR_PAR RB_LSR_PAR_ERR
#define STA_ERR_FIFOOV RB_LSR_OVER_ERR

#define STA_TXFIFO_EMP RB_LSR_TX_FIFO_EMP
#define STA_TXALL_EMP RB_LSR_TX_ALL_EMP
#define STA_RECV_DATA RB_LSR_DATA_RDY

/**
 * @brief  Configuration UART TrigByte num
 */
typedef enum {
  UART_1BYTE_TRIG = 0,
  UART_2BYTE_TRIG,
  UART_4BYTE_TRIG,
  UART_7BYTE_TRIG,

} UARTByteTRIGTypeDef;

void UART0_DefInit(void);

void UART0_BaudRateCfg(uint32_t baudrate);

void UART0_ByteTrigCfg(UARTByteTRIGTypeDef b);

void UART0_INTCfg(FunctionalState s, uint8_t i);

void UART0_Reset(void);

#define UART0_CLR_RXFIFO() (R8_UART0_FCR |= RB_FCR_RX_FIFO_CLR)

#define UART0_CLR_TXFIFO() (R8_UART0_FCR |= RB_FCR_TX_FIFO_CLR)

#define UART0_GetITFlag() (R8_UART0_IIR & RB_IIR_INT_MASK)

#define UART0_GetLinSTA() (R8_UART0_LSR)

#define UART0_SendByte(b) (R8_UART0_THR = b)

void UART0_SendString(uint8_t *buf, uint16_t l);

#define UART0_RecvByte() (R8_UART0_RBR)

uint16_t UART0_RecvString(uint8_t *buf);

void UART1_DefInit(void);

void UART1_BaudRateCfg(uint32_t baudrate);

void UART1_ByteTrigCfg(UARTByteTRIGTypeDef b);

void UART1_INTCfg(FunctionalState s, uint8_t i);

void UART1_Reset(void);

#define UART1_CLR_RXFIFO() (R8_UART1_FCR |= RB_FCR_RX_FIFO_CLR)

#define UART1_CLR_TXFIFO() (R8_UART1_FCR |= RB_FCR_TX_FIFO_CLR)

#define UART1_GetITFlag() (R8_UART1_IIR & RB_IIR_INT_MASK)

#define UART1_GetLinSTA() (R8_UART1_LSR)

#define UART1_SendByte(b) (R8_UART1_THR = b)

void UART1_SendString(uint8_t *buf, uint16_t l);

#define UART1_RecvByte() (R8_UART1_RBR)

uint16_t UART1_RecvString(uint8_t *buf);

void UART2_DefInit(void);

void UART2_BaudRateCfg(uint32_t baudrate);

void UART2_ByteTrigCfg(UARTByteTRIGTypeDef b);

void UART2_INTCfg(FunctionalState s, uint8_t i);

void UART2_Reset(void);

#define UART2_CLR_RXFIFO() (R8_UART2_FCR |= RB_FCR_RX_FIFO_CLR)

#define UART2_CLR_TXFIFO() (R8_UART2_FCR |= RB_FCR_TX_FIFO_CLR)

#define UART2_GetITFlag() (R8_UART2_IIR & RB_IIR_INT_MASK)

#define UART2_GetLinSTA() (R8_UART2_LSR)

#define UART2_SendByte(b) (R8_UART2_THR = b)

void UART2_SendString(uint8_t *buf, uint16_t l);

#define UART2_RecvByte() (R8_UART2_RBR)

uint16_t UART2_RecvString(uint8_t *buf);

void UART3_DefInit(void);

void UART3_BaudRateCfg(uint32_t baudrate);

void UART3_ByteTrigCfg(UARTByteTRIGTypeDef b);

void UART3_INTCfg(FunctionalState s, uint8_t i);

void UART3_Reset(void);

#define UART3_CLR_RXFIFO() (R8_UART3_FCR |= RB_FCR_RX_FIFO_CLR)

#define UART3_CLR_TXFIFO() (R8_UART3_FCR |= RB_FCR_TX_FIFO_CLR)

#define UART3_GetITFlag() (R8_UART3_IIR & RB_IIR_INT_MASK)

#define UART3_GetLinSTA() (R8_UART3_LSR)

#define UART3_SendByte(b) (R8_UART3_THR = b)

void UART3_SendString(uint8_t *buf, uint16_t l);

#define UART3_RecvByte() (R8_UART3_RBR)

uint16_t UART3_RecvString(uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif // __CH58x_UART_H__
