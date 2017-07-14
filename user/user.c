/**
  ******************************************************************************
  * @FileName:     user.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-6-14 16:59:40
  * @Description:  This file provides all the user.c functions. 
  ******************************************************************************
  */ 

#include "user.h"

#define USER_DBG(fmt, args...) \
    do {\
        UsartPrintf(USART1, "%s-->%s(%d)-->", __FILE__, __func__, __LINE__); \
        UsartPrintf(USART1, fmt, ##args);\
        UsartPrintf(USART1, "\r\n");\
    } while (0)


CHECK_DEVICE_INFO checkDeviceInfo = {DEV_ERR, DEV_ERR, DEV_ERR, DEV_ERR, DEV_ERR};
NET_WORK_INFO netWorkInfo = {NETWORK_ERR};
DEVICE_DATA_TYPE deviceDataType = {DEVICE_CMD_MODE};


/**
  ******************************************************************************
  * Function:     Heart_Data_Send()
  * Description:  ������
  * Parameter:    void
  * Return:       0 --�豸������
  * Others:       add by zlk, 2017-06-15
  ******************************************************************************
  */ 
uint8 Heart_Data_Send(void)
{
    uint8 ret;
    uint8 heartBuf[] = "heart data";
    uint8 sCount = 5;
    USART_IO_INFO *revBuf = NULL;
    uint8 status;

    revBuf = (USART_IO_INFO *) malloc(sizeof(USART_IO_INFO));
    if (NULL == revBuf) {
        USER_DBG("malloc error");
        return 1;
    }

    while (1) {
        memset(revBuf, 0, sizeof(USART_IO_INFO));
        if ((NETWORK_OK != netWorkInfo.netWork) || (DEVICE_DATA_MODE != deviceDataType.dataType)) {
            USER_DBG("not in device_data_mode");
            return 1;
        }

        sCount = 5;
        
        Net_Device_Set_DataMode(DEVICE_HEART_MODE);

        while (sCount--) {
            Net_Device_SendData(heartBuf, sizeof(heartBuf));

            ret = Net_Device_GetData(revBuf);
            if (ret) {
                USER_DBG("δ��ȡ������������");
            }
        }

        if (0 == sCount) {
           status = Net_Connect_Check();
           if (status) {
                USER_DBG("�ѹر�����");
                netWorkInfo.netWork = NETWORK_ERR;
           }
        }

        Net_Device_Set_DataMode(DEVICE_DATA_MODE);

        if (NULL != revBuf) {
            free(revBuf);
            revBuf = NULL;
        }

        OSTimeDlyHMSM(0, 0, 20, 0);
    }

    free(revBuf);
    return 0;
}


/**
  ******************************************************************************
  * Function:     Data_Receive()
  * Description:  ���ݽ���
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-16
  ******************************************************************************
  */ 
void Data_Receive(void)
{
    USART_IO_INFO *revBuf = NULL;

    revBuf = (USART_IO_INFO *) malloc(sizeof(USART_IO_INFO));
    if (NULL == revBuf) {
        USER_DBG("malloc error");
        return;
    }
    
    while (1) {
        memset(revBuf, 0, sizeof(USART_IO_INFO));
        if ((NETWORK_OK == netWorkInfo.netWork) || (DEVICE_DATA_MODE == deviceDataType.dataType)) {
            Net_Device_GetData(revBuf);
            if (NULL != revBuf) {
                USER_DBG("receive buffer: %s", revBuf);
            }
        }
    }

    free(revBuf);
}


/**
  ******************************************************************************
  * Function:     Net_Device_SendData()
  * Description:  ���ݷ���
  * Parameter:    data --���͵�����ָ�룬len --���ݴ�С
  * Return:       void
  * Others:       add by zlk, 2017-06-15
  ******************************************************************************
  */ 
uint8 Net_Device_SendData(unsigned char *data, unsigned short len)
{
#if(NET_DEVICE_TRANS == 1)
    GSM_SendString(data, len);  //�����豸������������
#else
    char cmdBuf[30];
    USART_IO_INFO gsmRevBuf = {0};

    OSTimeDly(10);

    GSM_IO_ClearRecive();
    sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);
    //�յ���>��ʱ���Է�������
    if (GSM_Device_SendCmd(cmdBuf, ">", &gsmRevBuf)) {
        USER_DBG("����ʧ��");
        return 1;
    }
    
    GSM_SendString(data, len);  //�����豸������������
    USER_DBG("���ͳɹ�");
    
    return 0;
#endif
}


/**
  ******************************************************************************
  * Function:     Net_Device_GetData()
  * Description:  ���ݽ���
  * Parameter:    revBuf --���յ�����
  * Return:       0 --��ȡ�ɹ���1 --δ��ȡ������
  * Others:       add by zlk, 2017-06-16
  ******************************************************************************
  */ 
uint8 Net_Device_GetData(USART_IO_INFO *revBuf)
{
    uint16 count = 200;
    
    while (count--) {
        if(Usart2_IO_WaitRecive() == REV_OK) {
            memcpy(revBuf, &usart2IOInfo, sizeof(usart2IOInfo));
            
            memset(&usart2IOInfo, 0, sizeof(USART_IO_INFO));
        }
        OSTimeDly(2);
    }
    if (0 == count) {
        return 1;
    }
    
    return 0;
}


/**
  ******************************************************************************
  * Function:     Net_Connect()
  * Description:  ���ӷ�����
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-14
  ******************************************************************************
  */ 
void Net_Connect(void)
{
    int ret;
    GSM_IO_Init();
    Net_Device_Set_DataMode(DEVICE_CMD_MODE);

    while (1) {
        if ((NETWORK_ERR == netWorkInfo.netWork) && (DEV_OK == checkDeviceInfo.netDeviceStatus)) {
            Net_Device_Set_DataMode(DEVICE_CMD_MODE);
            
            ret = GSM_Device_Init();
            if (!ret) {
                netWorkInfo.netWork = NETWORK_OK;
            }
            
            Net_Device_Set_DataMode(DEVICE_DATA_MODE);
        }

        if (DEV_ERR == checkDeviceInfo.netDeviceStatus) {
            if (GSM_Device_Exist()) {
                USER_DBG("ģ���쳣");
            } else {
                checkDeviceInfo.netDeviceStatus = NETWORK_OK;
            }
        }

        OSTimeDly(200);    //��ʱ 1s
    }
}


/**
  ******************************************************************************
  * Function:     Net_Connect_Check()
  * Description:  ����Ƿ����������������
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-17
  ******************************************************************************
  */ 
uint8 Net_Connect_Check(void)
{
    USART_IO_INFO revBuf = {0};

    GSM_IO_ClearRecive();

    if (GSM_Device_SendCmd("AT+CIPSTATUS\r\n", "TCP CLOSED", &revBuf)) {
        USER_DBG("service is close, revce buffer:%s", revBuf.buf);
        return 1;
    }

    return 0;
}


/**
  ******************************************************************************
  * Function:     Data_Process()
  * Description:  ����ƽ̨�·�������
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-17
  ******************************************************************************
  */ 
void Data_Process(void)
{
    uint8 ret;
    uint8 sendBuf[] = "I receive the data";
    USART_IO_INFO *revBuf = NULL;

    revBuf = (USART_IO_INFO *) malloc(sizeof(USART_IO_INFO));
    if (NULL == revBuf) {
        USER_DBG("malloc error");
        return;
    }
    
    while (1) {
        memset(revBuf, 0, sizeof(USART_IO_INFO));
        if ((NETWORK_OK== netWorkInfo.netWork) && (DEVICE_DATA_MODE == deviceDataType.dataType)) {
            memset(&revBuf, 0, sizeof(USART_IO_INFO));
            
            ret = Net_Device_GetData(revBuf);
            if (0 != revBuf) {
                USER_DBG("receive buffer = %s", revBuf->buf);
                Net_Device_SendData(sendBuf, sizeof(sendBuf));
            }
        }

        OSTimeDly(2);
    }

    free(revBuf);
}


/**
  ******************************************************************************
  * Function:     NET_DEVICE_Set_DataMode()
  * Description:  ģʽ����
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-15
  ******************************************************************************
  */ 
void Net_Device_Set_DataMode(unsigned char mode)
{
    deviceDataType.dataType = mode;

    return;
}


/**
  ******************************************************************************
  * Function:     NET_DEVICE_Get_DataMode()
  * Description:  ģʽ��ȡ
  * Parameter:    void
  * Return:       ��ǰģʽ״̬
  * Others:       add by zlk, 2017-06-15
  ******************************************************************************
  */ 
unsigned char Net_Device_Get_DataMode(void)
{
    return (deviceDataType.dataType);
}

