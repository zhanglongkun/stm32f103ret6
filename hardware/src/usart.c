/**
  ******************************************************************************
  * @FileName:     usart.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-20 ���� 12:05:28
  * @Description:  This file provides all the usart.c functions. 
  ******************************************************************************
  */


//Ӳ������
#include "usart.h"
#include "delay.h"

//C��
#include <stdarg.h>
#include <string.h>


void Usart1_Init(unsigned int baud)
{
    GPIO_InitTypeDef gpioInitStruct;
    USART_InitTypeDef usartInitStruct;
    NVIC_InitTypeDef nvicInitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);							//��GPIOA��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);							//��USART1��ʱ��

    //PA9	TXD
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;										//����Ϊ����ģʽ
    gpioInitStruct.GPIO_Pin = GPIO_Pin_9;											//��ʼ��Pin9
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;									//���ص����Ƶ��
    GPIO_Init(GPIOA, &gpioInitStruct);												//��ʼ��GPIOA

    //PA10	RXD
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;								//����Ϊ��������
    gpioInitStruct.GPIO_Pin = GPIO_Pin_10;											//��ʼ��Pin10
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;									//���ص����Ƶ��
    GPIO_Init(GPIOA, &gpioInitStruct);												//��ʼ��GPIOA

    usartInitStruct.USART_BaudRate = baud;
    usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
    usartInitStruct.USART_Parity = USART_Parity_No;									//��У��
    usartInitStruct.USART_StopBits = USART_StopBits_1;								//
    usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
    USART_Init(USART1, &usartInitStruct);

    USART_Cmd(USART1, ENABLE);														//ʹ�ܴ���

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�

    nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;									//usart1�жϺ�
    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;										//�ж�ͨ��ʹ��
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;							//��ռ�ж����ȼ�(ֵԽС���ȼ�Խ��)
    nvicInitStruct.NVIC_IRQChannelSubPriority = 2;									//���ж����ȼ�(ֵԽС���ȼ�Խ��)
    NVIC_Init(&nvicInitStruct);														//��ʼ��NVIC
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
    usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
    usartInitStruct.USART_Parity = USART_Parity_No;									//��У��
    usartInitStruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
    usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
    USART_Init(USART2, &usartInitStruct);

    USART_Cmd(USART2, ENABLE);														//ʹ�ܴ���

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�

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
        USART_SendData(USARTx, *str++);									//��������
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ��������
    }
}


void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
    unsigned char UsartPrintfBuf[296];
    va_list ap;
    unsigned char *pStr = UsartPrintfBuf;

    va_start(ap, fmt);
    vsprintf((char *)UsartPrintfBuf, fmt, ap);							//��ʽ��
    va_end(ap);

    while(*pStr != 0)
    {
        USART_SendData(USARTx, *pStr++);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    }
}

