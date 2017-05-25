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
#include <stdio.h>


/**
  ******************************************************************************
  * Function:     Usart1_Init()
  * Description:  初始化 UASRT1
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
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


/**
  ******************************************************************************
  * Function:     Usart2_Init()
  * Description:  初始化 USART2
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
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


/**
  ******************************************************************************
  * Function:     Usart_SendString()
  * Description:  发送数据
  * Parameter:    USARTx --串口号， str --字符串，len --发送个数
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
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
  * Function:     UsartPrintf()
  * Description:  串口打印
  * Parameter:    USARTx --串口号
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
    unsigned char UsartPrintfBuf[296];
    va_list ap;
    unsigned char *pStr = UsartPrintfBuf;

    va_start(ap, fmt);
    vsprintf((char *)UsartPrintfBuf, fmt, ap);							//格式化
    va_end(ap);
    /*
        把数据从内存(变量)写入到发送数据寄存器 TDR 后， 发送控制器将适时
        地自动把数据从 TDR 加载到发送移位寄存器，然后通过串口线 Tx，把数据一
        位一位地发送出去，在数据从 TDR 转移到移位寄存器时，会产生发送寄存器
        TDR 已空事件 TXE，当数据从移位寄存器全部发送出去时，会产生数据发送完
        成事件 TC，这些事件可以在状态寄存器中查询到
    */

    while(*pStr != 0)
    {
        USART_SendData(USARTx, *pStr++);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    }
}

extern unsigned char usart1Buf[64];
extern unsigned char usart1Len;
extern USART_IO_INFO usart1IOInfo;
extern USART_IO_INFO usart2IOInfo;


_Bool Usart1_IO_WaitRecive(void)
{
    if(usart1IOInfo.dataLen == 0) //如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
        return REV_WAIT;

    if(usart1IOInfo.dataLen == usart1IOInfo.dataLenPre) //如果上一次的值和这次相同，则说明接收完毕
    {
        usart1IOInfo.dataLen = 0; //清0接收计数
    	
        return REV_OK; //跳出
    }

    usart1IOInfo.dataLenPre = usart1IOInfo.dataLen; //置为相同

    return REV_WAIT;
}

_Bool Usart2_IO_WaitRecive(void)
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


void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	//接收中断
    {
        if(usart1IOInfo.dataLen >= sizeof(usart1IOInfo.buf))
            usart1IOInfo.dataLen = 0; //防止串口被刷爆
        usart1IOInfo.buf[usart1IOInfo.dataLen++] = USART1->DR;

        USART_ClearFlag(USART1, USART_FLAG_RXNE);
    }
}


void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)	//接收中断
    {
        if(usart2IOInfo.dataLen >= sizeof(usart2IOInfo.buf))
            usart2IOInfo.dataLen = 0; //防止串口被刷爆
        usart2IOInfo.buf[usart2IOInfo.dataLen++] = USART2->DR;

        USART_ClearFlag(USART2, USART_FLAG_RXNE);
    }
}


void Usart2_Send_Cmd(unsigned char *str, unsigned short len)
{
    int count = 0;
    	
    for (count = 0; count < len; count++)
	{
		USART_SendData(USART2, *str++);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	}
}

