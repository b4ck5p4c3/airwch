/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH57x_usbdev.h Author :
 *WCH Version            : V1.2 Date               : 2021/11/17 Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH58x_USBDEV_H__
#define __CH58x_USBDEV_H__

#ifdef __cplusplus
extern "C" {
#endif

#define DEF_USB_GET_IDLE 0x02     /* get idle for key or mouse */
#define DEF_USB_GET_PROTOCOL 0x03 /* get protocol for bios type */
#define DEF_USB_SET_REPORT 0x09   /* set report for key */
#define DEF_USB_SET_IDLE 0x0A     /* set idle for key or mouse */
#define DEF_USB_SET_PROTOCOL 0x0B /* set protocol for bios type */

extern uint8_t *pEP0_RAM_Addr; // ep0(64)+ep4_out(64)+ep4_in(64)
extern uint8_t *pEP1_RAM_Addr; // ep1_out(64)+ep1_in(64)
extern uint8_t *pEP2_RAM_Addr; // ep2_out(64)+ep2_in(64)
extern uint8_t *pEP3_RAM_Addr; // ep3_out(64)+ep3_in(64)

extern uint8_t *pU2EP0_RAM_Addr; // ep0(64)+ep4_out(64)+ep4_in(64)
extern uint8_t *pU2EP1_RAM_Addr; // ep1_out(64)+ep1_in(64)
extern uint8_t *pU2EP2_RAM_Addr; // ep2_out(64)+ep2_in(64)
extern uint8_t *pU2EP3_RAM_Addr; // ep3_out(64)+ep3_in(64)

#define pSetupReqPak ((PUSB_SETUP_REQ)pEP0_RAM_Addr)
#define pEP0_DataBuf (pEP0_RAM_Addr)
#define pEP1_OUT_DataBuf (pEP1_RAM_Addr)
#define pEP1_IN_DataBuf (pEP1_RAM_Addr + 64)
#define pEP2_OUT_DataBuf (pEP2_RAM_Addr)
#define pEP2_IN_DataBuf (pEP2_RAM_Addr + 64)
#define pEP3_OUT_DataBuf (pEP3_RAM_Addr)
#define pEP3_IN_DataBuf (pEP3_RAM_Addr + 64)
#define pEP4_OUT_DataBuf (pEP0_RAM_Addr + 64)
#define pEP4_IN_DataBuf (pEP0_RAM_Addr + 128)

#define pU2SetupReqPak ((PUSB_SETUP_REQ)pU2EP0_RAM_Addr)
#define pU2EP0_DataBuf (pU2EP0_RAM_Addr)
#define pU2EP1_OUT_DataBuf (pU2EP1_RAM_Addr)
#define pU2EP1_IN_DataBuf (pU2EP1_RAM_Addr + 64)
#define pU2EP2_OUT_DataBuf (pU2EP2_RAM_Addr)
#define pU2EP2_IN_DataBuf (pU2EP2_RAM_Addr + 64)
#define pU2EP3_OUT_DataBuf (pU2EP3_RAM_Addr)
#define pU2EP3_IN_DataBuf (pU2EP3_RAM_Addr + 64)
#define pU2EP4_OUT_DataBuf (pU2EP0_RAM_Addr + 64)
#define pU2EP4_IN_DataBuf (pU2EP0_RAM_Addr + 128)

void USB_DeviceInit(void);

void USB_DevTransProcess(void);

void DevEP1_OUT_Deal(uint8_t l);

void DevEP2_OUT_Deal(uint8_t l);

void DevEP3_OUT_Deal(uint8_t l);

void DevEP4_OUT_Deal(uint8_t l);

void DevEP1_IN_Deal(uint8_t l);

void DevEP2_IN_Deal(uint8_t l);

void DevEP3_IN_Deal(uint8_t l);

void DevEP4_IN_Deal(uint8_t l);

#define EP1_GetINSta() (R8_UEP1_CTRL & UEP_T_RES_NAK)

#define EP2_GetINSta() (R8_UEP2_CTRL & UEP_T_RES_NAK)

#define EP3_GetINSta() (R8_UEP3_CTRL & UEP_T_RES_NAK)

#define EP4_GetINSta() (R8_UEP4_CTRL & UEP_T_RES_NAK)

void USB2_DeviceInit(void);
void USB2_DevTransProcess(void);

void U2DevEP1_OUT_Deal(uint8_t l);
void U2DevEP2_OUT_Deal(uint8_t l);
void U2DevEP3_OUT_Deal(uint8_t l);
void U2DevEP4_OUT_Deal(uint8_t l);

void U2DevEP1_IN_Deal(uint8_t l);
void U2DevEP2_IN_Deal(uint8_t l);
void U2DevEP3_IN_Deal(uint8_t l);
void U2DevEP4_IN_Deal(uint8_t l);

#define U2EP1_GetINSta() (R8_U2EP1_CTRL & UEP_T_RES_NAK)
#define U2EP2_GetINSta() (R8_U2EP2_CTRL & UEP_T_RES_NAK)
#define U2EP3_GetINSta() (R8_U2EP3_CTRL & UEP_T_RES_NAK)
#define U2EP4_GetINSta() (R8_U2EP4_CTRL & UEP_T_RES_NAK)

#ifdef __cplusplus
}
#endif

#endif // __CH58x_USBDEV_H__
