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

void Usart1_Init(unsigned int baud);

void Usart2_Init(unsigned int baud);

void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);



#endif  /* __USART_H */


