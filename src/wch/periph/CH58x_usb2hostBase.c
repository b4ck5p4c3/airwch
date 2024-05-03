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

uint8_t       Usb2DevEndp0Size; // USB2�豸�Ķ˵�0�������ߴ�
uint8_t       FoundNewU2Dev;
_RootHubDev   ThisUsb2Dev;                   //ROOT��
_DevOnHubPort DevOnU2HubPort[HUB_MAX_PORTS]; // �ٶ�:������1���ⲿHUB,ÿ���ⲿHUB������HUB_MAX_PORTS���˿�(���˲���)

uint8_t *pU2HOST_RX_RAM_Addr;
uint8_t *pU2HOST_TX_RAM_Addr;

/*��ȡ�豸������*/
__attribute__((aligned(4))) const uint8_t SetupGetU2DevDescr[] = {USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00,
                                                                  USB_DESCR_TYP_DEVICE, 0x00, 0x00, sizeof(USB_DEV_DESCR), 0x00};
/*��ȡ����������*/
__attribute__((aligned(4))) const uint8_t SetupGetU2CfgDescr[] = {USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00,
                                                                  USB_DESCR_TYP_CONFIG, 0x00, 0x00, 0x04, 0x00};
/*����USB��ַ*/
__attribute__((aligned(4))) const uint8_t SetupSetUsb2Addr[] = {USB_REQ_TYP_OUT, USB_SET_ADDRESS, USB_DEVICE_ADDR,
                                                                0x00, 0x00, 0x00, 0x00, 0x00};
/*����USB����*/
__attribute__((aligned(4))) const uint8_t SetupSetUsb2Config[] = {USB_REQ_TYP_OUT, USB_SET_CONFIGURATION,
                                                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
/*����USB�ӿ�����*/
__attribute__((aligned(4))) const uint8_t SetupSetUsb2Interface[] = {USB_REQ_RECIP_INTERF, USB_SET_INTERFACE,
                                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
/*����˵�STALL*/
__attribute__((aligned(4))) const uint8_t SetupClrU2EndpStall[] = {USB_REQ_TYP_OUT | USB_REQ_RECIP_ENDP, USB_CLEAR_FEATURE,
                                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/*********************************************************************
 * @fn      DisableRootU2HubPort
 *
 * @brief   �ر�ROOT-HUB�˿�,ʵ����Ӳ���Ѿ��Զ��ر�,�˴�ֻ�����һЩ�ṹ״̬
 *
 * @param   none
 *
 * @return  none
 */
void DisableRootU2HubPort(void)
{
#ifdef FOR_ROOT_UDISK_ONLY
    CHRV3DiskStatus = DISK_DISCONNECT;
#endif
#ifndef DISK_BASE_BUF_LEN
    ThisUsb2Dev.DeviceStatus = ROOT_DEV_DISCONNECT;
    ThisUsb2Dev.DeviceAddress = 0x00;
#endif
}

/*********************************************************************
 * @fn      AnalyzeRootU2Hub
 *
 * @brief   ����ROOT-HUB״̬,����ROOT-HUB�˿ڵ��豸����¼�
 *          ����豸�γ�,�����е���DisableRootHubPort()����,���˿ڹر�,�����¼�,����Ӧ�˿ڵ�״̬λ
 *
 * @param   none
 *
 * @return  ����ERR_SUCCESSΪû�����,����ERR_USB_CONNECTΪ��⵽������,����ERR_USB_DISCONΪ��⵽�Ͽ�
 */
uint8_t AnalyzeRootU2Hub(void)
{
    uint8_t s;

    s = ERR_SUCCESS;

    if(R8_USB2_MIS_ST & RB_UMS_DEV_ATTACH)
    { // �豸����
#ifdef DISK_BASE_BUF_LEN
        if(CHRV3DiskStatus == DISK_DISCONNECT
#else
        if(ThisUsb2Dev.DeviceStatus == ROOT_DEV_DISCONNECT // ��⵽���豸����
#endif
           || (R8_U2HOST_CTRL & RB_UH_PORT_EN) == 0x00)
        {                           // ��⵽���豸����,����δ����,˵���Ǹղ���
            DisableRootU2HubPort(); // �رն˿�
#ifdef DISK_BASE_BUF_LEN
            CHRV3DiskStatus = DISK_CONNECT;
#else
            ThisUsb2Dev.DeviceSpeed = R8_USB2_MIS_ST & RB_UMS_DM_LEVEL ? 0 : 1;
            ThisUsb2Dev.DeviceStatus = ROOT_DEV_CONNECTED; //�����ӱ�־
#endif
            PRINT("USB2 dev in\n");
            s = ERR_USB_CONNECT;
        }
    }

#ifdef DISK_BASE_BUF_LEN
    else if(CHRV3DiskStatus >= DISK_CONNECT)
    {
#else
    else if(ThisUsb2Dev.DeviceStatus >= ROOT_DEV_CONNECTED)
    { //��⵽�豸�γ�
#endif
        DisableRootU2HubPort(); // �رն˿�
        PRINT("USB2 dev out\n");
        if(s == ERR_SUCCESS)
            s = ERR_USB_DISCON;
    }
    //  R8_USB_INT_FG = RB_UIF_DETECT;                                                  // ���жϱ�־
    return (s);
}

/*********************************************************************
 * @fn      SetHostUsb2Addr
 *
 * @brief   ����USB������ǰ������USB�豸��ַ
 *
 * @param   addr    - USB�豸��ַ
 *
 * @return  none
 */
void SetHostUsb2Addr(uint8_t addr)
{
    R8_USB2_DEV_AD = (R8_USB2_DEV_AD & RB_UDA_GP_BIT) | (addr & MASK_USB_ADDR);
}

/*********************************************************************
 * @fn      SetUsb2Speed
 *
 * @brief   ���õ�ǰUSB�ٶ�
 *
 * @param   FullSpeed   - USB�ٶ�
 *
 * @return  none
 */
void SetUsb2Speed(uint8_t FullSpeed)
{
#ifndef DISK_BASE_BUF_LEN
    if(FullSpeed) // ȫ��
    {
        R8_USB2_CTRL &= ~RB_UC_LOW_SPEED;  // ȫ��
        R8_U2H_SETUP &= ~RB_UH_PRE_PID_EN; // ��ֹPRE PID
    }
    else
    {
        R8_USB2_CTRL |= RB_UC_LOW_SPEED; // ����
    }
#endif
    (void)FullSpeed;
}

/*********************************************************************
 * @fn      ResetRootU2HubPort
 *
 * @brief   ��⵽�豸��,��λ����,Ϊö���豸׼��,����ΪĬ��Ϊȫ��
 *
 * @param   none
 *
 * @return  none
 */
void ResetRootU2HubPort(void)
{
    Usb2DevEndp0Size = DEFAULT_ENDP0_SIZE; //USB2�豸�Ķ˵�0�������ߴ�
    SetHostUsb2Addr(0x00);
    R8_U2HOST_CTRL &= ~RB_UH_PORT_EN;                                       // �ص��˿�
    SetUsb2Speed(1);                                                        // Ĭ��Ϊȫ��
    R8_U2HOST_CTRL = (R8_U2HOST_CTRL & ~RB_UH_LOW_SPEED) | RB_UH_BUS_RESET; // Ĭ��Ϊȫ��,��ʼ��λ
    mDelaymS(15);                                                           // ��λʱ��10mS��20mS
    R8_U2HOST_CTRL = R8_U2HOST_CTRL & ~RB_UH_BUS_RESET;                     // ������λ
    mDelayuS(250);
    R8_USB2_INT_FG = RB_UIF_DETECT; // ���жϱ�־
}

/*********************************************************************
 * @fn      EnableRootU2HubPort
 *
 * @brief   ʹ��ROOT-HUB�˿�,��Ӧ��bUH_PORT_EN��1�����˿�,�豸�Ͽ����ܵ��·���ʧ��
 *
 * @param   none
 *
 * @return  ����ERR_SUCCESSΪ��⵽������,����ERR_USB_DISCONΪ������
 */
uint8_t EnableRootU2HubPort(void)
{
#ifdef DISK_BASE_BUF_LEN
    if(CHRV3DiskStatus < DISK_CONNECT)
        CHRV3DiskStatus = DISK_CONNECT;
#else
    if(ThisUsb2Dev.DeviceStatus < ROOT_DEV_CONNECTED)
        ThisUsb2Dev.DeviceStatus = ROOT_DEV_CONNECTED;
#endif
    if(R8_USB2_MIS_ST & RB_UMS_DEV_ATTACH)
    { // ���豸
#ifndef DISK_BASE_BUF_LEN
        if((R8_U2HOST_CTRL & RB_UH_PORT_EN) == 0x00)
        { // ��δʹ��
            ThisUsb2Dev.DeviceSpeed = (R8_USB2_MIS_ST & RB_UMS_DM_LEVEL) ? 0 : 1;
            if(ThisUsb2Dev.DeviceSpeed == 0)
                R8_U2HOST_CTRL |= RB_UH_LOW_SPEED; // ����
        }
#endif
        R8_U2HOST_CTRL |= RB_UH_PORT_EN; //ʹ��HUB�˿�
        return (ERR_SUCCESS);
    }
    return (ERR_USB_DISCON);
}

#ifndef DISK_BASE_BUF_LEN
/*********************************************************************
 * @fn      SelectU2HubPort
 *
 * @brief   ѡ����Ҫ������HUB��
 *
 * @param   HubPortIndex    - ѡ�����ָ����ROOT-HUB�˿ڵ��ⲿHUB��ָ���˿�
 *
 * @return  None
 */
void SelectU2HubPort(uint8_t HubPortIndex)
{
    if(HubPortIndex) // ѡ�����ָ����ROOT-HUB�˿ڵ��ⲿHUB��ָ���˿�
    {
        SetHostUsb2Addr(DevOnU2HubPort[HubPortIndex - 1].DeviceAddress); // ����USB������ǰ������USB�豸��ַ
        SetUsb2Speed(DevOnU2HubPort[HubPortIndex - 1].DeviceSpeed);      // ���õ�ǰUSB�ٶ�
        if(DevOnU2HubPort[HubPortIndex - 1].DeviceSpeed == 0)            // ͨ���ⲿHUB�����USB�豸ͨѶ��Ҫǰ��ID
        {
            R8_U2EP1_CTRL |= RB_UH_PRE_PID_EN; // ����PRE PID
            mDelayuS(100);
        }
    }
    else
    {
        SetHostUsb2Addr(ThisUsb2Dev.DeviceAddress); // ����USB������ǰ������USB�豸��ַ
        SetUsb2Speed(ThisUsb2Dev.DeviceSpeed);      // ����USB�豸���ٶ�
    }
}
#endif

/*********************************************************************
 * @fn      WaitUSB2_Interrupt
 *
 * @brief   �ȴ�USB�ж�
 *
 * @param   none
 *
 * @return  ����ERR_SUCCESS ���ݽ��ջ��߷��ͳɹ�,����ERR_USB_UNKNOWN ���ݽ��ջ��߷���ʧ��
 */
uint8_t WaitUSB2_Interrupt(void)
{
    uint16_t i;
    for(i = WAIT_USB_TOUT_200US; i != 0 && (R8_USB2_INT_FG & RB_UIF_TRANSFER) == 0; i--)
    {
        ;
    }
    return ((R8_USB2_INT_FG & RB_UIF_TRANSFER) ? ERR_SUCCESS : ERR_USB_UNKNOWN);
}

/*********************************************************************
 * @fn      USB2HostTransact
 *
 * @brief   ��������,����Ŀ�Ķ˵��ַ/PID����,ͬ����־,��20uSΪ��λ��NAK������ʱ��(0������,0xFFFF��������),����0�ɹ�,��ʱ/��������
 *          ���ӳ���������������,����ʵ��Ӧ����,Ϊ���ṩ�����ٶ�,Ӧ�öԱ��ӳ����������Ż�
 *
 * @param   endp_pid    - ���ƺ͵�ַ, ��4λ��token_pid����, ��4λ�Ƕ˵��ַ
 * @param   tog         - ͬ����־
 * @param   timeout     - ��ʱʱ��
 *
 * @return  ERR_USB_UNKNOWN ��ʱ������Ӳ���쳣
 *          ERR_USB_DISCON  �豸�Ͽ�
 *          ERR_USB_CONNECT �豸����
 *          ERR_SUCCESS     �������
 */
uint8_t USB2HostTransact(uint8_t endp_pid, uint8_t tog, uint32_t timeout)
{
    uint8_t TransRetry;

    uint8_t  s, r;
    uint16_t i;

    R8_U2H_RX_CTRL = R8_U2H_TX_CTRL = tog;
    TransRetry = 0;

    do
    {
        R8_U2H_EP_PID = endp_pid; // ָ������PID��Ŀ�Ķ˵��
        R8_USB2_INT_FG = RB_UIF_TRANSFER;
        for(i = WAIT_USB_TOUT_200US; i != 0 && (R8_USB2_INT_FG & RB_UIF_TRANSFER) == 0; i--)
            ;
        R8_U2H_EP_PID = 0x00; // ֹͣUSB����
        if((R8_USB2_INT_FG & RB_UIF_TRANSFER) == 0)
        {
            return (ERR_USB_UNKNOWN);
        }

        if(R8_USB2_INT_FG & RB_UIF_DETECT)
        { // USB�豸����¼�
            //      mDelayuS( 200 );                                                       // �ȴ��������
            R8_USB2_INT_FG = RB_UIF_DETECT;
            s = AnalyzeRootU2Hub(); // ����ROOT-U2HUB״̬

            if(s == ERR_USB_CONNECT)
                FoundNewU2Dev = 1;
#ifdef DISK_BASE_BUF_LEN
            if(CHRV3DiskStatus == DISK_DISCONNECT)
            {
                return (ERR_USB_DISCON);
            } // USB�豸�Ͽ��¼�
            if(CHRV3DiskStatus == DISK_CONNECT)
            {
                return (ERR_USB_CONNECT);
            } // USB�豸�����¼�
#else
            if(ThisUsb2Dev.DeviceStatus == ROOT_DEV_DISCONNECT)
            {
                return (ERR_USB_DISCON);
            } // USB�豸�Ͽ��¼�
            if(ThisUsb2Dev.DeviceStatus == ROOT_DEV_CONNECTED)
            {
                return (ERR_USB_CONNECT);
            } // USB�豸�����¼�
#endif
            mDelayuS(200); // �ȴ��������
        }

        if(R8_USB2_INT_FG & RB_UIF_TRANSFER) // ��������¼�
        {
            if(R8_USB2_INT_ST & RB_UIS_TOG_OK)
            {
                return (ERR_SUCCESS);
            }
            r = R8_USB2_INT_ST & MASK_UIS_H_RES; // USB�豸Ӧ��״̬
            if(r == USB_PID_STALL)
            {
                return (r | ERR_USB_TRANSFER);
            }
            if(r == USB_PID_NAK)
            {
                if(timeout == 0)
                {
                    return (r | ERR_USB_TRANSFER);
                }
                if(timeout < 0xFFFFFFFF)
                    timeout--;
                --TransRetry;
            }
            else
                switch(endp_pid >> 4)
                {
                    case USB_PID_SETUP:
                    case USB_PID_OUT:
                        if(r)
                        {
                            return (r | ERR_USB_TRANSFER);
                        }      // ���ǳ�ʱ/����,����Ӧ��
                        break; // ��ʱ����
                    case USB_PID_IN:
                        if(r == USB_PID_DATA0 || r == USB_PID_DATA1)
                        { // ��ͬ�����趪��������
                        } // ��ͬ������
                        else if(r)
                        {
                            return (r | ERR_USB_TRANSFER);
                        }      // ���ǳ�ʱ/����,����Ӧ��
                        break; // ��ʱ����
                    default:
                        return (ERR_USB_UNKNOWN); // �����ܵ����
                        break;
                }
        }
        else
        {                          // �����ж�,��Ӧ�÷��������
            R8_USB2_INT_FG = 0xFF; /* ���жϱ�־ */
        }
        mDelayuS(15);
    } while(++TransRetry < 3);
    return (ERR_USB_TRANSFER); // Ӧ��ʱ
}

/*********************************************************************
 * @fn      U2HostCtrlTransfer
 *
 * @brief   ִ�п��ƴ���,8�ֽ���������pSetupReq��,DataBufΪ��ѡ���շ�������
 *
 * @param   DataBuf     - �����Ҫ���պͷ�������,��ôDataBuf��ָ����Ч���������ڴ�ź�������
 * @param   RetLen      - ʵ�ʳɹ��շ����ܳ��ȱ�����RetLenָ����ֽڱ�����
 *
 * @return  ERR_USB_BUF_OVER    IN״̬�׶γ���
 *          ERR_SUCCESS         ���ݽ����ɹ�
 */
uint8_t U2HostCtrlTransfer(uint8_t *DataBuf, uint8_t *RetLen)
{
    uint16_t RemLen = 0;
    uint8_t  s, RxLen, RxCnt, TxCnt;
    uint8_t *pBuf;
    uint8_t *pLen;

    pBuf = DataBuf;
    pLen = RetLen;
    mDelayuS(200);
    if(pLen)
        *pLen = 0; // ʵ�ʳɹ��շ����ܳ���

    R8_U2H_TX_LEN = sizeof(USB_SETUP_REQ);
    s = USB2HostTransact(USB_PID_SETUP << 4 | 0x00, 0x00, 200000 / 20); // SETUP�׶�,200mS��ʱ
    if(s != ERR_SUCCESS)
        return (s);
    R8_U2H_RX_CTRL = R8_U2H_TX_CTRL = RB_UH_R_TOG | RB_UH_R_AUTO_TOG | RB_UH_T_TOG | RB_UH_T_AUTO_TOG; // Ĭ��DATA1
    R8_U2H_TX_LEN = 0x01;                                                                              // Ĭ�������ݹ�״̬�׶�ΪIN
    RemLen = pU2SetupReq->wLength;
    if(RemLen && pBuf) // ��Ҫ�շ�����
    {
        if(pU2SetupReq->bRequestType & USB_REQ_TYP_IN) // ��
        {
            while(RemLen)
            {
                mDelayuS(200);
                s = USB2HostTransact(USB_PID_IN << 4 | 0x00, R8_U2H_RX_CTRL, 200000 / 20); // IN����
                if(s != ERR_SUCCESS)
                    return (s);
                RxLen = R8_USB2_RX_LEN < RemLen ? R8_USB2_RX_LEN : RemLen;
                RemLen -= RxLen;
                if(pLen)
                    *pLen += RxLen; // ʵ�ʳɹ��շ����ܳ���
                for(RxCnt = 0; RxCnt != RxLen; RxCnt++)
                {
                    *pBuf = pU2HOST_RX_RAM_Addr[RxCnt];
                    pBuf++;
                }
                if(R8_USB2_RX_LEN == 0 || (R8_USB2_RX_LEN & (Usb2DevEndp0Size - 1)))
                    break; // �̰�
            }
            R8_U2H_TX_LEN = 0x00; // ״̬�׶�ΪOUT
        }
        else // ��
        {
            while(RemLen)
            {
                mDelayuS(200);
                R8_U2H_TX_LEN = RemLen >= Usb2DevEndp0Size ? Usb2DevEndp0Size : RemLen;
                for(TxCnt = 0; TxCnt != R8_U2H_TX_LEN; TxCnt++)
                {
                    pU2HOST_TX_RAM_Addr[TxCnt] = *pBuf;
                    pBuf++;
                }
                s = USB2HostTransact(USB_PID_OUT << 4 | 0x00, R8_U2H_TX_CTRL, 200000 / 20); // OUT����
                if(s != ERR_SUCCESS)
                    return (s);
                RemLen -= R8_U2H_TX_LEN;
                if(pLen)
                    *pLen += R8_U2H_TX_LEN; // ʵ�ʳɹ��շ����ܳ���
            }
            //          R8_U2H_TX_LEN = 0x01;                                                     // ״̬�׶�ΪIN
        }
    }
    mDelayuS(200);
    s = USB2HostTransact((R8_U2H_TX_LEN ? USB_PID_IN << 4 | 0x00 : USB_PID_OUT << 4 | 0x00), RB_UH_R_TOG | RB_UH_T_TOG, 200000 / 20); // STATUS�׶�
    if(s != ERR_SUCCESS)
        return (s);
    if(R8_U2H_TX_LEN == 0)
        return (ERR_SUCCESS); // ״̬OUT
    if(R8_USB2_RX_LEN == 0)
        return (ERR_SUCCESS);  // ״̬IN,���IN״̬�������ݳ���
    return (ERR_USB_BUF_OVER); // IN״̬�׶δ���
}

/*********************************************************************
 * @fn      CopyU2SetupReqPkg
 *
 * @brief   ���ƿ��ƴ���������
 *
 * @param   pReqPkt     - �����������ַ
 *
 * @return  none
 */
void CopyU2SetupReqPkg(const uint8_t *pReqPkt) // ���ƿ��ƴ���������
{
    uint8_t i;
    for(i = 0; i != sizeof(USB_SETUP_REQ); i++)
    {
        ((uint8_t *)pU2SetupReq)[i] = *pReqPkt;
        pReqPkt++;
    }
}

/*********************************************************************
 * @fn      CtrlGetU2DeviceDescr
 *
 * @brief   ��ȡ�豸������,������ pHOST_TX_RAM_Addr ��
 *
 * @param   none
 *
 * @return  ERR_USB_BUF_OVER    ���������ȴ���
 *          ERR_SUCCESS         �ɹ�
 */
uint8_t CtrlGetU2DeviceDescr(void)
{
    uint8_t s;
    uint8_t len;

    Usb2DevEndp0Size = DEFAULT_ENDP0_SIZE;
    CopyU2SetupReqPkg((uint8_t *)SetupGetU2DevDescr);
    s = U2HostCtrlTransfer(U2Com_Buffer, &len); // ִ�п��ƴ���
    if(s != ERR_SUCCESS)
        return (s);
    Usb2DevEndp0Size = ((PUSB_DEV_DESCR)U2Com_Buffer)->bMaxPacketSize0; // �˵�0��������,���Ǽ򻯴���,����Ӧ���Ȼ�ȡǰ8�ֽں���������UsbDevEndp0Size�ټ���
    if(len < ((PUSB_SETUP_REQ)SetupGetU2DevDescr)->wLength)
        return (ERR_USB_BUF_OVER); // ���������ȴ���
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      CtrlGetU2ConfigDescr
 *
 * @brief   ��ȡ����������,������ pHOST_TX_RAM_Addr ��
 *
 * @param   none
 *
 * @return  ERR_USB_BUF_OVER    ���������ȴ���
 *          ERR_SUCCESS         �ɹ�
 */
uint8_t CtrlGetU2ConfigDescr(void)
{
    uint8_t s;
    uint8_t len;

    CopyU2SetupReqPkg((uint8_t *)SetupGetU2CfgDescr);
    s = U2HostCtrlTransfer(U2Com_Buffer, &len); // ִ�п��ƴ���
    if(s != ERR_SUCCESS)
        return (s);
    if(len < ((PUSB_SETUP_REQ)SetupGetU2CfgDescr)->wLength)
        return (ERR_USB_BUF_OVER); // ���س��ȴ���

    len = ((PUSB_CFG_DESCR)U2Com_Buffer)->wTotalLength;
    CopyU2SetupReqPkg((uint8_t *)SetupGetU2CfgDescr);
    pU2SetupReq->wLength = len;                 // �����������������ܳ���
    s = U2HostCtrlTransfer(U2Com_Buffer, &len); // ִ�п��ƴ���
    if(s != ERR_SUCCESS)
        return (s);

#ifdef DISK_BASE_BUF_LEN
    if(len > 64)
        len = 64;
    memcpy(TxBuffer, U2Com_Buffer, len); //U�̲���ʱ����Ҫ������TxBuffer
#endif

    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      CtrlSetUsb2Address
 *
 * @brief   ����USB�豸��ַ
 *
 * @param   addr    - �豸��ַ
 *
 * @return  ERR_SUCCESS     �ɹ�
 */
uint8_t CtrlSetUsb2Address(uint8_t addr)
{
    uint8_t s;

    CopyU2SetupReqPkg((uint8_t *)SetupSetUsb2Addr);
    pU2SetupReq->wValue = addr;         // USB�豸��ַ
    s = U2HostCtrlTransfer(NULL, NULL); // ִ�п��ƴ���
    if(s != ERR_SUCCESS)
        return (s);
    SetHostUsb2Addr(addr); // ����USB������ǰ������USB�豸��ַ
    mDelaymS(10);          // �ȴ�USB�豸��ɲ���
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      CtrlSetUsb2Config
 *
 * @brief   ����USB�豸����
 *
 * @param   cfg     - ����ֵ
 *
 * @return  ERR_SUCCESS     �ɹ�
 */
uint8_t CtrlSetUsb2Config(uint8_t cfg)
{
    CopyU2SetupReqPkg((uint8_t *)SetupSetUsb2Config);
    pU2SetupReq->wValue = cfg;               // USB�豸����
    return (U2HostCtrlTransfer(NULL, NULL)); // ִ�п��ƴ���
}

/*********************************************************************
 * @fn      CtrlClearU2EndpStall
 *
 * @brief   ����˵�STALL
 *
 * @param   endp    - �˵��ַ
 *
 * @return  ERR_SUCCESS     �ɹ�
 */
uint8_t CtrlClearU2EndpStall(uint8_t endp)
{
    CopyU2SetupReqPkg((uint8_t *)SetupClrU2EndpStall); // ����˵�Ĵ���
    pU2SetupReq->wIndex = endp;                        // �˵��ַ
    return (U2HostCtrlTransfer(NULL, NULL));           // ִ�п��ƴ���
}

/*********************************************************************
 * @fn      CtrlSetUsb2Intercace
 *
 * @brief   ����USB�豸�ӿ�
 *
 * @param   cfg     - ����ֵ
 *
 * @return  ERR_SUCCESS     �ɹ�
 */
uint8_t CtrlSetUsb2Intercace(uint8_t cfg)
{
    CopyU2SetupReqPkg((uint8_t *)SetupSetUsb2Interface);
    pU2SetupReq->wValue = cfg;               // USB�豸����
    return (U2HostCtrlTransfer(NULL, NULL)); // ִ�п��ƴ���
}

/*********************************************************************
 * @fn      USB2_HostInit
 *
 * @brief   USB�������ܳ�ʼ��
 *
 * @param   none
 *
 * @return  none
 */
void USB2_HostInit(void)
{
    R8_USB2_CTRL = RB_UC_HOST_MODE;
    R8_U2HOST_CTRL = 0;
    R8_USB2_DEV_AD = 0x00;

    R8_U2H_EP_MOD = RB_UH_EP_TX_EN | RB_UH_EP_RX_EN;
    R16_U2H_RX_DMA = (uint16_t)(uint32_t)pU2HOST_RX_RAM_Addr;
    R16_U2H_TX_DMA = (uint16_t)(uint32_t)pU2HOST_TX_RAM_Addr;

    R8_U2H_RX_CTRL = 0x00;
    R8_U2H_TX_CTRL = 0x00;
    R8_USB2_CTRL = RB_UC_HOST_MODE | RB_UC_INT_BUSY | RB_UC_DMA_EN;
    R8_U2H_SETUP = RB_UH_SOF_EN;
    R8_USB2_INT_FG = 0xFF;
    DisableRootU2HubPort();
    R8_USB2_INT_EN = RB_UIE_TRANSFER | RB_UIE_DETECT;

    FoundNewU2Dev = 0;
}
