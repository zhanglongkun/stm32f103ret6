/**
  ******************************************************************************
  * @FileName:     gsm.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-31 21:24:29
  * @Description:  This file provides all the gsm.c functions. 
  ******************************************************************************
  */ 


#include "sim808.h"

#define GSM_IO USART2

USART_IO_INFO usart1IOInfo;
USART_IO_INFO usart2IOInfo;


void GSM_IO_Init(unsigned int baud)
{
    Usart2_Init(baud);

    return;
}


sim_status GSM_Device_Check()
{
    UsartPrintf(USART1, "AT\r\n");
    if (!GSM_Device_SendCmd("AT\r\n", "OK", 1)) {
        return SIM_COMMUNTION_ERR;
    }
    
    if (!GSM_Device_SendCmd("AT+CPIN?\r\n", "READY", 1)) {
        return SIM_CPIN_ERR;
    }
    
    if (!GSM_Device_SendCmd("AT+CREG?\r\n", "0,1", 1)) {
        return SIM_CREG_FAIL;	
    }

    return SIM_OK;
}


uint8 GSM_Device_Init()
{
    sim_status res = 0;

    res = GSM_Device_Check();    
}


uint8 GSM_IO_WaitRecive()
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

void GSM_IO_ClearRecive(void)
{
    usart2IOInfo.dataLen = 0;

    memset(usart2IOInfo.buf, 0, sizeof(usart2IOInfo.buf));
}


void GSM_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned char len)
{
    unsigned short count = 0;

    for(; count < len; count++)
    {
        USART_SendData(USARTx, *str++);									//发送数据
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
    }
}


uint8 GSM_Device_SendCmd(char *cmd, char *res, _Bool mode)
{
    unsigned char timeout = 300;
    
    GSM_SendString(GSM_IO, (uint8 *)cmd, strlen((uint8 *)cmd));

    while (timeout--) {
        if (GSM_IO_WaitRecive() == REV_OK) {
            if (strstr((const char *)usart2IOInfo.buf, res) != 0) {
                if (mode) {
                    GSM_IO_ClearRecive();
                }
                return 0;
            }
        }

        DelayUs(200);
    }

    return 1;
}


