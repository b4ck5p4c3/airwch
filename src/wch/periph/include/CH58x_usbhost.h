/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH57x_usbhost.h Author :
 *WCH Version            : V1.2 Date               : 2021/11/17 Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH58x_USBHOST_H__
#define __CH58x_USBHOST_H__

#ifdef __cplusplus
extern "C" {
#endif

#if DISK_LIB_ENABLE
#if DISK_WITHOUT_USB_HUB
#define FOR_ROOT_UDISK_ONLY
#endif
#define DISK_BASE_BUF_LEN 512
#endif

#define ERR_SUCCESS 0x00
#define ERR_USB_CONNECT 0x15
#define ERR_USB_DISCON 0x16
#define ERR_USB_BUF_OVER 0x17
#define ERR_USB_DISK_ERR 0x1F
#define ERR_USB_TRANSFER 0x20
#define ERR_USB_UNSUPPORT 0xFB
#define ERR_USB_UNKNOWN 0xFE
#define ERR_AOA_PROTOCOL 0x41

#define ROOT_DEV_DISCONNECT 0
#define ROOT_DEV_CONNECTED 1
#define ROOT_DEV_FAILED 2
#define ROOT_DEV_SUCCESS 3
#define DEV_TYPE_KEYBOARD (USB_DEV_CLASS_HID | 0x20)
#define DEV_TYPE_MOUSE (USB_DEV_CLASS_HID | 0x30)
#define DEF_AOA_DEVICE 0xF0
#define DEV_TYPE_UNKNOW 0xFF

#define HUB_MAX_PORTS 4
#define WAIT_USB_TOUT_200US 800

typedef struct {
  uint8_t DeviceStatus;
  uint8_t DeviceAddress;
  uint8_t DeviceSpeed;
  uint8_t DeviceType;
  uint16_t DeviceVID;
  uint16_t DevicePID;
  uint8_t GpVar[4];
  uint8_t GpHUBPortNum;
} _RootHubDev;

typedef struct {
  UINT8 DeviceStatus;
  UINT8 DeviceAddress;
  UINT8 DeviceSpeed;
  UINT8 DeviceType;
  UINT16 DeviceVID;
  UINT16 DevicePID;
  UINT8 GpVar[4];
} _DevOnHubPort;

extern _RootHubDev ThisUsbDev;
extern _DevOnHubPort DevOnHubPort[HUB_MAX_PORTS];
extern uint8_t UsbDevEndp0Size;
extern uint8_t FoundNewDev;

extern uint8_t *pHOST_RX_RAM_Addr;
extern uint8_t *pHOST_TX_RAM_Addr;

extern _RootHubDev ThisUsb2Dev;
extern _DevOnHubPort DevOnU2HubPort[HUB_MAX_PORTS];
extern uint8_t Usb2DevEndp0Size;
extern uint8_t FoundNewU2Dev;

extern uint8_t *pU2HOST_RX_RAM_Addr;
extern uint8_t *pU2HOST_TX_RAM_Addr;

#define pSetupReq ((PUSB_SETUP_REQ)pHOST_TX_RAM_Addr)
#define pU2SetupReq ((PUSB_SETUP_REQ)pU2HOST_TX_RAM_Addr)
extern uint8_t Com_Buffer[];
extern uint8_t U2Com_Buffer[];

extern const uint8_t SetupGetDevDescr[];
extern const uint8_t SetupGetCfgDescr[];
extern const uint8_t SetupSetUsbAddr[];
extern const uint8_t SetupSetUsbConfig[];
extern const uint8_t SetupSetUsbInterface[];
extern const uint8_t SetupClrEndpStall[];

extern const uint8_t SetupGetU2DevDescr[];
extern const uint8_t SetupGetU2CfgDescr[];
extern const uint8_t SetupSetUsb2Addr[];
extern const uint8_t SetupSetUsb2Config[];
extern const uint8_t SetupSetUsb2Interface[];
extern const uint8_t SetupClrU2EndpStall[];

void DisableRootHubPort(void);

uint8_t AnalyzeRootHub(void);

void SetHostUsbAddr(uint8_t addr);

void SetUsbSpeed(uint8_t FullSpeed);

void ResetRootHubPort(void);

uint8_t EnableRootHubPort(void);

uint8_t WaitUSB_Interrupt(void);

uint8_t USBHostTransact(uint8_t endp_pid, uint8_t tog, uint32_t timeout);

uint8_t HostCtrlTransfer(uint8_t *DataBuf, uint8_t *RetLen);

void CopySetupReqPkg(const uint8_t *pReqPkt);

uint8_t CtrlGetDeviceDescr(void);

uint8_t CtrlGetConfigDescr(void);

uint8_t CtrlSetUsbAddress(uint8_t addr);

uint8_t CtrlSetUsbConfig(uint8_t cfg);

uint8_t CtrlClearEndpStall(uint8_t endp);

uint8_t CtrlSetUsbIntercace(uint8_t cfg);

void USB_HostInit(void);
uint8_t EnumAllHubPort(void);
void SelectHubPort(uint8_t HubPortIndex);
uint16_t SearchTypeDevice(uint8_t type);
uint8_t SETorOFFNumLock(uint8_t *buf);

void DisableRootU2HubPort(void);
uint8_t AnalyzeRootU2Hub(void);
void SetHostUsb2Addr(uint8_t addr);
void SetUsb2Speed(uint8_t FullSpeed);
void ResetRootU2HubPort(void);
uint8_t EnableRootU2HubPort(void);
void SelectU2HubPort(uint8_t HubPortIndex);
uint8_t WaitUSB2_Interrupt(void);
uint8_t USB2HostTransact(uint8_t endp_pid, uint8_t tog, UINT32 timeout);
uint8_t U2HostCtrlTransfer(uint8_t *DataBuf, uint8_t *RetLen);

void CopyU2SetupReqPkg(const uint8_t *pReqPkt);
uint8_t CtrlGetU2DeviceDescr(void);
uint8_t CtrlGetU2ConfigDescr(void);
uint8_t CtrlSetUsb2Address(uint8_t addr);
uint8_t CtrlSetUsb2Config(uint8_t cfg);
uint8_t CtrlClearU2EndpStall(uint8_t endp);
uint8_t CtrlSetUsb2Intercace(uint8_t cfg);

void USB2_HostInit(void);

/*************************************************************/

uint8_t InitRootDevice(void);

uint8_t CtrlGetHIDDeviceReport(uint8_t infc);

uint8_t CtrlGetHubDescr(void);

uint8_t HubGetPortStatus(uint8_t HubPortIndex);

uint8_t HubSetPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt);

uint8_t HubClearPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt);

uint8_t InitRootU2Device(void);
uint8_t EnumAllU2HubPort(void);
uint16_t U2SearchTypeDevice(uint8_t type);
uint8_t U2SETorOFFNumLock(uint8_t *buf);

uint8_t CtrlGetU2HIDDeviceReport(uint8_t infc);
uint8_t CtrlGetU2HubDescr(void);
uint8_t U2HubGetPortStatus(uint8_t HubPortIndex);
uint8_t U2HubSetPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt);
uint8_t U2HubClearPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt);

#ifdef __cplusplus
}
#endif

#endif // __CH58x_USBHOST_H__
