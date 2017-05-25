/**
  ******************************************************************************
  * @@FileName：   usart.h 
  * @author：      zlk
  * @version：     V1.0
  * @Date:         2017-5-19 下午 11:53:42
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef __USART_H
#define __USART_H


#include "stm32f10x.h"

#define USART_DEBUG		USART1		//调试打印所使用的串口组

typedef struct
{
    unsigned short dataLen;			//接收数据长度
    unsigned short dataLenPre;		//上一次的长度数据，用于比较

    unsigned char buf[200];			//接收缓存
}USART_IO_INFO;

#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志

void Usart1_Init(unsigned int baud);

void Usart2_Init(unsigned int baud);

void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);



#endif  /* __USART_H */


