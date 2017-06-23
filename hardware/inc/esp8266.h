/**
  ******************************************************************************
  * @@FileName：   esp8266.h 
  * @author：      zlk
  * @version：     V1.0
  * @Date:         2017-6-9 9:21:40
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef _ESP8266_H_
#define _ESP8266_H_

#include <stdio.h>

//单片机头文件
#include "stm32f10x.h"

#include "usart.h"


#define ESP8266_IO USART2

typedef struct
{
    char ip[16];
    char port[8];
} SERVICE_INFO;


typedef unsigned char uint8;

typedef struct 
{
    unsigned char SSID[20];
    unsigned char passWord[20];
    unsigned short err : 2; 		//错误类型
    unsigned short initStep : 4;	//初始化步骤
    unsigned short dataType : 4;	//设定数据返回类型--16种
    unsigned short reverse : 6;		//预留
} WIFI_INFO;

typedef enum
{
    ESP8266_OK,
    ESP8266_CWMODE_ERR,
    ESP8266_CIPMODE_ERR,
    ESP8266_CWJAP_ERR,
    ESP8266_CIPSTART_ERR
} esp8266_statu;

typedef enum
{
    ESP8266_AT_CWMODE,
    ESP8266_AT_CIPMODE,
    ESP8266_AT_CWJAP,
    ESP8266_AT_CIPSTART
} esp8266_init_step;

void ESP8266_IO_Init();

void ESP8266_Init();

uint8 ESP8266_Device_InitStep();

void ESP8266_Device_Init();

uint8 ESP8266_IO_WaitRecive(void);

uint8 ESP8266_Device_SendCmd(char *cmd, char *res, USART_IO_INFO *revBuf);

void ESP8266_IO_ClearRecive(void);

void ESP8266_SendString(unsigned char *str, unsigned char len);

void ESP8266_QuitTrans(void);

void ESP8266_EnterTrans(void);

void NET_DEVICE_Reset(void);

#endif  /* _ESP8266_H_ */


