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

#define ESP8266_DBG(fmt, args...) \
    do {\
        UsartPrintf(USART1, "%s-->%s(%d)-->", __FILE__, __func__, __LINE__); \
        UsartPrintf(USART1, fmt, ##args);\
        UsartPrintf(USART1, "\r\n");\
    } while (0)


WIFI_INFO wifiInfo = {"JQM-201", "jqmkj123", 0, 0, 0, 0};

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
    USART_IO_INFO gsmRevBuf = {0};
    char cfgBuffer[32] = {0};
    
    switch (wifiInfo.initStep) {
        case ESP8266_AT_CWMODE:
            UsartPrintf(USART1, "AT+CWMODE=1\r\n");
            if (GSM_Device_SendCmd("AT+CWMODE=1\r\n", "OK", &gsmRevBuf)) {
                ESP8266_DBG("设置模式失败, revBuf = %s", gsmRevBuf.buf);
                return ESP8266_CWMODE_ERR;
            }
            ESP8266_DBG("设置模式成功");
            break;
            
        case ESP8266_AT_CIPMODE:
            UsartPrintf(USART1, "AT+CIPMODE=0\r\n");
            if (GSM_Device_SendCmd("AT+CIPMODE=0\r\n", "OK", &gsmRevBuf)) {
                ESP8266_DBG("设置非透传模式失败, revBuf = %s", gsmRevBuf.buf);
                return ESP8266_CIPMODE_ERR;
            }
            ESP8266_DBG("设置非透传模式成功");
            break;
                
        case ESP8266_AT_CWJAP:
			memset(cfgBuffer, 0, sizeof(cfgBuffer));
		
			strcpy(cfgBuffer, "AT+CWJAP=\"");														//填写ssid、pswd
			strcat(cfgBuffer, wifiInfo.SSID);
			strcat(cfgBuffer, "\",\"");
			strcat(cfgBuffer, wifiInfo.passWord);
			strcat(cfgBuffer, "\"\r\n");
			UsartPrintf(USART_DEBUG, "STA Tips:	%s", cfgBuffer);
            ESP8266_DBG("");
            break;
            
        case ESP8266_AT_CIPSTART:
            UsartPrintf(USART1, "AT+CIPSTART=1\r\n");
            if (GSM_Device_SendCmd("AT+CWMODE=1\r\n", "OK", &gsmRevBuf)) {
                ESP8266_DBG(", revBuf = %s", gsmRevBuf.buf);
                return ESP8266_CIPSTART_ERR;
            }
            ESP8266_DBG("");
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


