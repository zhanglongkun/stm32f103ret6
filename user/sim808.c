/**
  ******************************************************************************
  * @FileName:     gsm.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-31 21:24:29
  * @Description:  This file provides all the gsm.c functions. 
  ******************************************************************************
  */ 


#include "sim808.h"


GSM_DEVICE_INFO gsmDeviceInfo = {0};
SERVICE_INFO serviceInfo = {"106.75.148.220", "6666"}



/**
  ******************************************************************************
  * Function:     GSM_IO_Init()
  * Description:  初始化串口
  * Parameter:    baud --波特率
  * Return:       void
  * Others:       add by zlk, 2017-06-01
  ******************************************************************************
  */ 
void GSM_IO_Init(unsigned int baud)
{
    Usart2_Init(baud);

    return;
}


/**
  ******************************************************************************
  * Function:     GSM_Device_InitStep()
  * Description:  GSM设置
  * Parameter:    void
  * Return:       sim_status --进行的状态
  * Others:       add by zlk, 2017-06-01
  ******************************************************************************
  */ 
sim_status GSM_Device_InitStep()
{
    char cfgBuffer[32] = {0};
    
    switch (gsmDeviceInfo.initStep) {
        case 0:
            //检测模块是否正常
            UsartPrintf(USART1, "AT\r\n");
            if (GSM_Device_SendCmd("AT\r\n", "OK", 1)) {
                return SIM_COMMUNTION_ERR;
            }
            gsmDeviceInfo.initStep++;
            break;

        case 1:
            //sim卡是否解锁,即检查登入移动设备(ME)的密码.READY(表示 SIM卡正常,同时不需要登入密码)
            UsartPrintf(USART1, "AT+CPIN?\r\n");
            if (GSM_Device_SendCmd("AT+CPIN?\r\n", "READY", 1)) {
                return SIM_CPIN_ERR;
            }
            gsmDeviceInfo.initStep++;
            break;

        case 2:
            //网络注册及状态查询
            UsartPrintf(USART1, "AT+CREG?\r\n");
            if (GSM_Device_SendCmd("AT+CREG?\r\n", "0,1", 1)) {
                return SIM_CREG_ERR;	
            }
            gsmDeviceInfo.initStep++;
            break;

        case 3:
            //查询信号强度
            UsartPrintf(USART1, "AT+CSQ\r\n");
            if (GSM_Device_SendCmd("AT+CSQ\r\n","OK", 1)) {
                return SIM_CSQ_ERR;	
            }
            gsmDeviceInfo.initStep++;
            break;

        case 4:
            //检查网络注册状态
            UsartPrintf(USART1, "AT+CGREG?\r\n");
            if (GSM_Device_SendCmd("AT+CGREG?\r\n","OK", 1)) {
                return SIM_CGREG_ERR;	
            }
            gsmDeviceInfo.initStep++;
            break;

 

        case 5:
            //附着GPRS业务
            UsartPrintf(USART1, "AT+CGATT=1\r\n");
            if (GSM_Device_SendCmd("AT+CGATT=1\r\n", "OK", 1)) {
                return SIM_CGATT_ERR;	
            }
            gsmDeviceInfo.initStep++;
            break;
            
        case 6:
            //设置PDP上下文,互联网接协议,接入点等信息
            UsartPrintf(USART1, "AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");
            if (GSM_Device_SendCmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n", "OK", 1)) {
                return SIM_CIPSHUT_ERR;   
            }
            gsmDeviceInfo.initStep++;
            break;

        case 7:
            //设置为GPRS连接模式
            UsartPrintf(USART1, "AT+CIPCSGP=1,\"CMNET\"\r\n");
            if (GSM_Device_SendCmd("AT+CIPCSGP=1,\"CMNET\"\r\n", "OK", 1)) {
                return SIM_CIPCSGP_ERR;   
            }
            gsmDeviceInfo.initStep++;
            break;

        
        case 8:
            //获取IP地址
            UsartPrintf(USART1, "AT+CIFSR\r\n");
            if (!GSM_Device_SendCmd("AT+CIFSR\r\n", "ERROR", 1)) {
                return SIM_CIPCSGP_ERR;   
            }
            gsmDeviceInfo.initStep++;
            break;

        
        case 9:
            //连接服务器
            memset(cfgBuffer, 0, sizeof(cfgBuffer));

            strcpy(cfgBuffer, "AT+CIPSTART=\"TCP\",\"");
            strcat(cfgBuffer, serviceInfo.ip);
            strcat(cfgBuffer, "\",");
            strcat(cfgBuffer, serviceInfo.port);
            strcat(cfgBuffer, "\r\n");
            UsartPrintf(USART1, "STA Tips:	%s", cfgBuffer);
            
            if (GSM_Device_SendCmd(cfgBuffer, "CONNECT", 1)) {
                return SIM_CIPSTART_ERR;   
            }
            gsmDeviceInfo.initStep++;
            break;

        
        case 10:
            //状态查询
            UsartPrintf(USART1, "AT+CIPSTATUS\r\n");
            if (GSM_Device_SendCmd("AT+CIPSTATUS\r\n", "CONNECT OK", 1)) {
                return SIM_CIPSTART_ERR;   
            }
            gsmDeviceInfo.initStep++;
            break;

        default:
            UsartPrintf(USART1, "parameter error\r\n");
            break;
    }

    if (11 == gsmDeviceInfo.initStep) {
        return SIM_OK;
    }

    return SIM_COMTINUE;
}


/**
  ******************************************************************************
  * Function:     GSM_Device_Init()
  * Description:  GSM设置
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-01
  ******************************************************************************
  */ 
uint8 GSM_Device_Init()
{
    sim_status res = 1;

    while (res) {
        res = GSM_Device_InitStep();
    }
}


/**
  ******************************************************************************
  * Function:     GSM_IO_WaitRecive()
  * Description:  判断数据是否接收完毕
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-01
  ******************************************************************************
  */ 
uint8 GSM_IO_WaitRecive()
{
    if(usart2IOInfo.dataLen == 0) //如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
        return REV_WAIT;

    if(usart2IOInfo.dataLen == usart2IOInfo.dataLenPre) //如果上一次的值和这次相同，则说明接收完毕
    {
        usart2IOInfo.dataLen = 0; //清0接收计数
    	
        return REV_OK; //跳出
    }

    usart2IOInfo.dataLenPre = usart2IOInfo.dataLen; //置为相同

    return REV_WAIT;
}

/**
  ******************************************************************************
  * Function:     GSM_IO_ClearRecive()
  * Description:  清除串口数据
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-01
  ******************************************************************************
  */ 
void GSM_IO_ClearRecive(void)
{
    usart2IOInfo.dataLen = 0;

    memset(usart2IOInfo.buf, 0, sizeof(usart2IOInfo.buf));
}


/**
  ******************************************************************************
  * Function:     GSM_SendString()
  * Description:  数据发送
  * Parameter:    USARTx --串口， str --发送的数据， len --数据长度
  * Return:       void
  * Others:       add by zlk, 2017-06-01
  ******************************************************************************
  */ 
void GSM_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned char len)
{
    unsigned short count = 0;

    for(; count < len; count++)
    {
        USART_SendData(USARTx, *str++);									//发送数据
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
    }
}


/**
  ******************************************************************************
  * Function:     GSM_Device_SendCmd()
  * Description:  发送命令
  * Parameter:    cmd --命令内容，res --检验字符串，mode --是否清除串口接收的数据
  * Return:       0 --发送成功，1 --发送失败
  * Others:       add by zlk, 2017-06-01
  ******************************************************************************
  */ 
uint8 GSM_Device_SendCmd(char *cmd, char *res, _Bool mode)
{
    unsigned char timeout = 300;
    
    GSM_SendString(GSM_IO, (uint8 *)cmd, strlen((uint8 *)cmd));

    while (timeout--) {
        if (GSM_IO_WaitRecive() == REV_OK) {
            if (strstr((const char *)usart2IOInfo.buf, res) != 0) {
                if (mode) {
                    GSM_IO_ClearRecive();
                }
                return 0;
            }
        }

        DelayUs(200);
    }

    return 1;
}


