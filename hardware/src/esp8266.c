/**
  ******************************************************************************
  * @FileName:     esp8266.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-6-9 9:21:47
  * @Description:  This file provides all the esp8266.c functions. 
  ******************************************************************************
  */ 


#include "esp8266.h"


#define NET_DEVICE_RST_ON		GPIO_ResetBits(GPIOA, GPIO_Pin_0)
#define NET_DEVICE_RST_OFF		GPIO_SetBits(GPIOA, GPIO_Pin_0)


#define ESP8266_DBG(fmt, args...) \
    do {\
        UsartPrintf(USART1, "%s-->%s(%d)-->", __FILE__, __func__, __LINE__); \
        UsartPrintf(USART1, fmt, ##args);\
        UsartPrintf(USART1, "\r\n");\
    } while (0)


WIFI_INFO wifiInfo = {"JQM-201", "jqmkj123", 0, 0, 0, 0};
    
SERVICE_INFO serviceInfo = {"183.230.40.39", "876"};

void ESP8266_IO_Init()
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


void ESP8266_Init()
{
	GPIO_InitTypeDef gpioInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//ESP8266复位引脚
	gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_0;					//GPIOA0-复位
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);

    ESP8266_IO_Init();

    return;
}


uint8 ESP8266_Device_InitStep()
{
	unsigned char errCount = 0;
    USART_IO_INFO gsmRevBuf = {0};
    char cfgBuffer[32] = {0};
    
    switch (wifiInfo.initStep) {
        case 0:

            memset(cfgBuffer, 0, sizeof(cfgBuffer));

            strcpy(cfgBuffer, "AT+CIPSTART=\"TCP\",\"");
            strcat(cfgBuffer, serviceInfo.ip);
            strcat(cfgBuffer, "\",");
            strcat(cfgBuffer, serviceInfo.port);
            strcat(cfgBuffer, "\r\n");
            UsartPrintf(USART_DEBUG, "STA Tips: %s", cfgBuffer);

            while(ESP8266_Device_SendCmd(cfgBuffer, "CONNECT", &gsmRevBuf))              //连接平台，检索“CONNECT”，如果失败会进入循环体
            {
                Led7_Set(LED_ON);
                OSTimeDly(100);

                Led7_Set(LED_OFF);
                OSTimeDly(100);

                if(++errCount >= 10)
                {
                    UsartPrintf(USART_DEBUG, "PT info Error,Use APP -> 8266\r\n");
                    break;
                }
            }

            if(errCount != 10)
            wifiInfo.initStep++;

            break;

        default:
            
    }
}


void ESP8266_Device_Init()
{
    uint8 res = 1;

    while (res) {
        res = ESP8266_Device_InitStep();
    }
}


uint8 ESP8266_IO_WaitRecive(void)
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


uint8 ESP8266_Device_SendCmd(char *cmd, char *res, USART_IO_INFO *revBuf)
{
    unsigned char timeout = 300;
    
    ESP8266_SendString((uint8 *)cmd, strlen((uint8 *)cmd));

    while (timeout--) {
        if (ESP8266_IO_WaitRecive() == REV_OK) {
            memcpy(revBuf, &usart2IOInfo, sizeof(USART_IO_INFO));

            if (strstr((const char *)revBuf->buf, res) != NULL) {
                ESP8266_IO_ClearRecive();
                return 0;
            }
        }

        DelayUs(200);
    }
    
    GSM_IO_ClearRecive();

    return 1;
}


void ESP8266_IO_ClearRecive(void)
{
    usart2IOInfo.dataLen = 0;

    memset(usart2IOInfo.buf, 0, sizeof(usart2IOInfo.buf));
}


void ESP8266_SendString(unsigned char *str, unsigned char len)
{
    unsigned short count = 0;

    for(; count < len; count++)
    {
        USART_SendData(ESP8266_IO, *str++);									//发送数据
        while(USART_GetFlagStatus(ESP8266_IO, USART_FLAG_TC) == RESET);		//等待发送完成
    }
}


void ESP8266_QuitTrans(void)
{
    USART_IO_INFO gsmRevBuf = {0};
    
    while((ESP8266_IO->SR & 0X40) == 0);	//等待发送空
    ESP8266_IO->DR = '+';
    OSTimeDly(3); 					//大于串口组帧时间(10ms)

    while((ESP8266_IO->SR & 0X40) == 0);	//等待发送空
    ESP8266_IO->DR = '+';        
    OSTimeDly(3); 					//大于串口组帧时间(10ms)

    while((ESP8266_IO->SR & 0X40) == 0);	//等待发送空
    ESP8266_IO->DR = '+';        
    OSTimeDly(20);					//等待100ms

    ESP8266_Device_SendCmd("AT+CIPMODE=0\r\n", "OK", &gsmRevBuf); //关闭透传模式
}


void ESP8266_EnterTrans(void)
{
    USART_IO_INFO gsmRevBuf = {0};
    
    ESP8266_Device_SendCmd("AT+CIPMUX=0\r\n", "OK", &gsmRevBuf);	//单链接模式

    ESP8266_Device_SendCmd("AT+CIPMODE=1\r\n", "OK", &gsmRevBuf);//使能透传

    ESP8266_Device_SendCmd("AT+CIPSEND\r\n", ">", &gsmRevBuf);	//发送数据

    OSTimeDly(20);								//等待100ms
}


void NET_DEVICE_Reset(void)
{
#if(NET_DEVICE_TRANS == 1)
    ESP8266_QuitTrans();	//退出透传模式
    UsartPrintf(USART_DEBUG, "Tips:	QuitTrans\r\n");
#endif

    UsartPrintf(USART_DEBUG, "Tips:	ESP8266_Reset\r\n");

    NET_DEVICE_RST_ON;		//复位
    OSTimeDly(50);

    NET_DEVICE_RST_OFF;		//结束复位
    OSTimeDly(200);
}

