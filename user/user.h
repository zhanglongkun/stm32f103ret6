/**
  ******************************************************************************
  * @@FileName��   user.h 
  * @author��      zlk
  * @version��     V1.0
  * @Date:         2017-6-14 16:59:33
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef _USER_H_
#define _USER_H_

#include "sim808.h"
#include "usart.h"


typedef unsigned char uint8;
typedef unsigned short uint16;

#define NET_DEVICE_TRANS 1

#define DEV_OK          1
#define DEV_ERR	        0

#define NETWORK_OK      1
#define NETWORK_ERR     0

#define DEVICE_CMD_MODE			0 	//AT����ģʽ
#define DEVICE_DATA_MODE		1 	//ƽ̨�����·�ģʽ
#define DEVICE_HEART_MODE		2 	//��������ģʽ



typedef struct
{
    unsigned short SHT20Status: 1;		//��ʪ�ȴ�����������־λ
    unsigned short ADXL345Status : 1;		//���ᴫ����������־λ
    unsigned short eepromStatus : 1;		//�洢��������־λ
    unsigned short oledStatus : 1;			//OLE������־λ
    unsigned short netDeviceStatus : 1;	//�����豸������־λ
} CHECK_DEVICE_INFO;


typedef struct
{
    unsigned char netWork;   //1-�������ӣ� 0-δ��������
} NET_WORK_INFO;


typedef struct
{
    unsigned char dataType;
} DEVICE_DATA_TYPE;


uint8 Heart_Data_Send(void);

void Data_Receive(void);

uint8 Net_Device_SendData(unsigned char *data, unsigned short len);

uint8 Net_Device_GetData(USART_IO_INFO *revBuf);

void Net_Connect(void);

uint8 Net_Connect_Check(void);

void Data_Process(void);

void Net_Device_Set_DataMode(unsigned char mode);

unsigned char Net_Device_Get_DataMode(void);



#endif  /* _USER_H_ */


