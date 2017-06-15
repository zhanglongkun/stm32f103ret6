/**
  ******************************************************************************
  * @@FileName：   gsm.h 
  * @author：      zlk
  * @version：     V1.0
  * @Date:         2017-5-31 21:24:24
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef _GSM_H_
#define _GSM_H_


#include <stdio.h>

#include "usart.h"

//OS
#include "includes.h"

#define GSM_IO USART2

typedef unsigned char uint8;

typedef struct
{
    char ip[16];
    char port[8];
} SERVICE_INFO;

typedef struct
{
    unsigned short err : 2; 		//错误类型
    unsigned short initStep : 4;	//初始化步骤
    unsigned char cardType : 3;		//手机卡为1，物联卡为5
    unsigned short dataType : 4;	//设定数据返回类型--16种
    unsigned short reboot : 1;		//死机重启标志
    unsigned short reverse : 2;		//预留
    unsigned short ipdBytes;
} GSM_DEVICE_INFO;

typedef enum
{
    SIM_OK,
    SIM_COMTINUE,
    SIM_COMMUNTION_ERR,
    SIM_CPIN_ERR,
    SIM_CREG_ERR,
    SIM_CSQ_ERR,
    SIM_CGREG_ERR,
    SIM_CGATT_ERR,
    SIM_CIPMODE_ERR,
    SIM_CSTT_ERR,
    SIM_CIICR_ERR,
    SIM_CGDCONT_ERR,
    SIM_CIPCSGP_ERR,
    SIM_CIPSTART_ERR
} sim_status;

typedef enum
{
    GSM_AT,
    GSM_AT_CPIN,
    GSM_AT_CREG,
    GSM_AT_CSQ,
    GSM_AT_CGREG,
    GSM_AT_CGATT,
    GSM_AT_CIPMODE,
    GSM_AT_CSTT,
    GSM_AT_CIICR,
#if 0
    GSM_AT_CGDCONT,
    GSM_AT_CIPCSGP,
#endif
    GSM_AT_CIFSR,
    GSM_AT_CIPSTART,
    GSM_AT_CIPSTATUS,
    GSM_SUCCEED
} gsm_init_step;


extern GSM_DEVICE_INFO gsmDeviceInfo;



void GSM_IO_Init(void);

void GSM_Init(void);

sim_status GSM_Device_Check(void);

uint8 GSM_Device_Init(void);

uint8 GSM_IO_WaitRecive(void);

void GSM_SendString(unsigned char *str, unsigned char len);

uint8 GSM_Device_SendCmd(char *cmd, char *res, USART_IO_INFO *revBuf);

void GSM_IO_ClearRecive(void);

void SIM808_QuitTrans(void);

uint8 GSM_Device_Exist(void);




#endif  /* _GSM_H_ */


