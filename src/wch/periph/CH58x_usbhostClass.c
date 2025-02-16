/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH58x_usbhost.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH58x_common.h"
#if DISK_LIB_ENABLE
  #include "CHRV3UFI.h"
#endif

/* ����HID�ϴ����� */
__attribute__((aligned(4))) const uint8_t SetupSetHIDIdle[] = {0x21, HID_SET_IDLE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
/* ��ȡHID�豸���������� */
__attribute__((aligned(4))) const uint8_t SetupGetHIDDevReport[] = {0x81, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_REPORT,
                                                                    0x00, 0x00, 0x41, 0x00};
/* ��ȡHUB������ */
__attribute__((aligned(4))) const uint8_t SetupGetHubDescr[] = {HUB_GET_HUB_DESCRIPTOR, HUB_GET_DESCRIPTOR, 0x00,
                                                                USB_DESCR_TYP_HUB, 0x00, 0x00, sizeof(USB_HUB_DESCR), 0x00};

__attribute__((aligned(4))) uint8_t Com_Buffer[128]; // �����û���ʱ������,ö��ʱ���ڴ���������,ö�ٽ���Ҳ����������ͨ��ʱ������

/*********************************************************************
 * @fn      AnalyzeHidIntEndp
 *
 * @brief   ���������з�����HID�ж϶˵�ĵ��?,���HubPortIndex��0���浽ROOTHUB������Ƿ���ֵ�򱣴浽HUB�½ṹ��
 *
 * @param   buf     - ���������ݻ�������ַ HubPortIndex��0��ʾ��HUB����0��ʾ�ⲿHUB�µĶ˿ں�
 *
 * @return  �˵���
 */
uint8_t AnalyzeHidIntEndp(uint8_t *buf, uint8_t HubPortIndex)
{
    uint8_t i, s, l;
    s = 0;

    if(HubPortIndex)
    {
        memset(DevOnHubPort[HubPortIndex - 1].GpVar, 0, sizeof(DevOnHubPort[HubPortIndex - 1].GpVar)); //�������?
    }
    else
    {
        memset(ThisUsbDev.GpVar, 0, sizeof(ThisUsbDev.GpVar)); //�������?
    }

    for(i = 0; i < ((PUSB_CFG_DESCR)buf)->wTotalLength; i += l) // �����ж϶˵�������,���������������ͽӿ�������
    {
        if(((PUSB_ENDP_DESCR)(buf + i))->bDescriptorType == USB_DESCR_TYP_ENDP                         // �Ƕ˵�������
           && (((PUSB_ENDP_DESCR)(buf + i))->bmAttributes & USB_ENDP_TYPE_MASK) == USB_ENDP_TYPE_INTER // ���ж϶˵�
           && (((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_DIR_MASK))                    // ��IN�˵�
        {                                                                                              // �����ж϶˵�ĵ��?,λ7����ͬ����־λ,��0
            if(HubPortIndex)
            {
                DevOnHubPort[HubPortIndex - 1].GpVar[s] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
            }
            else
            {
                ThisUsbDev.GpVar[s] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK; // �ж϶˵�ĵ�ַ�����Ը������?����wMaxPacketSize��bInterval
            }
            PRINT("%02x ", (uint16_t)ThisUsbDev.GpVar[s]);
            s++;
            if(s >= 4)
            {
                break; //ֻ����4���˵�
            }
        }
        l = ((PUSB_ENDP_DESCR)(buf + i))->bLength; // ��ǰ����������,����
        if(l > 16)
        {
            break;
        }
    }
    PRINT("\n");
    return (s);
}

/*********************************************************************
 * @fn      AnalyzeBulkEndp
 *
 * @brief   �����������˵�,GpVar[0]��GpVar[1]����ϴ��˵�?GpVar[2]��GpVar[3]����´��˵�?
 *
 * @param   buf     - ���������ݻ�������ַ HubPortIndex��0��ʾ��HUB����0��ʾ�ⲿHUB�µĶ˿ں�
 *
 * @return  0
 */
uint8_t AnalyzeBulkEndp(uint8_t *buf, uint8_t HubPortIndex)
{
    uint8_t i, s1, s2, l;
    s1 = 0;
    s2 = 2;

    if(HubPortIndex)
    {
        memset(DevOnHubPort[HubPortIndex - 1].GpVar, 0, sizeof(DevOnHubPort[HubPortIndex - 1].GpVar)); //�������?
    }
    else
    {
        memset(ThisUsbDev.GpVar, 0, sizeof(ThisUsbDev.GpVar)); //�������?
    }

    for(i = 0; i < ((PUSB_CFG_DESCR)buf)->wTotalLength; i += l) // �����ж϶˵�������,���������������ͽӿ�������
    {
        if((((PUSB_ENDP_DESCR)(buf + i))->bDescriptorType == USB_DESCR_TYP_ENDP)                         // �Ƕ˵�������
           && ((((PUSB_ENDP_DESCR)(buf + i))->bmAttributes & USB_ENDP_TYPE_MASK) == USB_ENDP_TYPE_BULK)) // ���ж϶˵�

        {
            if(HubPortIndex)
            {
                if(((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_DIR_MASK)
                {
                    DevOnHubPort[HubPortIndex - 1].GpVar[s1++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
                }
                else
                {
                    DevOnHubPort[HubPortIndex - 1].GpVar[s2++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
                }
            }
            else
            {
                if(((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_DIR_MASK)
                {
                    ThisUsbDev.GpVar[s1++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
                }
                else
                {
                    ThisUsbDev.GpVar[s2++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
                }
            }

            if(s1 == 2)
            {
                s1 = 1;
            }
            if(s2 == 4)
            {
                s2 = 3;
            }
        }
        l = ((PUSB_ENDP_DESCR)(buf + i))->bLength; // ��ǰ����������,����
        if(l > 16)
        {
            break;
        }
    }
    return (0);
}

/*********************************************************************
 * @fn      InitRootDevice
 *
 * @brief   ��ʼ��ָ��ROOT-HUB�˿ڵ�USB�豸
 *
 * @param   none
 *
 * @return  ������
 */
uint8_t InitRootDevice(void)
{
    uint8_t i, s;
    uint8_t cfg, dv_cls, if_cls;

    PRINT("Reset host port\n");
    ResetRootHubPort(); // ��⵽�豸��?,��λ��Ӧ�˿ڵ�USB����
    for(i = 0, s = 0; i < 100; i++)
    { // �ȴ�USB�豸��λ����������,100mS��ʱ
        mDelaymS(1);
        if(EnableRootHubPort() == ERR_SUCCESS)
        { // ʹ�ܶ˿�
            i = 0;
            s++;
            if(s > 100)
            {
                break; // �Ѿ��ȶ�����100mS
            }
        }
    }
    if(i)
    { // ��λ���豸û������
        DisableRootHubPort();
        PRINT("Disable host port because of disconnect\n");
        return (ERR_USB_DISCON);
    }
    SetUsbSpeed(ThisUsbDev.DeviceSpeed); // ���õ�ǰUSB�ٶ�

    PRINT("GetDevDescr: ");
    s = CtrlGetDeviceDescr(); // ��ȡ�豸������
    if(s == ERR_SUCCESS)
    {
        for(i = 0; i < ((PUSB_SETUP_REQ)SetupGetDevDescr)->wLength; i++)
        {
            PRINT("x%02X ", (uint16_t)(Com_Buffer[i]));
        }
        PRINT("\n");

        ThisUsbDev.DeviceVID = ((PUSB_DEV_DESCR)Com_Buffer)->idVendor; //����VID PID��Ϣ
        ThisUsbDev.DevicePID = ((PUSB_DEV_DESCR)Com_Buffer)->idProduct;
        dv_cls = ((PUSB_DEV_DESCR)Com_Buffer)->bDeviceClass;

        s = CtrlSetUsbAddress(((PUSB_SETUP_REQ)SetupSetUsbAddr)->wValue);
        if(s == ERR_SUCCESS)
        {
            ThisUsbDev.DeviceAddress = ((PUSB_SETUP_REQ)SetupSetUsbAddr)->wValue; // ����USB��ַ

            PRINT("GetCfgDescr: ");
            s = CtrlGetConfigDescr();
            if(s == ERR_SUCCESS)
            {
                for(i = 0; i < ((PUSB_CFG_DESCR)Com_Buffer)->wTotalLength; i++)
                {
                    PRINT("x%02X ", (uint16_t)(Com_Buffer[i]));
                }
                PRINT("\n");
                /* ��������������,��ȡ�˵�����/���˵���?/���˵��С��?,���±���endp_addr��endp_size�� */
                cfg = ((PUSB_CFG_DESCR)Com_Buffer)->bConfigurationValue;
                if_cls = ((PUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceClass; // �ӿ������?

                if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_STORAGE))
                { // ��USB�洢���豸,������ȷ����U��
#ifdef FOR_ROOT_UDISK_ONLY
                    CHRV3DiskStatus = DISK_USB_ADDR;
                    return (ERR_SUCCESS);
                }
                else
                {
                    return (ERR_USB_UNSUPPORT);
                }
#else
                    s = CtrlSetUsbConfig(cfg); // ����USB�豸����
                    if(s == ERR_SUCCESS)
                    {
                        ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsbDev.DeviceType = USB_DEV_CLASS_STORAGE;
                        PRINT("USB-Disk Ready\n");
                        SetUsbSpeed(1); // Ĭ��Ϊȫ��
                        return (ERR_SUCCESS);
                    }
                }
                else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_PRINTER) && ((PUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceSubClass == 0x01)
                {                              // �Ǵ�ӡ�����豸
                    s = CtrlSetUsbConfig(cfg); // ����USB�豸����
                    if(s == ERR_SUCCESS)
                    {
                        //	�豣��˵����?�Ա����������USB����
                        ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsbDev.DeviceType = USB_DEV_CLASS_PRINTER;
                        PRINT("USB-Print Ready\n");
                        SetUsbSpeed(1); // Ĭ��Ϊȫ��
                        return (ERR_SUCCESS);
                    }
                }
                else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) && ((PUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceSubClass <= 0x01)
                { // ��HID���豸,����/����
                    //  ���������з�����HID�ж϶˵�ĵ��?
                    s = AnalyzeHidIntEndp(Com_Buffer, 0); // ���������з�����HID�ж϶˵�ĵ��?
                    PRINT("AnalyzeHidIntEndp %02x\n", (uint16_t)s);
                    //  �����ж϶˵�ĵ��?,λ7����ͬ����־λ,��0
                    if_cls = ((PUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceProtocol;
                    s = CtrlSetUsbConfig(cfg); // ����USB�豸����
                    if(s == ERR_SUCCESS)
                    {
                        //	    					Set_Idle( );
                        //	�豣��˵����?�Ա����������USB����
                        ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                        if(if_cls == 1)
                        {
                            ThisUsbDev.DeviceType = DEV_TYPE_KEYBOARD;
                            //	��һ����ʼ��,�����豸����ָʾ��LED��
                            PRINT("USB-Keyboard Ready\n");
                            SetUsbSpeed(1); // Ĭ��Ϊȫ��
                            return (ERR_SUCCESS);
                        }
                        else if(if_cls == 2)
                        {
                            ThisUsbDev.DeviceType = DEV_TYPE_MOUSE;
                            //	Ϊ���Ժ���?���״�?,Ӧ�÷���������,ȡ���ж϶˿ڵĵ�ַ,���ȵ���Ϣ
                            PRINT("USB-Mouse Ready\n");
                            SetUsbSpeed(1); // Ĭ��Ϊȫ��
                            return (ERR_SUCCESS);
                        }
                        s = ERR_USB_UNSUPPORT;
                    }
                }
                else if(dv_cls == USB_DEV_CLASS_HUB)
                { // ��HUB���豸,��������
                    s = CtrlGetHubDescr();
                    if(s == ERR_SUCCESS)
                    {
                        PRINT("Max Port:%02X ", (((PXUSB_HUB_DESCR)Com_Buffer)->bNbrPorts));
                        ThisUsbDev.GpHUBPortNum = ((PXUSB_HUB_DESCR)Com_Buffer)->bNbrPorts; // ����HUB�Ķ˿�����
                        if(ThisUsbDev.GpHUBPortNum > HUB_MAX_PORTS)
                        {
                            ThisUsbDev.GpHUBPortNum = HUB_MAX_PORTS; // ��Ϊ����ṹDevOnHubPortʱ��Ϊ�ٶ�ÿ��HUB������HUB_MAX_PORTS���˿�
                        }
                        s = CtrlSetUsbConfig(cfg); // ����USB�豸����
                        if(s == ERR_SUCCESS)
                        {
                            ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                            ThisUsbDev.DeviceType = USB_DEV_CLASS_HUB;
                            //�豣��˵����?�Ա����������USB����,�����ж϶˵������HUB�¼�֪ͨ,��������ʹ�ò�ѯ״̬���ƴ������?
                            //��HUB���˿��ϵ�,��ѯ���˿�״̬,��ʼ�����豸���ӵ�HUB�˿�,��ʼ���豸
                            for(i = 1; i <= ThisUsbDev.GpHUBPortNum; i++) // ��HUB���˿ڶ��ϵ�
                            {
                                DevOnHubPort[i - 1].DeviceStatus = ROOT_DEV_DISCONNECT; // ���ⲿHUB�˿����豸��״̬
                                s = HubSetPortFeature(i, HUB_PORT_POWER);
                                if(s != ERR_SUCCESS)
                                {
                                    PRINT("Ext-HUB Port_%1d# power on error\n", (uint16_t)i); // �˿��ϵ�ʧ��
                                }
                            }
                            PRINT("USB-HUB Ready\n");
                            SetUsbSpeed(1); // Ĭ��Ϊȫ��
                            return (ERR_SUCCESS);
                        }
                    }
                }
                else
                {                              // ���Խ�һ������
                    s = CtrlSetUsbConfig(cfg); // ����USB�豸����
                    if(s == ERR_SUCCESS)
                    {
                        //	�豣��˵����?�Ա����������USB����
                        ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsbDev.DeviceType = DEV_TYPE_UNKNOW;
                        SetUsbSpeed(1);       // Ĭ��Ϊȫ��
                        return (ERR_SUCCESS); /* δ֪�豸��ʼ���ɹ� */
                    }
                }
#endif
            }
        }
    }

    PRINT("InitRootDev Err = %02X\n", (uint16_t)s);
#ifdef FOR_ROOT_UDISK_ONLY
    CHRV3DiskStatus = DISK_CONNECT;
#else
    ThisUsbDev.DeviceStatus = ROOT_DEV_FAILED;
#endif
    SetUsbSpeed(1); // Ĭ��Ϊȫ��
    return (s);
}

/*********************************************************************
 * @fn      InitDevOnHub
 *
 * @brief   ��ʼ��ö���ⲿHUB��Ķ���USB�豸
 *
 * @param   HubPortIndex    - ָ���ⲿHUB
 *
 * @return  ������
 */
uint8_t InitDevOnHub(uint8_t HubPortIndex)
{
    uint8_t i, s, cfg, dv_cls, if_cls;
    uint8_t ifc;
    PRINT("Init dev @ExtHub-port_%1d ", (uint16_t)HubPortIndex);
    if(HubPortIndex == 0)
    {
        return (ERR_USB_UNKNOWN);
    }
    SelectHubPort(HubPortIndex); // ѡ�����ָ����ROOT-HUB�˿ڵ��ⲿHUB��ָ���˿�,ѡ���ٶ�
    PRINT("GetDevDescr: ");
    s = CtrlGetDeviceDescr(); // ��ȡ�豸������
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    DevOnHubPort[HubPortIndex - 1].DeviceVID = ((uint16_t)((PUSB_DEV_DESCR)Com_Buffer)->idVendor); //����VID PID��Ϣ
    DevOnHubPort[HubPortIndex - 1].DevicePID = ((uint16_t)((PUSB_DEV_DESCR)Com_Buffer)->idProduct);

    dv_cls = ((PUSB_DEV_DESCR)Com_Buffer)->bDeviceClass; // �豸�����?
    cfg = (1 << 4) + HubPortIndex;                       // �����һ��USB��ַ,�����ַ�ص�?
    s = CtrlSetUsbAddress(cfg);                          // ����USB�豸��ַ
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    DevOnHubPort[HubPortIndex - 1].DeviceAddress = cfg; // ��������USB��ַ
    PRINT("GetCfgDescr: ");
    s = CtrlGetConfigDescr(); // ��ȡ����������
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    cfg = ((PUSB_CFG_DESCR)Com_Buffer)->bConfigurationValue;
    for(i = 0; i < ((PUSB_CFG_DESCR)Com_Buffer)->wTotalLength; i++)
    {
        PRINT("x%02X ", (uint16_t)(Com_Buffer[i]));
    }
    PRINT("\n");
    /* ��������������,��ȡ�˵�����/���˵���?/���˵��С��?,���±���endp_addr��endp_size�� */
    if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceClass; // �ӿ������?
    if(dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE)                   // ��USB�洢���豸,������ȷ����U��
    {
        AnalyzeBulkEndp(Com_Buffer, HubPortIndex);
        for(i = 0; i != 4; i++)
        {
            PRINT("%02x ", (uint16_t)DevOnHubPort[HubPortIndex - 1].GpVar[i]);
        }
        PRINT("\n");
        s = CtrlSetUsbConfig(cfg); // ����USB�豸����
        if(s == ERR_SUCCESS)
        {
            DevOnHubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_SUCCESS;
            DevOnHubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_STORAGE;
            PRINT("USB-Disk Ready\n");
            SetUsbSpeed(1); // Ĭ��Ϊȫ��
            return (ERR_SUCCESS);
        }
    }
    else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) && (((PXUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceSubClass <= 0x01)) // ��HID���豸,����/����
    {
        ifc = ((PXUSB_CFG_DESCR_LONG)Com_Buffer)->cfg_descr.bNumInterfaces;
        s = AnalyzeHidIntEndp(Com_Buffer, HubPortIndex); // ���������з�����HID�ж϶˵�ĵ��?
        PRINT("AnalyzeHidIntEndp %02x\n", (uint16_t)s);
        if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceProtocol;
        s = CtrlSetUsbConfig(cfg); // ����USB�豸����
        if(s == ERR_SUCCESS)
        {
            for(dv_cls = 0; dv_cls < ifc; dv_cls++)
            {
                s = CtrlGetHIDDeviceReport(dv_cls); //��ȡ����������
                if(s == ERR_SUCCESS)
                {
                    for(i = 0; i < 64; i++)
                    {
                        PRINT("x%02X ", (uint16_t)(Com_Buffer[i]));
                    }
                    PRINT("\n");
                }
            }
            //�豣��˵����?�Ա����������USB����
            DevOnHubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_SUCCESS;
            if(if_cls == 1)
            {
                DevOnHubPort[HubPortIndex - 1].DeviceType = DEV_TYPE_KEYBOARD;
                //��һ����ʼ��,�����豸����ָʾ��LED��
                if(ifc > 1)
                {
                    PRINT("USB_DEV_CLASS_HID Ready\n");
                    DevOnHubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_HID; //����HID�豸
                }
                PRINT("USB-Keyboard Ready\n");
                SetUsbSpeed(1); // Ĭ��Ϊȫ��

                return (ERR_SUCCESS);
            }
            else if(if_cls == 2)
            {
                DevOnHubPort[HubPortIndex - 1].DeviceType = DEV_TYPE_MOUSE;
                //Ϊ���Ժ���?���״�?,Ӧ�÷���������,ȡ���ж϶˿ڵĵ�ַ,���ȵ���Ϣ
                if(ifc > 1)
                {
                    PRINT("USB_DEV_CLASS_HID Ready\n");
                    DevOnHubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_HID; //����HID�豸
                }
                PRINT("USB-Mouse Ready\n");
                SetUsbSpeed(1); // Ĭ��Ϊȫ��

                return (ERR_SUCCESS);
            }
            s = ERR_USB_UNSUPPORT;
        }
    }
    else if(dv_cls == USB_DEV_CLASS_HUB) // ��HUB���豸,��������
    {
        DevOnHubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_HUB;
        PRINT("This program don't support Level 2 HUB\n"); // ��Ҫ֧�ֶ༶HUB������ο������������չ
        s = HubClearPortFeature(i, HUB_PORT_ENABLE);       // ��ֹHUB�˿�
        if(s != ERR_SUCCESS)
        {
            return (s);
        }
        s = ERR_USB_UNSUPPORT;
    }
    else //�����豸
    {
        AnalyzeBulkEndp(Com_Buffer, HubPortIndex); //�����������˵�
        for(i = 0; i != 4; i++)
        {
            PRINT("%02x ", (uint16_t)DevOnHubPort[HubPortIndex - 1].GpVar[i]);
        }
        PRINT("\n");
        s = CtrlSetUsbConfig(cfg); // ����USB�豸����
        if(s == ERR_SUCCESS)
        {
            //�豣��˵����?�Ա����������USB����
            DevOnHubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_SUCCESS;
            DevOnHubPort[HubPortIndex - 1].DeviceType = dv_cls ? dv_cls : if_cls;
            SetUsbSpeed(1);       // Ĭ��Ϊȫ��
            return (ERR_SUCCESS); //δ֪�豸��ʼ���ɹ�
        }
    }
    PRINT("InitDevOnHub Err = %02X\n", (uint16_t)s);
    DevOnHubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_FAILED;
    SetUsbSpeed(1); // Ĭ��Ϊȫ��
    return (s);
}

/*********************************************************************
 * @fn      EnumHubPort
 *
 * @brief   ö��ָ��ROOT-HUB�˿��ϵ��ⲿHUB�������ĸ����˿�,�����˿��������ӻ��Ƴ��¼�����ʼ������USB�豸
 *
 * @param   RootHubIndex    - ROOT_HUB0��ROOT_HUB1
 *
 * @return  ������
 */
uint8_t EnumHubPort()
{
    uint8_t i, s;

    for(i = 1; i <= ThisUsbDev.GpHUBPortNum; i++) // ��ѯ�������Ķ˿��Ƿ��б仯
    {
        SelectHubPort(0);        // ѡ�����ָ����ROOT-HUB�˿�,���õ�ǰUSB�ٶ��Լ��������豸��USB��ַ
        s = HubGetPortStatus(i); // ��ȡ�˿�״̬
        if(s != ERR_SUCCESS)
        {
            return (s); // �����Ǹ�HUB�Ͽ���
        }
        if(((Com_Buffer[0] & (1 << (HUB_PORT_CONNECTION & 0x07))) && (Com_Buffer[2] & (1 << (HUB_C_PORT_CONNECTION & 0x07)))) || (Com_Buffer[2] == 0x10))
        {                                                          // �������豸����
            DevOnHubPort[i - 1].DeviceStatus = ROOT_DEV_CONNECTED; // ���豸����
            DevOnHubPort[i - 1].DeviceAddress = 0x00;
            s = HubGetPortStatus(i); // ��ȡ�˿�״̬
            if(s != ERR_SUCCESS)
            {
                return (s); // �����Ǹ�HUB�Ͽ���
            }
            DevOnHubPort[i - 1].DeviceSpeed = Com_Buffer[1] & (1 << (HUB_PORT_LOW_SPEED & 0x07)) ? 0 : 1; // ���ٻ���ȫ��
            if(DevOnHubPort[i - 1].DeviceSpeed)
            {
                PRINT("Found full speed device on port %1d\n", (uint16_t)i);
            }
            else
            {
                PRINT("Found low speed device on port %1d\n", (uint16_t)i);
            }
            mDelaymS(200);                            // �ȴ��豸�ϵ��ȶ�
            s = HubSetPortFeature(i, HUB_PORT_RESET); // �����豸���ӵĶ˿ڸ�λ
            if(s != ERR_SUCCESS)
            {
                return (s); // �����Ǹ�HUB�Ͽ���
            }
            PRINT("Reset port and then wait in\n");
            do // ��ѯ��λ�˿�,ֱ����λ���?,����ɺ��״̬��ʾ����
            {
                mDelaymS(1);
                s = HubGetPortStatus(i);
                if(s != ERR_SUCCESS)
                {
                    return (s); // �����Ǹ�HUB�Ͽ���
                }
            } while(Com_Buffer[0] & (1 << (HUB_PORT_RESET & 0x07))); // �˿����ڸ�λ��ȴ�?
            mDelaymS(100);
            s = HubClearPortFeature(i, HUB_C_PORT_RESET);      // �����λ��ɱ�־
                                                               //             s = HubSetPortFeature( i, HUB_PORT_ENABLE );                              // ����HUB�˿�
            s = HubClearPortFeature(i, HUB_C_PORT_CONNECTION); // ������ӻ��Ƴ���?��־
            if(s != ERR_SUCCESS)
            {
                return (s);
            }
            s = HubGetPortStatus(i); // �ٶ�ȡ״̬,�����豸�Ƿ���
            if(s != ERR_SUCCESS)
            {
                return (s);
            }
            if((Com_Buffer[0] & (1 << (HUB_PORT_CONNECTION & 0x07))) == 0)
            {
                DevOnHubPort[i - 1].DeviceStatus = ROOT_DEV_DISCONNECT; // �豸������
            }
            s = InitDevOnHub(i); // ��ʼ������USB�豸
            if(s != ERR_SUCCESS)
            {
                return (s);
            }
            SetUsbSpeed(1); // Ĭ��Ϊȫ��
        }
        else if(Com_Buffer[2] & (1 << (HUB_C_PORT_ENABLE & 0x07))) // �豸���ӳ���
        {
            HubClearPortFeature(i, HUB_C_PORT_ENABLE); // ������Ӵ����־
            PRINT("Device on port error\n");
            s = HubSetPortFeature(i, HUB_PORT_RESET); // �����豸���ӵĶ˿ڸ�λ
            if(s != ERR_SUCCESS)
                return (s); // �����Ǹ�HUB�Ͽ���
            do              // ��ѯ��λ�˿�,ֱ����λ���?,����ɺ��״̬��ʾ����
            {
                mDelaymS(1);
                s = HubGetPortStatus(i);
                if(s != ERR_SUCCESS)
                    return (s);                                      // �����Ǹ�HUB�Ͽ���
            } while(Com_Buffer[0] & (1 << (HUB_PORT_RESET & 0x07))); // �˿����ڸ�λ��ȴ�?
        }
        else if((Com_Buffer[0] & (1 << (HUB_PORT_CONNECTION & 0x07))) == 0) // �豸�Ѿ��Ͽ�
        {
            if(DevOnHubPort[i - 1].DeviceStatus >= ROOT_DEV_CONNECTED)
            {
                PRINT("Device on port %1d removed\n", (uint16_t)i);
            }
            DevOnHubPort[i - 1].DeviceStatus = ROOT_DEV_DISCONNECT; // ���豸����
            if(Com_Buffer[2] & (1 << (HUB_C_PORT_CONNECTION & 0x07)))
            {
                HubClearPortFeature(i, HUB_C_PORT_CONNECTION); // ����Ƴ���?��־
            }
        }
    }
    return (ERR_SUCCESS); // ���ز����ɹ�
}

/*********************************************************************
 * @fn      EnumAllHubPort
 *
 * @brief   ö������ROOT-HUB�˿����ⲿHUB��Ķ���USB�豸
 *
 * @return  ������
 */
uint8_t EnumAllHubPort(void)
{
    uint8_t s;

    if((ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS) && (ThisUsbDev.DeviceType == USB_DEV_CLASS_HUB)) // HUBö�ٳɹ�
    {
        SelectHubPort(0);    // ѡ�����ָ����ROOT-HUB�˿�,���õ�ǰUSB�ٶ��Լ��������豸��USB��ַ
        s = EnumHubPort();   // ö��ָ��ROOT-HUB�˿��ϵ��ⲿHUB�������ĸ����˿�,�����˿��������ӻ��Ƴ��¼�
        if(s != ERR_SUCCESS) // ������HUB�Ͽ���
        {
            PRINT("EnumAllHubPort err = %02X\n", (uint16_t)s);
        }
        SetUsbSpeed(1); // Ĭ��Ϊȫ��
    }
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      SearchTypeDevice
 *
 * @brief   ��ROOT-HUB�Լ��ⲿHUB���˿�������ָ�����͵��豸���ڵĶ˿ں�,����˿ں��?0xFFFF��δ������.
 *          ��ȻҲ���Ը���USB�ĳ���VID��ƷPID��������(����Ҫ��¼���豸��VID��PID),�Լ�ָ���������?
 *
 * @param   type    - �������豸����
 *
 * @return  �����?8λΪROOT-HUB�˿ں�,��8λΪ�ⲿHUB�Ķ˿ں�,��8λΪ0���豸ֱ����ROOT-HUB�˿���
 */
uint16_t SearchTypeDevice(uint8_t type)
{
    uint8_t RootHubIndex; //CH554ֻ��һ��USB��,RootHubIndex = 0,ֻ�迴����ֵ�ĵͰ�λ����
    uint8_t HubPortIndex;

    RootHubIndex = 0;
    if((ThisUsbDev.DeviceType == USB_DEV_CLASS_HUB) && (ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS)) // �ⲿ������HUB��ö�ٳɹ�
    {
        for(HubPortIndex = 1; HubPortIndex <= ThisUsbDev.GpHUBPortNum; HubPortIndex++) // �����ⲿHUB�ĸ����˿�
        {
            if(DevOnHubPort[HubPortIndex - 1].DeviceType == type && DevOnHubPort[HubPortIndex - 1].DeviceStatus >= ROOT_DEV_SUCCESS)
            {
                return (((uint16_t)RootHubIndex << 8) | HubPortIndex); // ����ƥ����ö�ٳɹ�
            }
        }
    }
    if((ThisUsbDev.DeviceType == type) && (ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS))
    {
        return ((uint16_t)RootHubIndex << 8); // ����ƥ����ö�ٳɹ�,��ROOT-HUB�˿���
    }

    return (0xFFFF);
}

/*********************************************************************
 * @fn      SETorOFFNumLock
 *
 * @brief   NumLock�ĵ���ж�?
 *
 * @param   buf     - ��Ƽ��?
 *
 * @return  ������
 */
uint8_t SETorOFFNumLock(uint8_t *buf)
{
    uint8_t tmp[] = {0x21, 0x09, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00};
    uint8_t len, s;
    if((buf[2] == 0x53) & ((buf[0] | buf[1] | buf[3] | buf[4] | buf[5] | buf[6] | buf[7]) == 0))
    {
        for(s = 0; s != sizeof(tmp); s++)
        {
            ((uint8_t *)pSetupReq)[s] = tmp[s];
        }
        s = HostCtrlTransfer(Com_Buffer, &len); // ִ�п��ƴ���
        if(s != ERR_SUCCESS)
        {
            return (s);
        }
    }
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      CtrlGetHIDDeviceReport
 *
 * @brief   ��ȡHID�豸����������,������TxBuffer��
 *
 * @param   none
 *
 * @return  ������
 */
uint8_t CtrlGetHIDDeviceReport(uint8_t infc)
{
    uint8_t s;
    uint8_t len;

    CopySetupReqPkg(SetupSetHIDIdle);
    pSetupReq->wIndex = infc;
    s = HostCtrlTransfer(Com_Buffer, &len); // ִ�п��ƴ���
    if(s != ERR_SUCCESS)
    {
        return (s);
    }

    CopySetupReqPkg(SetupGetHIDDevReport);
    pSetupReq->wIndex = infc;
    s = HostCtrlTransfer(Com_Buffer, &len); // ִ�п��ƴ���
    if(s != ERR_SUCCESS)
    {
        return (s);
    }

    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      CtrlGetHubDescr
 *
 * @brief   ��ȡHUB������,������Com_Buffer��
 *
 * @param   none
 *
 * @return  ������
 */
uint8_t CtrlGetHubDescr(void)
{
    uint8_t s;
    uint8_t len;

    CopySetupReqPkg(SetupGetHubDescr);
    s = HostCtrlTransfer(Com_Buffer, &len); // ִ�п��ƴ���
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    if(len < ((PUSB_SETUP_REQ)SetupGetHubDescr)->wLength)
    {
        return (ERR_USB_BUF_OVER); // ���������ȴ���
    }
    //  if ( len < 4 ) return( ERR_USB_BUF_OVER );                                 // ���������ȴ���
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      HubGetPortStatus
 *
 * @brief   ��ѯHUB�˿�״̬,������Com_Buffer��
 *
 * @param   HubPortIndex    - �˿ں�
 *
 * @return  ������
 */
uint8_t HubGetPortStatus(uint8_t HubPortIndex)
{
    uint8_t s;
    uint8_t len;

    pSetupReq->bRequestType = HUB_GET_PORT_STATUS;
    pSetupReq->bRequest = HUB_GET_STATUS;
    pSetupReq->wValue = 0x0000;
    pSetupReq->wIndex = 0x0000 | HubPortIndex;
    pSetupReq->wLength = 0x0004;
    s = HostCtrlTransfer(Com_Buffer, &len); // ִ�п��ƴ���
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    if(len < 4)
    {
        return (ERR_USB_BUF_OVER); // ���������ȴ���
    }
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      HubSetPortFeature
 *
 * @brief   ����HUB�˿�����
 *
 * @param   HubPortIndex    - �˿ں�
 * @param   FeatureSelt     - �˿�����
 *
 * @return  ������
 */
uint8_t HubSetPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt)
{
    pSetupReq->bRequestType = HUB_SET_PORT_FEATURE;
    pSetupReq->bRequest = HUB_SET_FEATURE;
    pSetupReq->wValue = 0x0000 | FeatureSelt;
    pSetupReq->wIndex = 0x0000 | HubPortIndex;
    pSetupReq->wLength = 0x0000;
    return (HostCtrlTransfer(NULL, NULL)); // ִ�п��ƴ���
}

/*********************************************************************
 * @fn      HubClearPortFeature
 *
 * @brief   ���HUB�˿�����
 *
 * @param   HubPortIndex    - �˿ں�
 * @param   FeatureSelt     - �˿�����
 *
 * @return  ������
 */
uint8_t HubClearPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt)
{
    pSetupReq->bRequestType = HUB_CLEAR_PORT_FEATURE;
    pSetupReq->bRequest = HUB_CLEAR_FEATURE;
    pSetupReq->wValue = 0x0000 | FeatureSelt;
    pSetupReq->wIndex = 0x0000 | HubPortIndex;
    pSetupReq->wLength = 0x0000;
    return (HostCtrlTransfer(NULL, NULL)); // ִ�п��ƴ���
}
