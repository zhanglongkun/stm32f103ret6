/**
  ******************************************************************************
  * @FileName:     gsm.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-31 21:24:29
  * @Description:  This file provides all the gsm.c functions. 
  ******************************************************************************
  */ 


#include <stdio.h>

#include "sim808.h"



#define GSM_DBG(fmt, args...) \
    do {\
        UsartPrintf(USART1, "%s-->%s(%d)-->", __FILE__, __func__, __LINE__); \
        UsartPrintf(USART1, fmt, ##args);\
        UsartPrintf(USART1, "\r\n");\
    } while (0)


GSM_DEVICE_INFO gsmDeviceInfo = {0};
#if 0
SERVICE_INFO serviceInfo = {"106.75.148.220", "6666"};
#endif

SERVICE_INFO serviceInfo = {"183.230.40.39", "876"};



/**
  ******************************************************************************
  * Function:     GSM_IO_Init()
  * Description:  初始化串口
  * Parameter:    baud --波特率
  * Return:       void
  * Others:       add by zlk, 2017-06-01
  ******************************************************************************
  */ 
void GSM_IO_Init()
{
    GPIO_InitTypeDef gpioInitStruct;
    USART_InitTypeDef usartInitStruct;
    NVIC_InitTypeDef nvicInitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    //PA2   TXD
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);

    //PA3   RXD
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);

    usartInitStruct.USART_BaudRate = 115200;
    usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //接收和发送
    usartInitStruct.USART_Parity = USART_Parity_No;                             //无校验
    usartInitStruct.USART_StopBits = USART_StopBits_1;                          //1位停止位
    usartInitStruct.USART_WordLength = USART_WordLength_8b;                     //8位数据位
    USART_Init(USART2, &usartInitStruct);

    USART_Cmd(USART2, ENABLE);                                                  //使能串口

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                              //使能接收中断

    nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvicInitStruct);

    GSM_IO_ClearRecive();
    
    return;
}


/**
  ******************************************************************************
  * Function:     GSM_Init()
  * Description:  GSM初始化
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-06
  ******************************************************************************
  */ 
void GSM_Init()
{
    GPIO_InitTypeDef gpioInitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

    //
    gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_1;			//GPIOA1-复位
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);

    gpioInitStruct.GPIO_Pin = GPIO_Pin_4;			//GPIOC4-PowerKey
    GPIO_Init(GPIOC, &gpioInitStruct);

    gpioInitStruct.GPIO_Mode = GPIO_Mode_IPD;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_7;			//GPIOA7-status
    GPIO_Init(GPIOA, &gpioInitStruct);

    GPIO_SetBits(GPIOC, GPIO_Pin_4);
    
    GSM_IO_Init();

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
    USART_IO_INFO gsmRevBuf = {0};
    char cfgBuffer[32] = {0};
    
    switch (gsmDeviceInfo.initStep) {
        case GSM_AT:
            //检测模块是否正常
            UsartPrintf(USART1, "AT\r\n");
            if (GSM_Device_SendCmd("AT\r\n", "OK", &gsmRevBuf)) {
                GSM_DBG("未检测到模块, revBuf = %s", gsmRevBuf.buf);
                return SIM_COMMUNTION_ERR;
            }
            GSM_DBG("GSM模块正常");
            
            gsmDeviceInfo.initStep++;
            break;

        case GSM_AT_CPIN:
            //sim卡是否解锁,即检查登入移动设备(ME)的密码.READY(表示 SIM卡正常,同时不需要登入密码)
            UsartPrintf(USART1, "AT+CPIN?\r\n");
            if (GSM_Device_SendCmd("AT+CPIN?\r\n", "READY", &gsmRevBuf)) {
                GSM_DBG("sim卡未解锁");
                return SIM_CPIN_ERR;
            }
            GSM_DBG("sim卡正常");
            
            gsmDeviceInfo.initStep++;
            break;

        case GSM_AT_CREG:
            //网络注册及状态查询
            UsartPrintf(USART1, "AT+CREG?\r\n");
            if (GSM_Device_SendCmd("AT+CREG?\r\n", "0,1", &gsmRevBuf)) {
                if (strstr(&gsmRevBuf.buf, "0,0")) {
                    GSM_DBG("未注册，终端当前并未在搜寻新的运营商");
                }
                if (strstr(&gsmRevBuf.buf, "0,2")) {
                    GSM_DBG("未注册，终端正在搜寻基站");
                }
                if (strstr(&gsmRevBuf.buf, "0,4")) {
                    GSM_DBG("未知代码");
                }
                if (strstr(&gsmRevBuf.buf, "0,5")) {
                    GSM_DBG("已注册，处于漫游状态");
                }
                return SIM_CREG_ERR;
            }
            GSM_DBG("已注册本地网络");
            
            gsmDeviceInfo.initStep++;
            break;

        case GSM_AT_CSQ:
            //查询信号强度
            UsartPrintf(USART1, "AT+CSQ\r\n");
            if (GSM_Device_SendCmd("AT+CSQ\r\n","OK", &gsmRevBuf)) {
                GSM_DBG("查询信号强度失败");
                return SIM_CSQ_ERR;	
            }
            GSM_DBG("查询信号强度成功");
            
            gsmDeviceInfo.initStep++;
            break;

        case GSM_AT_CGREG:
            //检查网络注册状态
            UsartPrintf(USART1, "AT+CGREG?\r\n");
            if (GSM_Device_SendCmd("AT+CGREG?\r\n","OK", &gsmRevBuf)) {
                GSM_DBG("注册失败");
                return SIM_CGREG_ERR;
            }
            GSM_DBG("网络注册正常");
            
            gsmDeviceInfo.initStep++;
            break;

        case GSM_AT_CGATT:
            //附着GPRS业务
            UsartPrintf(USART1, "AT+CGATT=1\r\n");
            if (GSM_Device_SendCmd("AT+CGATT=1\r\n", "OK", &gsmRevBuf)) {
                GSM_DBG("附着GPRS业务失败");
                return SIM_CGATT_ERR;
            }
            GSM_DBG("附着GPRS业务");
            
            gsmDeviceInfo.initStep++;
            break;
            
        case GSM_AT_CGDCONT:
            //设置PDP上下文,互联网接协议,接入点等信息
            UsartPrintf(USART1, "AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");
            if (GSM_Device_SendCmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n", "OK", &gsmRevBuf)) {
                GSM_DBG("设置PDP上下文,互联网接协议,接入点等信息失败");
                return SIM_CGDCONT_ERR;   
            }
            GSM_DBG("设置PDP成功");
            
            gsmDeviceInfo.initStep++;
            break;

        case GSM_AT_CIPCSGP:
            //设置为GPRS连接模式
            UsartPrintf(USART1, "AT+CIPCSGP=1,\"CMNET\"\r\n");
            if (GSM_Device_SendCmd("AT+CIPCSGP=1,\"CMNET\"\r\n", "OK", &gsmRevBuf)) {
                GSM_DBG("设置为GPRS连接模式失败");
                return SIM_CIPCSGP_ERR;   
            }
            GSM_DBG("设置为GPRS连接模式成功");
            
            gsmDeviceInfo.initStep++;
            break;

        case GSM_AT_CIFSR:
            //获取IP地址
            UsartPrintf(USART1, "AT+CIFSR\r\n");
            if (!GSM_Device_SendCmd("AT+CIFSR\r\n", "ERROR", &gsmRevBuf)) {
                GSM_DBG("获取IP地址失败");
                return SIM_CIPCSGP_ERR;   
            }
            GSM_DBG("获取IP地址成功");
            
            gsmDeviceInfo.initStep++;
            break;

        case GSM_AT_CIPSTART:
            //连接服务器
            memset(cfgBuffer, 0, sizeof(cfgBuffer));

            strcpy(cfgBuffer, "AT+CIPSTART=\"TCP\",\"");
            strcat(cfgBuffer, serviceInfo.ip);
            strcat(cfgBuffer, "\",");
            strcat(cfgBuffer, serviceInfo.port);
            strcat(cfgBuffer, "\r\n");
            UsartPrintf(USART1, "STA Tips:	%s", cfgBuffer);
            
            if (GSM_Device_SendCmd(cfgBuffer, "CONNECT", &gsmRevBuf)) {
                GSM_DBG("连接服务器失败");
                return SIM_CIPSTART_ERR;   
            }
            GSM_DBG("连接服务器成功");
            
            gsmDeviceInfo.initStep++;
            break;

        case GSM_AT_CIPSTATUS:
            //状态查询
            UsartPrintf(USART1, "AT+CIPSTATUS\r\n");
            if (GSM_Device_SendCmd("AT+CIPSTATUS\r\n", "CONNECT OK", &gsmRevBuf)) {
                GSM_DBG("服务器未连接");
                return SIM_CIPSTART_ERR;   
            }
            GSM_DBG("以正常连接服务器");
            
            gsmDeviceInfo.initStep++;
            break;

        default:
            UsartPrintf(USART1, "parameter error\r\n");
            break;
    }

    if (GSM_SUCCEED == gsmDeviceInfo.initStep) {
        GSM_DBG("网络设备初始化成功");
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
        DelayMs(1000);
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
void GSM_IO_ClearRecive()
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
uint8 GSM_Device_SendCmd(char *cmd, char *res, USART_IO_INFO *revBuf)
{
    unsigned char timeout = 300;
    
    GSM_SendString(GSM_IO, (uint8 *)cmd, strlen((uint8 *)cmd));

    while (timeout--) {
        if (GSM_IO_WaitRecive() == REV_OK) {
            memcpy(revBuf, &usart2IOInfo, sizeof(USART_IO_INFO));

            if (strstr((const char *)revBuf->buf, res) != NULL) {
                GSM_IO_ClearRecive();
                return 0;
            }
        }

        DelayUs(200);
    }
    
    GSM_IO_ClearRecive();

    return 1;
}


