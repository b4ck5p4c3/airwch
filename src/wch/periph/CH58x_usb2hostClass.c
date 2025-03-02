/********************************** (C) COPYRIGHT
 ******************************** File Name          : CH58x_usbhost.c Author :
 *WCH Version            : V1.2 Date               : 2021/11/17 Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH58x_common.h"
#if DISK_LIB_ENABLE
#include "CHRV3UFI.H"
#endif

__attribute__((aligned(4))) const uint8_t SetupSetU2HIDIdle[] = {
    0x21, HID_SET_IDLE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

__attribute__((aligned(4))) const uint8_t SetupGetU2HIDDevReport[] = {
    0x81, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_REPORT, 0x00, 0x00, 0x41,
    0x00};

__attribute__((aligned(4)))
const uint8_t SetupGetU2HubDescr[] = {HUB_GET_HUB_DESCRIPTOR,
                                      HUB_GET_DESCRIPTOR,
                                      0x00,
                                      USB_DESCR_TYP_HUB,
                                      0x00,
                                      0x00,
                                      sizeof(USB_HUB_DESCR),
                                      0x00};

__attribute__((aligned(4))) uint8_t U2Com_Buffer[128];

uint8_t AnalyzeU2HidIntEndp(uint8_t *buf, uint8_t HubPortIndex) {
  uint8_t i, s, l;
  s = 0;

  if (HubPortIndex) {
    memset(DevOnU2HubPort[HubPortIndex - 1].GpVar, 0,
           sizeof(DevOnU2HubPort[HubPortIndex - 1].GpVar));
  } else {
    memset(ThisUsb2Dev.GpVar, 0, sizeof(ThisUsb2Dev.GpVar));
  }

  for (i = 0; i < ((PUSB_CFG_DESCR)buf)->wTotalLength; i += l) {
    if (((PUSB_ENDP_DESCR)(buf + i))->bDescriptorType == USB_DESCR_TYP_ENDP &&
        (((PUSB_ENDP_DESCR)(buf + i))->bmAttributes & USB_ENDP_TYPE_MASK) ==
            USB_ENDP_TYPE_INTER &&
        (((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_DIR_MASK)) {
      if (HubPortIndex) {
        DevOnU2HubPort[HubPortIndex - 1].GpVar[s] =
            ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
      } else {
        ThisUsb2Dev.GpVar[s] =
            ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
      }
      PRINT("%02x ", (uint16_t)ThisUsb2Dev.GpVar[s]);
      s++;
      if (s >= 4) {
        break;
      }
    }
    l = ((PUSB_ENDP_DESCR)(buf + i))->bLength;
    if (l > 16) {
      break;
    }
  }
  PRINT("\n");
  return (s);
}

uint8_t AnalyzeU2BulkEndp(uint8_t *buf, uint8_t HubPortIndex) {
  uint8_t i, s1, s2, l;
  s1 = 0;
  s2 = 2;

  if (HubPortIndex) {
    memset(DevOnU2HubPort[HubPortIndex - 1].GpVar, 0,
           sizeof(DevOnU2HubPort[HubPortIndex - 1].GpVar));
  } else {
    memset(ThisUsb2Dev.GpVar, 0, sizeof(ThisUsb2Dev.GpVar));
  }

  for (i = 0; i < ((PUSB_CFG_DESCR)buf)->wTotalLength; i += l) {
    if ((((PUSB_ENDP_DESCR)(buf + i))->bDescriptorType == USB_DESCR_TYP_ENDP) &&
        ((((PUSB_ENDP_DESCR)(buf + i))->bmAttributes & USB_ENDP_TYPE_MASK) ==
         USB_ENDP_TYPE_BULK))

    {
      if (HubPortIndex) {
        if (((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress &
            USB_ENDP_DIR_MASK) {
          DevOnU2HubPort[HubPortIndex - 1].GpVar[s1++] =
              ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress &
              USB_ENDP_ADDR_MASK;
        } else {
          DevOnU2HubPort[HubPortIndex - 1].GpVar[s2++] =
              ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress &
              USB_ENDP_ADDR_MASK;
        }
      } else {
        if (((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress &
            USB_ENDP_DIR_MASK) {
          ThisUsb2Dev.GpVar[s1++] =
              ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress &
              USB_ENDP_ADDR_MASK;
        } else {
          ThisUsb2Dev.GpVar[s2++] =
              ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress &
              USB_ENDP_ADDR_MASK;
        }
      }

      if (s1 == 2) {
        s1 = 1;
      }
      if (s2 == 4) {
        s2 = 3;
      }
    }
    l = ((PUSB_ENDP_DESCR)(buf + i))->bLength;
    if (l > 16) {
      break;
    }
  }
  return (0);
}

uint8_t InitRootU2Device(void) {
  uint8_t i, s;
  uint8_t cfg, dv_cls, if_cls;

  PRINT("Reset U2 host port\n");
  ResetRootU2HubPort();
  for (i = 0, s = 0; i < 100; i++) {
    mDelaymS(1);
    if (EnableRootU2HubPort() == ERR_SUCCESS) {
      i = 0;
      s++;
      if (s > 100)
        break;
    }
  }
  if (i) {
    DisableRootU2HubPort();
    PRINT("Disable U2 host port because of disconnect\n");
    return (ERR_USB_DISCON);
  }
  SetUsb2Speed(ThisUsb2Dev.DeviceSpeed);

  PRINT("GetU2DevDescr: ");
  s = CtrlGetU2DeviceDescr();
  if (s == ERR_SUCCESS) {
    for (i = 0; i < ((PUSB_SETUP_REQ)SetupGetU2DevDescr)->wLength; i++)
      PRINT("x%02X ", (uint16_t)(U2Com_Buffer[i]));
    PRINT("\n");

    ThisUsb2Dev.DeviceVID = ((PUSB_DEV_DESCR)U2Com_Buffer)->idVendor;
    ThisUsb2Dev.DevicePID = ((PUSB_DEV_DESCR)U2Com_Buffer)->idProduct;
    dv_cls = ((PUSB_DEV_DESCR)U2Com_Buffer)->bDeviceClass;

    s = CtrlSetUsb2Address(((PUSB_SETUP_REQ)SetupSetUsb2Addr)->wValue);
    if (s == ERR_SUCCESS) {
      ThisUsb2Dev.DeviceAddress = ((PUSB_SETUP_REQ)SetupSetUsb2Addr)->wValue;

      PRINT("GetU2CfgDescr: ");
      s = CtrlGetU2ConfigDescr();
      if (s == ERR_SUCCESS) {
        for (i = 0; i < ((PUSB_CFG_DESCR)U2Com_Buffer)->wTotalLength; i++) {
          PRINT("x%02X ", (uint16_t)(U2Com_Buffer[i]));
        }
        PRINT("\n");
        cfg = ((PUSB_CFG_DESCR)U2Com_Buffer)->bConfigurationValue;
        if_cls = ((PUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceClass;

        if ((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_STORAGE)) {
#ifdef FOR_ROOT_UDISK_ONLY
          CHRV3DiskStatus = DISK_USB_ADDR;
          return (ERR_SUCCESS);
        } else
          return (ERR_USB_UNSUPPORT);
#else
          s = CtrlSetUsb2Config(cfg);
          if (s == ERR_SUCCESS) {
            ThisUsb2Dev.DeviceStatus = ROOT_DEV_SUCCESS;
            ThisUsb2Dev.DeviceType = USB_DEV_CLASS_STORAGE;
            PRINT("U2 USB-Disk Ready\n");
            SetUsb2Speed(1);
            return (ERR_SUCCESS);
          }
        } else if ((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_PRINTER) &&
                   ((PUSB_CFG_DESCR_LONG)U2Com_Buffer)
                           ->itf_descr.bInterfaceSubClass == 0x01) {
          s = CtrlSetUsb2Config(cfg);
          if (s == ERR_SUCCESS) {
            ThisUsb2Dev.DeviceStatus = ROOT_DEV_SUCCESS;
            ThisUsb2Dev.DeviceType = USB_DEV_CLASS_PRINTER;
            PRINT("U2 USB-Print Ready\n");
            SetUsb2Speed(1);
            return (ERR_SUCCESS);
          }
        } else if ((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) &&
                   ((PUSB_CFG_DESCR_LONG)U2Com_Buffer)
                           ->itf_descr.bInterfaceSubClass <= 0x01) {
          s = AnalyzeU2HidIntEndp(U2Com_Buffer, 0);
          PRINT("AnalyzeU2HidIntEndp %02x\n", (uint16_t)s);
          if_cls =
              ((PUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceProtocol;
          s = CtrlSetUsb2Config(cfg);
          if (s == ERR_SUCCESS) {
            //	    					Set_Idle( );
            ThisUsb2Dev.DeviceStatus = ROOT_DEV_SUCCESS;
            if (if_cls == 1) {
              ThisUsb2Dev.DeviceType = DEV_TYPE_KEYBOARD;
              PRINT("U2 USB-Keyboard Ready\n");
              SetUsb2Speed(1);
              return (ERR_SUCCESS);
            } else if (if_cls == 2) {
              ThisUsb2Dev.DeviceType = DEV_TYPE_MOUSE;
              PRINT("U2 USB-Mouse Ready\n");
              SetUsb2Speed(1);
              return (ERR_SUCCESS);
            }
            s = ERR_USB_UNSUPPORT;
          }
        } else if (dv_cls == USB_DEV_CLASS_HUB) {
          s = CtrlGetU2HubDescr();
          if (s == ERR_SUCCESS) {
            PRINT("Max Port:%02X ",
                  (((PXUSB_HUB_DESCR)U2Com_Buffer)->bNbrPorts));
            ThisUsb2Dev.GpHUBPortNum =
                ((PXUSB_HUB_DESCR)U2Com_Buffer)->bNbrPorts;
            if (ThisUsb2Dev.GpHUBPortNum > HUB_MAX_PORTS) {
              ThisUsb2Dev.GpHUBPortNum = HUB_MAX_PORTS;
            }
            s = CtrlSetUsb2Config(cfg);
            if (s == ERR_SUCCESS) {
              ThisUsb2Dev.DeviceStatus = ROOT_DEV_SUCCESS;
              ThisUsb2Dev.DeviceType = USB_DEV_CLASS_HUB;
              for (i = 1; i <= ThisUsb2Dev.GpHUBPortNum; i++) {
                DevOnU2HubPort[i - 1].DeviceStatus = ROOT_DEV_DISCONNECT;
                s = U2HubSetPortFeature(i, HUB_PORT_POWER);
                if (s != ERR_SUCCESS) {
                  PRINT("Ext-HUB Port_%1d# power on error\n", (uint16_t)i);
                }
              }
              PRINT("U2 USB-HUB Ready\n");
              SetUsb2Speed(1);
              return (ERR_SUCCESS);
            }
          }
        } else {
          s = CtrlSetUsb2Config(cfg);
          if (s == ERR_SUCCESS) {
            ThisUsb2Dev.DeviceStatus = ROOT_DEV_SUCCESS;
            ThisUsb2Dev.DeviceType = DEV_TYPE_UNKNOW;
            SetUsb2Speed(1);
            return (ERR_SUCCESS);
          }
        }
#endif
      }
    }
  }

  PRINT("InitRootU2Dev Err = %02X\n", (uint16_t)s);
#ifdef FOR_ROOT_UDISK_ONLY
  CHRV3DiskStatus = DISK_CONNECT;
#else
  ThisUsb2Dev.DeviceStatus = ROOT_DEV_FAILED;
#endif
  SetUsb2Speed(1);
  return (s);
}

uint8_t InitU2DevOnHub(uint8_t HubPortIndex) {
  uint8_t i, s, cfg, dv_cls, if_cls;
  uint8_t ifc;
  PRINT("Init dev @ExtHub-port_%1d ", (uint16_t)HubPortIndex);
  if (HubPortIndex == 0) {
    return (ERR_USB_UNKNOWN);
  }
  SelectU2HubPort(HubPortIndex);
  PRINT("GetDevDescr: ");
  s = CtrlGetU2DeviceDescr();
  if (s != ERR_SUCCESS) {
    return (s);
  }
  DevOnU2HubPort[HubPortIndex - 1].DeviceVID =
      ((uint16_t)((PUSB_DEV_DESCR)U2Com_Buffer)->idVendor);
  DevOnU2HubPort[HubPortIndex - 1].DevicePID =
      ((uint16_t)((PUSB_DEV_DESCR)U2Com_Buffer)->idProduct);

  dv_cls = ((PUSB_DEV_DESCR)U2Com_Buffer)->bDeviceClass;
  cfg = (1 << 4) + HubPortIndex;
  s = CtrlSetUsb2Address(cfg);
  if (s != ERR_SUCCESS) {
    return (s);
  }
  DevOnU2HubPort[HubPortIndex - 1].DeviceAddress = cfg;
  PRINT("GetCfgDescr: ");
  s = CtrlGetU2ConfigDescr();
  if (s != ERR_SUCCESS) {
    return (s);
  }
  cfg = ((PUSB_CFG_DESCR)U2Com_Buffer)->bConfigurationValue;
  for (i = 0; i < ((PUSB_CFG_DESCR)U2Com_Buffer)->wTotalLength; i++) {
    PRINT("x%02X ", (uint16_t)(U2Com_Buffer[i]));
  }
  PRINT("\n");
  if_cls = ((PXUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceClass;
  if (dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE) {
    AnalyzeU2BulkEndp(U2Com_Buffer, HubPortIndex);
    for (i = 0; i != 4; i++) {
      PRINT("%02x ", (uint16_t)DevOnU2HubPort[HubPortIndex - 1].GpVar[i]);
    }
    PRINT("\n");
    s = CtrlSetUsb2Config(cfg);
    if (s == ERR_SUCCESS) {
      DevOnU2HubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_SUCCESS;
      DevOnU2HubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_STORAGE;
      PRINT("USB-Disk Ready\n");
      SetUsb2Speed(1);
      return (ERR_SUCCESS);
    }
  } else if ((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) &&
             (((PXUSB_CFG_DESCR_LONG)U2Com_Buffer)
                  ->itf_descr.bInterfaceSubClass <= 0x01)) {
    ifc = ((PXUSB_CFG_DESCR_LONG)U2Com_Buffer)->cfg_descr.bNumInterfaces;
    PRINT("AnalyzeU2HidIntEndp %02x\n", (uint16_t)s);
    if_cls = ((PXUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceProtocol;
    s = CtrlSetUsb2Config(cfg);
    if (s == ERR_SUCCESS) {
      for (dv_cls = 0; dv_cls < ifc; dv_cls++) {
        s = CtrlGetU2HIDDeviceReport(dv_cls);
        if (s == ERR_SUCCESS) {
          for (i = 0; i < 64; i++) {
            PRINT("x%02X ", (uint16_t)(U2Com_Buffer[i]));
          }
          PRINT("\n");
        }
      }
      DevOnU2HubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_SUCCESS;
      if (if_cls == 1) {
        DevOnU2HubPort[HubPortIndex - 1].DeviceType = DEV_TYPE_KEYBOARD;
        if (ifc > 1) {
          PRINT("USB_DEV_CLASS_HID Ready\n");
          //                    DevOnU2HubPort[HubPortIndex - 1].DeviceType =
          //                    USB_DEV_CLASS_HID;
        }
        PRINT("USB-Keyboard Ready\n");
        SetUsb2Speed(1);

        return (ERR_SUCCESS);
      } else if (if_cls == 2) {
        DevOnU2HubPort[HubPortIndex - 1].DeviceType = DEV_TYPE_MOUSE;
        if (ifc > 1) {
          PRINT("USB_DEV_CLASS_HID Ready\n");
          //                    DevOnU2HubPort[HubPortIndex - 1].DeviceType =
          //                    USB_DEV_CLASS_HID;
        }
        PRINT("USB-Mouse Ready\n");
        SetUsb2Speed(1);

        return (ERR_SUCCESS);
      }
      s = ERR_USB_UNSUPPORT;
    }
  } else if (dv_cls == USB_DEV_CLASS_HUB) {
    DevOnU2HubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_HUB;
    PRINT("This program don't support Level 2 HUB\n");
    s = U2HubClearPortFeature(i, HUB_PORT_ENABLE);
    if (s != ERR_SUCCESS) {
      return (s);
    }
    s = ERR_USB_UNSUPPORT;
  } else {
    AnalyzeU2BulkEndp(U2Com_Buffer, HubPortIndex);
    for (i = 0; i != 4; i++) {
      PRINT("%02x ", (uint16_t)DevOnU2HubPort[HubPortIndex - 1].GpVar[i]);
    }
    PRINT("\n");
    s = CtrlSetUsb2Config(cfg);
    if (s == ERR_SUCCESS) {
      DevOnU2HubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_SUCCESS;
      DevOnU2HubPort[HubPortIndex - 1].DeviceType = dv_cls ? dv_cls : if_cls;
      SetUsb2Speed(1);
      return (ERR_SUCCESS);
    }
  }
  PRINT("InitDevOnHub Err = %02X\n", (uint16_t)s);
  DevOnU2HubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_FAILED;
  SetUsb2Speed(1);
  return (s);
}

uint8_t EnumU2HubPort() {
  uint8_t i, s;

  for (i = 1; i <= ThisUsb2Dev.GpHUBPortNum; i++) {
    SelectU2HubPort(0);
    s = U2HubGetPortStatus(i);
    if (s != ERR_SUCCESS) {
      return (s);
    }
    if (((U2Com_Buffer[0] & (1 << (HUB_PORT_CONNECTION & 0x07))) &&
         (U2Com_Buffer[2] & (1 << (HUB_C_PORT_CONNECTION & 0x07)))) ||
        (U2Com_Buffer[2] == 0x10)) {
      DevOnU2HubPort[i - 1].DeviceStatus = ROOT_DEV_CONNECTED;
      DevOnU2HubPort[i - 1].DeviceAddress = 0x00;
      s = U2HubGetPortStatus(i);
      if (s != ERR_SUCCESS) {
        return (s);
      }
      DevOnU2HubPort[i - 1].DeviceSpeed =
          U2Com_Buffer[1] & (1 << (HUB_PORT_LOW_SPEED & 0x07))
              ? 0
              : 1; // ���ٻ���ȫ��
      if (DevOnU2HubPort[i - 1].DeviceSpeed) {
        PRINT("Found full speed device on port %1d\n", (uint16_t)i);
      } else {
        PRINT("Found low speed device on port %1d\n", (uint16_t)i);
      }
      mDelaymS(200);
      s = U2HubSetPortFeature(i, HUB_PORT_RESET);
      if (s != ERR_SUCCESS) {
        return (s);
      }
      PRINT("Reset port and then wait in\n");
      do {
        mDelaymS(1);
        s = U2HubGetPortStatus(i);
        if (s != ERR_SUCCESS) {
          return (s);
        }
      } while (U2Com_Buffer[0] & (1 << (HUB_PORT_RESET & 0x07)));
      mDelaymS(100);
      s = U2HubClearPortFeature(i, HUB_C_PORT_RESET);
      //             s = U2HubSetPortFeature( i, HUB_PORT_ENABLE );
      s = U2HubClearPortFeature(i, HUB_C_PORT_CONNECTION);
      if (s != ERR_SUCCESS) {
        return (s);
      }
      s = U2HubGetPortStatus(i);
      if (s != ERR_SUCCESS) {
        return (s);
      }
      if ((U2Com_Buffer[0] & (1 << (HUB_PORT_CONNECTION & 0x07))) == 0) {
        DevOnU2HubPort[i - 1].DeviceStatus = ROOT_DEV_DISCONNECT;
      }
      s = InitU2DevOnHub(i);
      if (s != ERR_SUCCESS) {
        return (s);
      }
      SetUsb2Speed(1);
    } else if (U2Com_Buffer[2] & (1 << (HUB_C_PORT_ENABLE & 0x07))) {
      U2HubClearPortFeature(i, HUB_C_PORT_ENABLE);
      PRINT("Device on port error\n");
      s = U2HubSetPortFeature(i, HUB_PORT_RESET);
      if (s != ERR_SUCCESS)
        return (s);
      do {
        mDelaymS(1);
        s = U2HubGetPortStatus(i);
        if (s != ERR_SUCCESS)
          return (s);
      } while (U2Com_Buffer[0] & (1 << (HUB_PORT_RESET & 0x07)));
    } else if ((U2Com_Buffer[0] & (1 << (HUB_PORT_CONNECTION & 0x07))) == 0) {
      if (DevOnU2HubPort[i - 1].DeviceStatus >= ROOT_DEV_CONNECTED) {
        PRINT("Device on port %1d removed\n", (uint16_t)i);
      }
      DevOnU2HubPort[i - 1].DeviceStatus = ROOT_DEV_DISCONNECT;
      if (U2Com_Buffer[2] & (1 << (HUB_C_PORT_CONNECTION & 0x07))) {
        U2HubClearPortFeature(i, HUB_C_PORT_CONNECTION);
      }
    }
  }
  return (ERR_SUCCESS);
}

uint8_t EnumAllU2HubPort(void) {
  uint8_t s;

  if ((ThisUsb2Dev.DeviceStatus >= ROOT_DEV_SUCCESS) &&
      (ThisUsb2Dev.DeviceType == USB_DEV_CLASS_HUB)) {
    SelectU2HubPort(0);
    s = EnumU2HubPort();
    if (s != ERR_SUCCESS) {
      PRINT("EnumAllHubPort err = %02X\n", (uint16_t)s);
    }
    SetUsb2Speed(1);
  }
  return (ERR_SUCCESS);
}

uint16_t U2SearchTypeDevice(uint8_t type) {
  uint8_t RootHubIndex;
  uint8_t HubPortIndex;

  RootHubIndex = 0;
  if ((ThisUsb2Dev.DeviceType == USB_DEV_CLASS_HUB) &&
      (ThisUsb2Dev.DeviceStatus >= ROOT_DEV_SUCCESS)) {
    for (HubPortIndex = 1; HubPortIndex <= ThisUsb2Dev.GpHUBPortNum;
         HubPortIndex++) {
      if (DevOnU2HubPort[HubPortIndex - 1].DeviceType == type &&
          DevOnU2HubPort[HubPortIndex - 1].DeviceStatus >= ROOT_DEV_SUCCESS) {
        return (((uint16_t)RootHubIndex << 8) | HubPortIndex);
      }
    }
  }
  if ((ThisUsb2Dev.DeviceType == type) &&
      (ThisUsb2Dev.DeviceStatus >= ROOT_DEV_SUCCESS)) {
    return ((uint16_t)RootHubIndex << 8);
  }

  return (0xFFFF);
}

uint8_t U2SETorOFFNumLock(uint8_t *buf) {
  uint8_t tmp[] = {0x21, 0x09, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00};
  uint8_t len, s;
  if ((buf[2] == 0x53) &
      ((buf[0] | buf[1] | buf[3] | buf[4] | buf[5] | buf[6] | buf[7]) == 0)) {
    for (s = 0; s != sizeof(tmp); s++) {
      ((uint8_t *)pU2SetupReq)[s] = tmp[s];
    }
    s = U2HostCtrlTransfer(U2Com_Buffer, &len);
    if (s != ERR_SUCCESS) {
      return (s);
    }
  }
  return (ERR_SUCCESS);
}

uint8_t CtrlGetU2HIDDeviceReport(uint8_t infc) {
  uint8_t s;
  uint8_t len;

  CopyU2SetupReqPkg((uint8_t *)SetupSetU2HIDIdle);
  pU2SetupReq->wIndex = infc;
  s = U2HostCtrlTransfer(U2Com_Buffer, &len);
  if (s != ERR_SUCCESS) {
    return (s);
  }

  CopyU2SetupReqPkg((uint8_t *)SetupGetU2HIDDevReport);
  pU2SetupReq->wIndex = infc;
  s = U2HostCtrlTransfer(U2Com_Buffer, &len);
  if (s != ERR_SUCCESS) {
    return (s);
  }

  return (ERR_SUCCESS);
}

uint8_t CtrlGetU2HubDescr(void) {
  uint8_t s;
  uint8_t len;

  CopyU2SetupReqPkg((uint8_t *)SetupGetU2HubDescr);
  s = U2HostCtrlTransfer(U2Com_Buffer, &len);
  if (s != ERR_SUCCESS) {
    return (s);
  }
  if (len < ((PUSB_SETUP_REQ)SetupGetU2HubDescr)->wLength) {
    return (ERR_USB_BUF_OVER); // ���������ȴ���
  }
  //  if ( len < 4 ) return( ERR_USB_BUF_OVER );
  return (ERR_SUCCESS);
}

uint8_t U2HubGetPortStatus(uint8_t HubPortIndex) {
  uint8_t s;
  uint8_t len;

  pU2SetupReq->bRequestType = HUB_GET_PORT_STATUS;
  pU2SetupReq->bRequest = HUB_GET_STATUS;
  pU2SetupReq->wValue = 0x0000;
  pU2SetupReq->wIndex = 0x0000 | HubPortIndex;
  pU2SetupReq->wLength = 0x0004;
  s = U2HostCtrlTransfer(U2Com_Buffer, &len);
  if (s != ERR_SUCCESS) {
    return (s);
  }
  if (len < 4) {
    return (ERR_USB_BUF_OVER);
  }
  return (ERR_SUCCESS);
}

uint8_t U2HubSetPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt) {
  pU2SetupReq->bRequestType = HUB_SET_PORT_FEATURE;
  pU2SetupReq->bRequest = HUB_SET_FEATURE;
  pU2SetupReq->wValue = 0x0000 | FeatureSelt;
  pU2SetupReq->wIndex = 0x0000 | HubPortIndex;
  pU2SetupReq->wLength = 0x0000;
  return (U2HostCtrlTransfer(NULL, NULL));
}

uint8_t U2HubClearPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt) {
  pU2SetupReq->bRequestType = HUB_CLEAR_PORT_FEATURE;
  pU2SetupReq->bRequest = HUB_CLEAR_FEATURE;
  pU2SetupReq->wValue = 0x0000 | FeatureSelt;
  pU2SetupReq->wIndex = 0x0000 | HubPortIndex;
  pU2SetupReq->wLength = 0x0000;
  return (U2HostCtrlTransfer(NULL, NULL));
}
