/**
  ******************************************************************************
  * @@FileName：   modbus.h 
  * @author：      zlk
  * @version：     V1.0
  * @Date:         2017-6-9 10:28:59
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef _MODBUS_H_
#define _MODBUS_H_

#include <string.h>
#include <stdlib.h>

#include "usart.h"
#include "led.h"


#define MODBUS_USART USART1

typedef unsigned char uint8;
typedef unsigned short uint16;


typedef enum
{
    FUNCTION_ERR = 0x01,  //非法功能码
    REGISTERADD_ERR,      //非法数据地址
    DATA_ERR,             //非法数据内容
    DEVICE_ERR            //子设备错误
} modbusErr;


typedef struct _ST_VAR_MODBUS_REQ
{
    uint8 deviceAdd;
    uint8 function;
    uint16 registerAdd;
    uint16 data;
    uint16 checkCode;
} ST_VAR_MODBUD_REQ_REQ;


typedef struct _ST_VAR_MODBUS_RSP
{
    uint8 deviceAdd;
    uint8 function;
    uint16 registerAdd;
    uint16 data;
    uint16 checkCode;
} ST_VAR_MODBUD_RSP;


typedef struct _ST_VAR_EXCEP_RSP
{
    uint8 deviceAdd;
    uint8 function;
    uint8 exceptionCode;
    uint16 checkCode;
} ST_VAR_EXCEP_RSP;


void Modbus_String_Send(uint8 *data, uint8 dataLen);

uint16 CrC_Check_Compute(uint8 *CmmBuf, uint8 len);

uint8 Modbus_Excep_Response(uint8 function, uint8 exception);

uint8 Modbus_Led_Set(uint16 registerAdd, uint16 data);

void Modbus_Manager(ST_VAR_MODBUD_REQ_REQ modbusInfo);

uint8 Modbus_Conversion_Handle(USART_IO_INFO usartRev);


#endif  /* _MODBUS_H_ */


