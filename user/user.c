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
  * Description:  心跳包
  * Parameter:    void
  * Return:       0 --设备正常，
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
                USER_DBG("未获取到服务器数据");
            }
        }

        if (0 == sCount) {
           status = Net_Connect_Check();
           if (status) {
                USER_DBG("已关闭连接");
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
  * Description:  数据接收
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
  * Description:  数据发送
  * Parameter:    data --发送的数据指针，len --数据大小
  * Return:       void
  * Others:       add by zlk, 2017-06-15
  ******************************************************************************
  */ 
uint8 Net_Device_SendData(unsigned char *data, unsigned short len)
{
#if(NET_DEVICE_TRANS == 1)
    GSM_SendString(data, len);  //发送设备连接请求数据
#else
    char cmdBuf[30];
    USART_IO_INFO gsmRevBuf = {0};

    OSTimeDly(10);

    GSM_IO_ClearRecive();
    sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);
    //收到‘>’时可以发送数据
    if (GSM_Device_SendCmd(cmdBuf, ">", &gsmRevBuf)) {
        USER_DBG("发送失败");
        return 1;
    }
    
    GSM_SendString(data, len);  //发送设备连接请求数据
    USER_DBG("发送成功");
    
    return 0;
#endif
}


/**
  ******************************************************************************
  * Function:     Net_Device_GetData()
  * Description:  数据接收
  * Parameter:    revBuf --接收的数据
  * Return:       0 --获取成功，1 --未获取到数据
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
  * Description:  连接服务器
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
                USER_DBG("模块异常");
            } else {
                checkDeviceInfo.netDeviceStatus = NETWORK_OK;
            }
        }

        OSTimeDly(200);    //延时 1s
    }
}


/**
  ******************************************************************************
  * Function:     Net_Connect_Check()
  * Description:  检测是否与服务器连接正常
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
  * Description:  处理平台下发的数据
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
  * Description:  模式设置
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
  * Description:  模式获取
  * Parameter:    void
  * Return:       当前模式状态
  * Others:       add by zlk, 2017-06-15
  ******************************************************************************
  */ 
unsigned char Net_Device_Get_DataMode(void)
{
    return (deviceDataType.dataType);
}

