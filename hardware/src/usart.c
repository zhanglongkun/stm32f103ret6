/**
  ******************************************************************************
  * @FileName:     usart.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-20 上午 12:05:28
  * @Description:  This file provides all the usart.c functions. 
  ******************************************************************************
  */


//硬件驱动
#include "usart.h"
#include "delay.h"

//C库
#include <stdarg.h>
#include <string.h>


void Usart1_Init(unsigned int baud)
{
    GPIO_InitTypeDef gpioInitStruct;
    USART_InitTypeDef usartInitStruct;
    NVIC_InitTypeDef nvicInitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);							//打开GPIOA的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);							//打开USART1的时钟

    //PA9	TXD
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;										//设置为复用模式
    gpioInitStruct.GPIO_Pin = GPIO_Pin_9;											//初始化Pin9
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;									//承载的最大频率
    GPIO_Init(GPIOA, &gpioInitStruct);												//初始化GPIOA

    //PA10	RXD
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;								//设置为浮空输入
    gpioInitStruct.GPIO_Pin = GPIO_Pin_10;											//初始化Pin10
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;									//承载的最大频率
    GPIO_Init(GPIOA, &gpioInitStruct);												//初始化GPIOA

    usartInitStruct.USART_BaudRate = baud;
    usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//接收和发送
    usartInitStruct.USART_Parity = USART_Parity_No;									//无校验
    usartInitStruct.USART_StopBits = USART_StopBits_1;								//
    usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8位数据位
    USART_Init(USART1, &usartInitStruct);

    USART_Cmd(USART1, ENABLE);														//使能串口

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);									//使能接收中断

    nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;									//usart1中断号
    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;										//中断通道使能
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;							//抢占中断优先级(值越小优先级越高)
    nvicInitStruct.NVIC_IRQChannelSubPriority = 2;									//子中断优先级(值越小优先级越高)
    NVIC_Init(&nvicInitStruct);														//初始化NVIC
}


void Usart2_Init(unsigned int baud)
{
    GPIO_InitTypeDef gpioInitStruct;
    USART_InitTypeDef usartInitStruct;
    NVIC_InitTypeDef nvicInitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    //PA2	TXD
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);

    //PA3	RXD
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);

    usartInitStruct.USART_BaudRate = baud;
    usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//接收和发送
    usartInitStruct.USART_Parity = USART_Parity_No;									//无校验
    usartInitStruct.USART_StopBits = USART_StopBits_1;								//1位停止位
    usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8位数据位
    USART_Init(USART2, &usartInitStruct);

    USART_Cmd(USART2, ENABLE);														//使能串口

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//使能接收中断

    nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvicInitStruct);
}


void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
    unsigned short count = 0;

    for(; count < len; count++)
    {
        USART_SendData(USARTx, *str++);									//发送数据
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
    }
}


void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
    unsigned char UsartPrintfBuf[296];
    va_list ap;
    unsigned char *pStr = UsartPrintfBuf;

    va_start(ap, fmt);
    vsprintf((char *)UsartPrintfBuf, fmt, ap);							//格式化
    va_end(ap);

    while(*pStr != 0)
    {
        USART_SendData(USARTx, *pStr++);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    }
}

