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
__attribute__((aligned(4))) const uint8_t SetupSetU2HIDIdle[] = {0x21, HID_SET_IDLE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
/* ��ȡHID�豸���������� */
__attribute__((aligned(4))) const uint8_t SetupGetU2HIDDevReport[] = {0x81, USB_GET_DESCRIPTOR, 0x00,
                                                                      USB_DESCR_TYP_REPORT, 0x00, 0x00, 0x41, 0x00};
/* ��ȡHUB������ */
__attribute__((aligned(4))) const uint8_t SetupGetU2HubDescr[] = {HUB_GET_HUB_DESCRIPTOR, HUB_GET_DESCRIPTOR,
                                                                  0x00, USB_DESCR_TYP_HUB, 0x00, 0x00, sizeof(USB_HUB_DESCR), 0x00};

__attribute__((aligned(4))) uint8_t U2Com_Buffer[128]; // �����û���ʱ������,ö��ʱ���ڴ���������,ö�ٽ���Ҳ����������ͨ��ʱ������

/*********************************************************************
 * @fn      AnalyzeU2HidIntEndp
 *
 * @brief   ���������з�����HID�ж϶˵�ĵ�ַ,���HubPortIndex��0���浽ROOTHUB������Ƿ���ֵ�򱣴浽HUB�½ṹ��
 *
 * @param   buf     - ���������ݻ�������ַ HubPortIndex��0��ʾ��HUB����0��ʾ�ⲿHUB�µĶ˿ں�
 *
 * @return  �˵���
 */
uint8_t AnalyzeU2HidIntEndp(uint8_t *buf, uint8_t HubPortIndex)
{
    uint8_t i, s, l;
    s = 0;

    if(HubPortIndex)
    {
        memset(DevOnU2HubPort[HubPortIndex - 1].GpVar, 0, sizeof(DevOnU2HubPort[HubPortIndex - 1].GpVar)); //�������
    }
    else
    {
        memset(ThisUsb2Dev.GpVar, 0, sizeof(ThisUsb2Dev.GpVar)); //�������
    }

    for(i = 0; i < ((PUSB_CFG_DESCR)buf)->wTotalLength; i += l) // �����ж϶˵�������,���������������ͽӿ�������
    {
        if(((PUSB_ENDP_DESCR)(buf + i))->bDescriptorType == USB_DESCR_TYP_ENDP                         // �Ƕ˵�������
           && (((PUSB_ENDP_DESCR)(buf + i))->bmAttributes & USB_ENDP_TYPE_MASK) == USB_ENDP_TYPE_INTER // ���ж϶˵�
           && (((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_DIR_MASK))                    // ��IN�˵�
        {                                                                                              // �����ж϶˵�ĵ�ַ,λ7����ͬ����־λ,��0
            if(HubPortIndex)
            {
                DevOnU2HubPort[HubPortIndex - 1].GpVar[s] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
            }
            else
            {
                ThisUsb2Dev.GpVar[s] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK; // �ж϶˵�ĵ�ַ�����Ը�����Ҫ����wMaxPacketSize��bInterval
            }
            PRINT("%02x ", (uint16_t)ThisUsb2Dev.GpVar[s]);
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
 * @fn      AnalyzeU2BulkEndp
 *
 * @brief   �����������˵�,GpVar[0]��GpVar[1]����ϴ��˵㡣GpVar[2]��GpVar[3]����´��˵�
 *
 * @param   buf     - ���������ݻ�������ַ HubPortIndex��0��ʾ��HUB����0��ʾ�ⲿHUB�µĶ˿ں�
 *
 * @return  0
 */
uint8_t AnalyzeU2BulkEndp(uint8_t *buf, uint8_t HubPortIndex)
{
    uint8_t i, s1, s2, l;
    s1 = 0;
    s2 = 2;

    if(HubPortIndex)
    {
        memset(DevOnU2HubPort[HubPortIndex - 1].GpVar, 0, sizeof(DevOnU2HubPort[HubPortIndex - 1].GpVar)); //�������
    }
    else
    {
        memset(ThisUsb2Dev.GpVar, 0, sizeof(ThisUsb2Dev.GpVar)); //�������
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
                    DevOnU2HubPort[HubPortIndex - 1].GpVar[s1++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
                }
                else
                {
                    DevOnU2HubPort[HubPortIndex - 1].GpVar[s2++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
                }
            }
            else
            {
                if(((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_DIR_MASK)
                {
                    ThisUsb2Dev.GpVar[s1++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
                }
                else
                {
                    ThisUsb2Dev.GpVar[s2++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
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
 * @fn      InitRootU2Device
 *
 * @brief   ��ʼ��ָ��ROOT-HUB�˿ڵ�USB�豸
 *
 * @param   none
 *
 * @return  ������
 */
uint8_t InitRootU2Device(void)
{
    uint8_t i, s;
    uint8_t cfg, dv_cls, if_cls;

    PRINT("Reset U2 host port\n");
    ResetRootU2HubPort(); // ��⵽�豸��,��λ��Ӧ�˿ڵ�USB����
    for(i = 0, s = 0; i < 100; i++)
    { // �ȴ�USB�豸��λ����������,100mS��ʱ
        mDelaymS(1);
        if(EnableRootU2HubPort() == ERR_SUCCESS)
        { // ʹ�ܶ˿�
            i = 0;
            s++;
            if(s > 100)
                break; // �Ѿ��ȶ�����100mS
        }
    }
    if(i)
    { // ��λ���豸û������
        DisableRootU2HubPort();
        PRINT("Disable U2 host port because of disconnect\n");
        return (ERR_USB_DISCON);
    }
    SetUsb2Speed(ThisUsb2Dev.DeviceSpeed); // ���õ�ǰUSB�ٶ�

    PRINT("GetU2DevDescr: ");
    s = CtrlGetU2DeviceDescr(); // ��ȡ�豸������
    if(s == ERR_SUCCESS)
    {
        for(i = 0; i < ((PUSB_SETUP_REQ)SetupGetU2DevDescr)->wLength; i++)
            PRINT("x%02X ", (uint16_t)(U2Com_Buffer[i]));
        PRINT("\n");

        ThisUsb2Dev.DeviceVID = ((PUSB_DEV_DESCR)U2Com_Buffer)->idVendor; //����VID PID��Ϣ
        ThisUsb2Dev.DevicePID = ((PUSB_DEV_DESCR)U2Com_Buffer)->idProduct;
        dv_cls = ((PUSB_DEV_DESCR)U2Com_Buffer)->bDeviceClass;

        s = CtrlSetUsb2Address(((PUSB_SETUP_REQ)SetupSetUsb2Addr)->wValue);
        if(s == ERR_SUCCESS)
        {
            ThisUsb2Dev.DeviceAddress = ((PUSB_SETUP_REQ)SetupSetUsb2Addr)->wValue; // ����USB��ַ

            PRINT("GetU2CfgDescr: ");
            s = CtrlGetU2ConfigDescr();
            if(s == ERR_SUCCESS)
            {
                for(i = 0; i < ((PUSB_CFG_DESCR)U2Com_Buffer)->wTotalLength; i++)
                {
                    PRINT("x%02X ", (uint16_t)(U2Com_Buffer[i]));
                }
                PRINT("\n");
                /* ��������������,��ȡ�˵�����/���˵��ַ/���˵��С��,���±���endp_addr��endp_size�� */
                cfg = ((PUSB_CFG_DESCR)U2Com_Buffer)->bConfigurationValue;
                if_cls = ((PUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceClass; // �ӿ������

                if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_STORAGE))
                { // ��USB�洢���豸,������ȷ����U��
#ifdef FOR_ROOT_UDISK_ONLY
                    CHRV3DiskStatus = DISK_USB_ADDR;
                    return (ERR_SUCCESS);
                }
                else
                    return (ERR_USB_UNSUPPORT);
#else
                    s = CtrlSetUsb2Config(cfg); // ����USB�豸����
                    if(s == ERR_SUCCESS)
                    {
                        ThisUsb2Dev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsb2Dev.DeviceType = USB_DEV_CLASS_STORAGE;
                        PRINT("U2 USB-Disk Ready\n");
                        SetUsb2Speed(1); // Ĭ��Ϊȫ��
                        return (ERR_SUCCESS);
                    }
                }
                else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_PRINTER) && ((PUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceSubClass == 0x01)
                {                               // �Ǵ�ӡ�����豸
                    s = CtrlSetUsb2Config(cfg); // ����USB�豸����
                    if(s == ERR_SUCCESS)
                    {
                        //	�豣��˵���Ϣ�Ա����������USB����
                        ThisUsb2Dev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsb2Dev.DeviceType = USB_DEV_CLASS_PRINTER;
                        PRINT("U2 USB-Print Ready\n");
                        SetUsb2Speed(1); // Ĭ��Ϊȫ��
                        return (ERR_SUCCESS);
                    }
                }
                else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) && ((PUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceSubClass <= 0x01)
                { // ��HID���豸,����/����
                    //  ���������з�����HID�ж϶˵�ĵ�ַ
                    s = AnalyzeU2HidIntEndp(U2Com_Buffer, 0); // ���������з�����HID�ж϶˵�ĵ�ַ
                    PRINT("AnalyzeU2HidIntEndp %02x\n", (uint16_t)s);
                    //  �����ж϶˵�ĵ�ַ,λ7����ͬ����־λ,��0
                    if_cls = ((PUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceProtocol;
                    s = CtrlSetUsb2Config(cfg); // ����USB�豸����
                    if(s == ERR_SUCCESS)
                    {
                        //	    					Set_Idle( );
                        //	�豣��˵���Ϣ�Ա����������USB����
                        ThisUsb2Dev.DeviceStatus = ROOT_DEV_SUCCESS;
                        if(if_cls == 1)
                        {
                            ThisUsb2Dev.DeviceType = DEV_TYPE_KEYBOARD;
                            //	��һ����ʼ��,�����豸����ָʾ��LED��
                            PRINT("U2 USB-Keyboard Ready\n");
                            SetUsb2Speed(1); // Ĭ��Ϊȫ��
                            return (ERR_SUCCESS);
                        }
                        else if(if_cls == 2)
                        {
                            ThisUsb2Dev.DeviceType = DEV_TYPE_MOUSE;
                            //	Ϊ���Ժ��ѯ���״̬,Ӧ�÷���������,ȡ���ж϶˿ڵĵ�ַ,���ȵ���Ϣ
                            PRINT("U2 USB-Mouse Ready\n");
                            SetUsb2Speed(1); // Ĭ��Ϊȫ��
                            return (ERR_SUCCESS);
                        }
                        s = ERR_USB_UNSUPPORT;
                    }
                }
                else if(dv_cls == USB_DEV_CLASS_HUB)
                { // ��HUB���豸,��������
                    s = CtrlGetU2HubDescr();
                    if(s == ERR_SUCCESS)
                    {
                        PRINT("Max Port:%02X ", (((PXUSB_HUB_DESCR)U2Com_Buffer)->bNbrPorts));
                        ThisUsb2Dev.GpHUBPortNum = ((PXUSB_HUB_DESCR)U2Com_Buffer)->bNbrPorts; // ����HUB�Ķ˿�����
                        if(ThisUsb2Dev.GpHUBPortNum > HUB_MAX_PORTS)
                        {
                            ThisUsb2Dev.GpHUBPortNum = HUB_MAX_PORTS; // ��Ϊ����ṹDevOnHubPortʱ��Ϊ�ٶ�ÿ��HUB������HUB_MAX_PORTS���˿�
                        }
                        s = CtrlSetUsb2Config(cfg); // ����USB�豸����
                        if(s == ERR_SUCCESS)
                        {
                            ThisUsb2Dev.DeviceStatus = ROOT_DEV_SUCCESS;
                            ThisUsb2Dev.DeviceType = USB_DEV_CLASS_HUB;
                            //�豣��˵���Ϣ�Ա����������USB����,�����ж϶˵������HUB�¼�֪ͨ,��������ʹ�ò�ѯ״̬���ƴ������
                            //��HUB���˿��ϵ�,��ѯ���˿�״̬,��ʼ�����豸���ӵ�HUB�˿�,��ʼ���豸
                            for(i = 1; i <= ThisUsb2Dev.GpHUBPortNum; i++) // ��HUB���˿ڶ��ϵ�
                            {
                                DevOnU2HubPort[i - 1].DeviceStatus = ROOT_DEV_DISCONNECT; // ���ⲿHUB�˿����豸��״̬
                                s = U2HubSetPortFeature(i, HUB_PORT_POWER);
                                if(s != ERR_SUCCESS)
                                {
                                    PRINT("Ext-HUB Port_%1d# power on error\n", (uint16_t)i); // �˿��ϵ�ʧ��
                                }
                            }
                            PRINT("U2 USB-HUB Ready\n");
                            SetUsb2Speed(1); // Ĭ��Ϊȫ��
                            return (ERR_SUCCESS);
                        }
                    }
                }
                else
                {                               // ���Խ�һ������
                    s = CtrlSetUsb2Config(cfg); // ����USB�豸����
                    if(s == ERR_SUCCESS)
                    {
                        //	�豣��˵���Ϣ�Ա����������USB����
                        ThisUsb2Dev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsb2Dev.DeviceType = DEV_TYPE_UNKNOW;
                        SetUsb2Speed(1);      // Ĭ��Ϊȫ��
                        return (ERR_SUCCESS); /* δ֪�豸��ʼ���ɹ� */
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
    SetUsb2Speed(1); // Ĭ��Ϊȫ��
    return (s);
}

/*********************************************************************
 * @fn      InitU2DevOnHub
 *
 * @brief   ��ʼ��ö���ⲿHUB��Ķ���USB�豸
 *
 * @param   HubPortIndex    - ָ���ⲿHUB
 *
 * @return  ������
 */
uint8_t InitU2DevOnHub(uint8_t HubPortIndex)
{
    uint8_t i, s, cfg, dv_cls, if_cls;
    uint8_t ifc;
    PRINT("Init dev @ExtHub-port_%1d ", (uint16_t)HubPortIndex);
    if(HubPortIndex == 0)
    {
        return (ERR_USB_UNKNOWN);
    }
    SelectU2HubPort(HubPortIndex); // ѡ�����ָ����ROOT-HUB�˿ڵ��ⲿHUB��ָ���˿�,ѡ���ٶ�
    PRINT("GetDevDescr: ");
    s = CtrlGetU2DeviceDescr(); // ��ȡ�豸������
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    DevOnU2HubPort[HubPortIndex - 1].DeviceVID = ((uint16_t)((PUSB_DEV_DESCR)U2Com_Buffer)->idVendor); //����VID PID��Ϣ
    DevOnU2HubPort[HubPortIndex - 1].DevicePID = ((uint16_t)((PUSB_DEV_DESCR)U2Com_Buffer)->idProduct);

    dv_cls = ((PUSB_DEV_DESCR)U2Com_Buffer)->bDeviceClass; // �豸�����
    cfg = (1 << 4) + HubPortIndex;                         // �����һ��USB��ַ,�����ַ�ص�
    s = CtrlSetUsb2Address(cfg);                           // ����USB�豸��ַ
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    DevOnU2HubPort[HubPortIndex - 1].DeviceAddress = cfg; // ��������USB��ַ
    PRINT("GetCfgDescr: ");
    s = CtrlGetU2ConfigDescr(); // ��ȡ����������
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    cfg = ((PUSB_CFG_DESCR)U2Com_Buffer)->bConfigurationValue;
    for(i = 0; i < ((PUSB_CFG_DESCR)U2Com_Buffer)->wTotalLength; i++)
    {
        PRINT("x%02X ", (uint16_t)(U2Com_Buffer[i]));
    }
    PRINT("\n");
    /* ��������������,��ȡ�˵�����/���˵��ַ/���˵��С��,���±���endp_addr��endp_size�� */
    if_cls = ((PXUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceClass; // �ӿ������
    if(dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE)                     // ��USB�洢���豸,������ȷ����U��
    {
        AnalyzeU2BulkEndp(U2Com_Buffer, HubPortIndex);
        for(i = 0; i != 4; i++)
        {
            PRINT("%02x ", (uint16_t)DevOnU2HubPort[HubPortIndex - 1].GpVar[i]);
        }
        PRINT("\n");
        s = CtrlSetUsb2Config(cfg); // ����USB�豸����
        if(s == ERR_SUCCESS)
        {
            DevOnU2HubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_SUCCESS;
            DevOnU2HubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_STORAGE;
            PRINT("USB-Disk Ready\n");
            SetUsb2Speed(1); // Ĭ��Ϊȫ��
            return (ERR_SUCCESS);
        }
    }
    else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) && (((PXUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceSubClass <= 0x01)) // ��HID���豸,����/����
    {
        ifc = ((PXUSB_CFG_DESCR_LONG)U2Com_Buffer)->cfg_descr.bNumInterfaces;
        s = AnalyzeU2HidIntEndp(U2Com_Buffer, HubPortIndex); // ���������з�����HID�ж϶˵�ĵ�ַ
        PRINT("AnalyzeU2HidIntEndp %02x\n", (uint16_t)s);
        if_cls = ((PXUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceProtocol;
        s = CtrlSetUsb2Config(cfg); // ����USB�豸����
        if(s == ERR_SUCCESS)
        {
            for(dv_cls = 0; dv_cls < ifc; dv_cls++)
            {
                s = CtrlGetU2HIDDeviceReport(dv_cls); //��ȡ����������
                if(s == ERR_SUCCESS)
                {
                    for(i = 0; i < 64; i++)
                    {
                        PRINT("x%02X ", (uint16_t)(U2Com_Buffer[i]));
                    }
                    PRINT("\n");
                }
            }
            //�豣��˵���Ϣ�Ա����������USB����
            DevOnU2HubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_SUCCESS;
            if(if_cls == 1)
            {
                DevOnU2HubPort[HubPortIndex - 1].DeviceType = DEV_TYPE_KEYBOARD;
                //��һ����ʼ��,�����豸����ָʾ��LED��
                if(ifc > 1)
                {
                    PRINT("USB_DEV_CLASS_HID Ready\n");
                    //                    DevOnU2HubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_HID; //����HID�豸
                }
                PRINT("USB-Keyboard Ready\n");
                SetUsb2Speed(1); // Ĭ��Ϊȫ��

                return (ERR_SUCCESS);
            }
            else if(if_cls == 2)
            {
                DevOnU2HubPort[HubPortIndex - 1].DeviceType = DEV_TYPE_MOUSE;
                //Ϊ���Ժ��ѯ���״̬,Ӧ�÷���������,ȡ���ж϶˿ڵĵ�ַ,���ȵ���Ϣ
                if(ifc > 1)
                {
                    PRINT("USB_DEV_CLASS_HID Ready\n");
                    //                    DevOnU2HubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_HID; //����HID�豸
                }
                PRINT("USB-Mouse Ready\n");
                SetUsb2Speed(1); // Ĭ��Ϊȫ��

                return (ERR_SUCCESS);
            }
            s = ERR_USB_UNSUPPORT;
        }
    }
    else if(dv_cls == USB_DEV_CLASS_HUB) // ��HUB���豸,��������
    {
        DevOnU2HubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_HUB;
        PRINT("This program don't support Level 2 HUB\n"); // ��Ҫ֧�ֶ༶HUB������ο������������չ
        s = U2HubClearPortFeature(i, HUB_PORT_ENABLE);     // ��ֹHUB�˿�
        if(s != ERR_SUCCESS)
        {
            return (s);
        }
        s = ERR_USB_UNSUPPORT;
    }
    else //�����豸
    {
        AnalyzeU2BulkEndp(U2Com_Buffer, HubPortIndex); //�����������˵�
        for(i = 0; i != 4; i++)
        {
            PRINT("%02x ", (uint16_t)DevOnU2HubPort[HubPortIndex - 1].GpVar[i]);
        }
        PRINT("\n");
        s = CtrlSetUsb2Config(cfg); // ����USB�豸����
        if(s == ERR_SUCCESS)
        {
            //�豣��˵���Ϣ�Ա����������USB����
            DevOnU2HubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_SUCCESS;
            DevOnU2HubPort[HubPortIndex - 1].DeviceType = dv_cls ? dv_cls : if_cls;
            SetUsb2Speed(1);      // Ĭ��Ϊȫ��
            return (ERR_SUCCESS); //δ֪�豸��ʼ���ɹ�
        }
    }
    PRINT("InitDevOnHub Err = %02X\n", (uint16_t)s);
    DevOnU2HubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_FAILED;
    SetUsb2Speed(1); // Ĭ��Ϊȫ��
    return (s);
}

/*********************************************************************
 * @fn      EnumU2HubPort
 *
 * @brief   ö��ָ��ROOT-HUB�˿��ϵ��ⲿHUB�������ĸ����˿�,�����˿��������ӻ��Ƴ��¼�����ʼ������USB�豸
 *
 * @param   RootHubIndex    - ROOT_HUB0��ROOT_HUB1
 *
 * @return  ������
 */
uint8_t EnumU2HubPort()
{
    uint8_t i, s;

    for(i = 1; i <= ThisUsb2Dev.GpHUBPortNum; i++) // ��ѯ�������Ķ˿��Ƿ��б仯
    {
        SelectU2HubPort(0);        // ѡ�����ָ����ROOT-HUB�˿�,���õ�ǰUSB�ٶ��Լ��������豸��USB��ַ
        s = U2HubGetPortStatus(i); // ��ȡ�˿�״̬
        if(s != ERR_SUCCESS)
        {
            return (s); // �����Ǹ�HUB�Ͽ���
        }
        if(((U2Com_Buffer[0] & (1 << (HUB_PORT_CONNECTION & 0x07))) && (U2Com_Buffer[2] & (1 << (HUB_C_PORT_CONNECTION & 0x07)))) || (U2Com_Buffer[2] == 0x10))
        {                                                            // �������豸����
            DevOnU2HubPort[i - 1].DeviceStatus = ROOT_DEV_CONNECTED; // ���豸����
            DevOnU2HubPort[i - 1].DeviceAddress = 0x00;
            s = U2HubGetPortStatus(i); // ��ȡ�˿�״̬
            if(s != ERR_SUCCESS)
            {
                return (s); // �����Ǹ�HUB�Ͽ���
            }
            DevOnU2HubPort[i - 1].DeviceSpeed = U2Com_Buffer[1] & (1 << (HUB_PORT_LOW_SPEED & 0x07)) ? 0 : 1; // ���ٻ���ȫ��
            if(DevOnU2HubPort[i - 1].DeviceSpeed)
            {
                PRINT("Found full speed device on port %1d\n", (uint16_t)i);
            }
            else
            {
                PRINT("Found low speed device on port %1d\n", (uint16_t)i);
            }
            mDelaymS(200);                              // �ȴ��豸�ϵ��ȶ�
            s = U2HubSetPortFeature(i, HUB_PORT_RESET); // �����豸���ӵĶ˿ڸ�λ
            if(s != ERR_SUCCESS)
            {
                return (s); // �����Ǹ�HUB�Ͽ���
            }
            PRINT("Reset port and then wait in\n");
            do // ��ѯ��λ�˿�,ֱ����λ���,����ɺ��״̬��ʾ����
            {
                mDelaymS(1);
                s = U2HubGetPortStatus(i);
                if(s != ERR_SUCCESS)
                {
                    return (s); // �����Ǹ�HUB�Ͽ���
                }
            } while(U2Com_Buffer[0] & (1 << (HUB_PORT_RESET & 0x07))); // �˿����ڸ�λ��ȴ�
            mDelaymS(100);
            s = U2HubClearPortFeature(i, HUB_C_PORT_RESET);      // �����λ��ɱ�־
                                                                 //             s = U2HubSetPortFeature( i, HUB_PORT_ENABLE );                              // ����HUB�˿�
            s = U2HubClearPortFeature(i, HUB_C_PORT_CONNECTION); // ������ӻ��Ƴ��仯��־
            if(s != ERR_SUCCESS)
            {
                return (s);
            }
            s = U2HubGetPortStatus(i); // �ٶ�ȡ״̬,�����豸�Ƿ���
            if(s != ERR_SUCCESS)
            {
                return (s);
            }
            if((U2Com_Buffer[0] & (1 << (HUB_PORT_CONNECTION & 0x07))) == 0)
            {
                DevOnU2HubPort[i - 1].DeviceStatus = ROOT_DEV_DISCONNECT; // �豸������
            }
            s = InitU2DevOnHub(i); // ��ʼ������USB�豸
            if(s != ERR_SUCCESS)
            {
                return (s);
            }
            SetUsb2Speed(1); // Ĭ��Ϊȫ��
        }
        else if(U2Com_Buffer[2] & (1 << (HUB_C_PORT_ENABLE & 0x07))) // �豸���ӳ���
        {
            U2HubClearPortFeature(i, HUB_C_PORT_ENABLE); // ������Ӵ����־
            PRINT("Device on port error\n");
            s = U2HubSetPortFeature(i, HUB_PORT_RESET); // �����豸���ӵĶ˿ڸ�λ
            if(s != ERR_SUCCESS)
                return (s); // �����Ǹ�HUB�Ͽ���
            do              // ��ѯ��λ�˿�,ֱ����λ���,����ɺ��״̬��ʾ����
            {
                mDelaymS(1);
                s = U2HubGetPortStatus(i);
                if(s != ERR_SUCCESS)
                    return (s);                                        // �����Ǹ�HUB�Ͽ���
            } while(U2Com_Buffer[0] & (1 << (HUB_PORT_RESET & 0x07))); // �˿����ڸ�λ��ȴ�
        }
        else if((U2Com_Buffer[0] & (1 << (HUB_PORT_CONNECTION & 0x07))) == 0) // �豸�Ѿ��Ͽ�
        {
            if(DevOnU2HubPort[i - 1].DeviceStatus >= ROOT_DEV_CONNECTED)
            {
                PRINT("Device on port %1d removed\n", (uint16_t)i);
            }
            DevOnU2HubPort[i - 1].DeviceStatus = ROOT_DEV_DISCONNECT; // ���豸����
            if(U2Com_Buffer[2] & (1 << (HUB_C_PORT_CONNECTION & 0x07)))
            {
                U2HubClearPortFeature(i, HUB_C_PORT_CONNECTION); // ����Ƴ��仯��־
            }
        }
    }
    return (ERR_SUCCESS); // ���ز����ɹ�
}

/*********************************************************************
 * @fn      EnumAllU2HubPort
 *
 * @brief   ö������ROOT-HUB�˿����ⲿHUB��Ķ���USB�豸
 *
 * @return  ������
 */
uint8_t EnumAllU2HubPort(void)
{
    uint8_t s;

    if((ThisUsb2Dev.DeviceStatus >= ROOT_DEV_SUCCESS) && (ThisUsb2Dev.DeviceType == USB_DEV_CLASS_HUB)) // HUBö�ٳɹ�
    {
        SelectU2HubPort(0);  // ѡ�����ָ����ROOT-HUB�˿�,���õ�ǰUSB�ٶ��Լ��������豸��USB��ַ
        s = EnumU2HubPort(); // ö��ָ��ROOT-HUB�˿��ϵ��ⲿHUB�������ĸ����˿�,�����˿��������ӻ��Ƴ��¼�
        if(s != ERR_SUCCESS) // ������HUB�Ͽ���
        {
            PRINT("EnumAllHubPort err = %02X\n", (uint16_t)s);
        }
        SetUsb2Speed(1); // Ĭ��Ϊȫ��
    }
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      U2SearchTypeDevice
 *
 * @brief   ��ROOT-HUB�Լ��ⲿHUB���˿�������ָ�����͵��豸���ڵĶ˿ں�,����˿ں�Ϊ0xFFFF��δ������.
 *          ��ȻҲ���Ը���USB�ĳ���VID��ƷPID��������(����Ҫ��¼���豸��VID��PID),�Լ�ָ���������
 *
 * @param   type    - �������豸����
 *
 * @return  �����8λΪROOT-HUB�˿ں�,��8λΪ�ⲿHUB�Ķ˿ں�,��8λΪ0���豸ֱ����ROOT-HUB�˿���
 */
uint16_t U2SearchTypeDevice(uint8_t type)
{
    uint8_t RootHubIndex; //CH554ֻ��һ��USB��,RootHubIndex = 0,ֻ�迴����ֵ�ĵͰ�λ����
    uint8_t HubPortIndex;

    RootHubIndex = 0;
    if((ThisUsb2Dev.DeviceType == USB_DEV_CLASS_HUB) && (ThisUsb2Dev.DeviceStatus >= ROOT_DEV_SUCCESS)) // �ⲿ������HUB��ö�ٳɹ�
    {
        for(HubPortIndex = 1; HubPortIndex <= ThisUsb2Dev.GpHUBPortNum; HubPortIndex++) // �����ⲿHUB�ĸ����˿�
        {
            if(DevOnU2HubPort[HubPortIndex - 1].DeviceType == type && DevOnU2HubPort[HubPortIndex - 1].DeviceStatus >= ROOT_DEV_SUCCESS)
            {
                return (((uint16_t)RootHubIndex << 8) | HubPortIndex); // ����ƥ����ö�ٳɹ�
            }
        }
    }
    if((ThisUsb2Dev.DeviceType == type) && (ThisUsb2Dev.DeviceStatus >= ROOT_DEV_SUCCESS))
    {
        return ((uint16_t)RootHubIndex << 8); // ����ƥ����ö�ٳɹ�,��ROOT-HUB�˿���
    }

    return (0xFFFF);
}

/*********************************************************************
 * @fn      U2SETorOFFNumLock
 *
 * @brief   NumLock�ĵ���ж�
 *
 * @param   buf     - ��Ƽ�ֵ
 *
 * @return  ������
 */
uint8_t U2SETorOFFNumLock(uint8_t *buf)
{
    uint8_t tmp[] = {0x21, 0x09, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00};
    uint8_t len, s;
    if((buf[2] == 0x53) & ((buf[0] | buf[1] | buf[3] | buf[4] | buf[5] | buf[6] | buf[7]) == 0))
    {
        for(s = 0; s != sizeof(tmp); s++)
        {
            ((uint8_t *)pU2SetupReq)[s] = tmp[s];
        }
        s = U2HostCtrlTransfer(U2Com_Buffer, &len); // ִ�п��ƴ���
        if(s != ERR_SUCCESS)
        {
            return (s);
        }
    }
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      CtrlGetU2HIDDeviceReport
 *
 * @brief   ��ȡHID�豸����������,������TxBuffer��
 *
 * @param   none
 *
 * @return  ������
 */
uint8_t CtrlGetU2HIDDeviceReport(uint8_t infc)
{
    uint8_t s;
    uint8_t len;

    CopyU2SetupReqPkg((uint8_t *)SetupSetU2HIDIdle);
    pU2SetupReq->wIndex = infc;
    s = U2HostCtrlTransfer(U2Com_Buffer, &len); // ִ�п��ƴ���
    if(s != ERR_SUCCESS)
    {
        return (s);
    }

    CopyU2SetupReqPkg((uint8_t *)SetupGetU2HIDDevReport);
    pU2SetupReq->wIndex = infc;
    s = U2HostCtrlTransfer(U2Com_Buffer, &len); // ִ�п��ƴ���
    if(s != ERR_SUCCESS)
    {
        return (s);
    }

    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      CtrlGetU2HubDescr
 *
 * @brief   ��ȡHUB������,������Com_Buffer��
 *
 * @param   none
 *
 * @return  ������
 */
uint8_t CtrlGetU2HubDescr(void)
{
    uint8_t s;
    uint8_t len;

    CopyU2SetupReqPkg((uint8_t *)SetupGetU2HubDescr);
    s = U2HostCtrlTransfer(U2Com_Buffer, &len); // ִ�п��ƴ���
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    if(len < ((PUSB_SETUP_REQ)SetupGetU2HubDescr)->wLength)
    {
        return (ERR_USB_BUF_OVER); // ���������ȴ���
    }
    //  if ( len < 4 ) return( ERR_USB_BUF_OVER );                                 // ���������ȴ���
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      U2HubGetPortStatus
 *
 * @brief   ��ѯHUB�˿�״̬,������Com_Buffer��
 *
 * @param   HubPortIndex    - �˿ں�
 *
 * @return  ������
 */
uint8_t U2HubGetPortStatus(uint8_t HubPortIndex)
{
    uint8_t s;
    uint8_t len;

    pU2SetupReq->bRequestType = HUB_GET_PORT_STATUS;
    pU2SetupReq->bRequest = HUB_GET_STATUS;
    pU2SetupReq->wValue = 0x0000;
    pU2SetupReq->wIndex = 0x0000 | HubPortIndex;
    pU2SetupReq->wLength = 0x0004;
    s = U2HostCtrlTransfer(U2Com_Buffer, &len); // ִ�п��ƴ���
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
 * @fn      U2HubSetPortFeature
 *
 * @brief   ����HUB�˿�����
 *
 * @param   HubPortIndex    - �˿ں�
 * @param   FeatureSelt     - �˿�����
 *
 * @return  ������
 */
uint8_t U2HubSetPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt)
{
    pU2SetupReq->bRequestType = HUB_SET_PORT_FEATURE;
    pU2SetupReq->bRequest = HUB_SET_FEATURE;
    pU2SetupReq->wValue = 0x0000 | FeatureSelt;
    pU2SetupReq->wIndex = 0x0000 | HubPortIndex;
    pU2SetupReq->wLength = 0x0000;
    return (U2HostCtrlTransfer(NULL, NULL)); // ִ�п��ƴ���
}

/*********************************************************************
 * @fn      U2HubClearPortFeature
 *
 * @brief   ���HUB�˿�����
 *
 * @param   HubPortIndex    - �˿ں�
 * @param   FeatureSelt     - �˿�����
 *
 * @return  ������
 */
uint8_t U2HubClearPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt)
{
    pU2SetupReq->bRequestType = HUB_CLEAR_PORT_FEATURE;
    pU2SetupReq->bRequest = HUB_CLEAR_FEATURE;
    pU2SetupReq->wValue = 0x0000 | FeatureSelt;
    pU2SetupReq->wIndex = 0x0000 | HubPortIndex;
    pU2SetupReq->wLength = 0x0000;
    return (U2HostCtrlTransfer(NULL, NULL)); // ִ�п��ƴ���
}
