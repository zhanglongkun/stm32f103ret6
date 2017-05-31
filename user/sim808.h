/**
  ******************************************************************************
  * @@FileName��   gsm.h 
  * @author��      zlk
  * @version��     V1.0
  * @Date:         2017-5-31 21:24:24
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef _GSM_H_
#define _GSM_H_

#include "usart.h"

#define GSM_IO USART2

typedef unsigned char uint8;


typedef struct
{
    unsigned short err : 2; 		//��������
    unsigned short initStep : 4;	//��ʼ������
    unsigned char cardType : 3;		//�ֻ���Ϊ1��������Ϊ5
    unsigned short dataType : 4;	//�趨���ݷ�������--16��
    unsigned short reboot : 1;		//����������־
    unsigned short reverse : 2;		//Ԥ��

    unsigned short ipdBytes;
} NET_DEVICE_INFO;

typedef enum
{
    SIM_OK,
    SIM_COMMUNTION_ERR,
    SIM_CPIN_ERR,
    SIM_CREG_FAIL
}sim_status;


void GSM_IO_Init(unsigned int baud);

sim_status GSM_Device_Check();

uint8 GSM_Device_Init();

uint8 GSM_IO_WaitRecive();

void GSM_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned char len);

uint8 GSM_Device_SendCmd(char *cmd, char *res, _Bool mode);

#endif  /* _GSM_H_ */


