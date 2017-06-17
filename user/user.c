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
  * Description:  描述
  * Parameter:    参数
  * Return:       返回值
  * Others:       add by zlk, 2017-06-15
  ******************************************************************************
  */ 
uint8 Heart_Data_Send(void)
{
    uint8 ret;
    uint8 heartBuf[] = "heart data";
    uint8 sCount = 5;
    uint8 *revBuf = NULL;

    
    if ((NETWORK_OK != netWorkInfo.netWork) || (DEVICE_DATA_MODE != deviceDataType.dataType)) {
        return 1;
    }

    Net_Device_Set_DataMode(DEVICE_HEART_MODE);

    while (sCount--) {
        Net_Device_SendData(heartBuf, sizeof(heartBuf));

        ret = Net_Device_GetData(revBuf);
        if (ret) {
            USER_DBG("未获取到服务器数据");
            return 1;
        }
    }

    Net_Device_Set_DataMode(DEVICE_DATA_MODE);

    if (NULL != revBuf) {
        free(revBuf);
    }
    
    return 0;
}


/**
  ******************************************************************************
  * Function:     Data_Receive()
  * Description:  描述
  * Parameter:    参数
  * Return:       返回值
  * Others:       add by zlk, 2017-06-16
  ******************************************************************************
  */ 
void Data_Receive(void)
{
    uint8 *revBuf = NULL;
    
    while (1) {
        if ((NETWORK_OK == netWorkInfo.netWork) || (DEVICE_DATA_MODE == deviceDataType.dataType)) {
            Net_Device_GetData(revBuf);
            if (NULL != revBuf) {
                USER_DBG("receive buffer: %s", revBuf);
            }
        }
    }
}


/**
  ******************************************************************************
  * Function:     Net_Device_SendData()
  * Description:  描述
  * Parameter:    参数
  * Return:       返回值
  * Others:       add by zlk, 2017-06-15
  ******************************************************************************
  */ 
void Net_Device_SendData(unsigned char *data, unsigned short len)
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
  * Description:  描述
  * Parameter:    参数
  * Return:       返回值
  * Others:       add by zlk, 2017-06-16
  ******************************************************************************
  */ 
uint8 Net_Device_GetData(uint8 *revBuf)
{
    uint16 count = 200;
    uint8 *buffer = NULL;
    
    while (count--) {
        if(Usart2_IO_WaitRecive() == REV_OK) {
            buffer = (uint8 *) malloc(usart2IOInfo.dataLenPre);
            memset(buffer, 0, sizeof(USART_IO_INFO));
            memcpy(buffer, usart2IOInfo.buf, sizeof(usart2IOInfo.buf));
            
            memset(&usart2IOInfo, 0, sizeof(USART_IO_INFO));
        }
        OSTimeDly(2);
    }
    if (0 == count) {
        return 1;
    }
    
    revBuf = buffer;
    return 0;
}


/**
  ******************************************************************************
  * Function:     Net_Connect()
  * Description:  描述
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-14
  ******************************************************************************
  */ 
void Net_Connect(void)
{
    GSM_IO_Init();
    Net_Device_Set_DataMode(DEVICE_CMD_MODE);

    while (1) {
        if ((NETWORK_ERR == netWorkInfo.netWork) && (DEV_OK == checkDeviceInfo.netDeviceStatus)) {
            Net_Device_Set_DataMode(DEVICE_CMD_MODE);
            GSM_Device_Init();
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
  * Function:     NET_DEVICE_Set_DataMode()
  * Description:  描述
  * Parameter:    void
  * Return:       返回值
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
  * Description:  描述
  * Parameter:    void
  * Return:       返回值
  * Others:       add by zlk, 2017-06-15
  ******************************************************************************
  */ 
unsigned char Net_Device_Get_DataMode(void)
{
    return (deviceDataType.dataType);
}

