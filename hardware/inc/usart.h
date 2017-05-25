/**
  ******************************************************************************
  * @@FileName��   usart.h 
  * @author��      zlk
  * @version��     V1.0
  * @Date:         2017-5-19 ���� 11:53:42
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef __USART_H
#define __USART_H


#include "stm32f10x.h"

#define USART_DEBUG		USART1		//���Դ�ӡ��ʹ�õĴ�����

typedef struct
{
    unsigned short dataLen;			//�������ݳ���
    unsigned short dataLenPre;		//��һ�εĳ������ݣ����ڱȽ�

    unsigned char buf[200];			//���ջ���
}USART_IO_INFO;

#define REV_OK		0	//������ɱ�־
#define REV_WAIT	1	//����δ��ɱ�־

void Usart1_Init(unsigned int baud);

void Usart2_Init(unsigned int baud);

void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);



#endif  /* __USART_H */


