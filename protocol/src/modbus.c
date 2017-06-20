/**
  ******************************************************************************
  * @FileName:     modbus.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-6-9 10:29:04
  * @Description:  This file provides all the modbus.c functions. 
  ******************************************************************************
  */ 


#include "modbus.h"


#define MODBUS_DBG(fmt, args...) \
    do {\
        UsartPrintf(USART1, "%s-->%s(%d)-->", __FILE__, __func__, __LINE__); \
        UsartPrintf(USART1, fmt, ##args);\
        UsartPrintf(USART1, "\r\n");\
    } while (0)



/**
  ******************************************************************************
  * Function:     Modbus_String_Send()
  * Description:  数据发送
  * Parameter:    data --发送数据指针， dataLen --数据长度
  * Return:       void
  * Others:       add by zlk, 2017-06-14
  ******************************************************************************
  */ 
void Modbus_String_Send(uint8 *data, uint8 dataLen)
{
    Usart_SendString(MODBUS_USART, data, dataLen);

    return;
}


/**
  ******************************************************************************
  * Function:     CrC_Check_Compute()
  * Description:  计算检验值
  * Parameter:    CmmBuf --检验数据指针，len --检验数据长度(扣去检验位)
  * Return:       返回值(低地址放在高字节，高地址放在低字节)
  * Others:       add by zlk, 2017-06-14
  ******************************************************************************
  */ 
uint16 CrC_Check_Compute(uint8 *CmmBuf, uint8 len)
{
    uint16 crc, crcTmp = 0xFFFF;
    uint8 i;
    uint8 j;
    
    if(len > 1024) 
        return 1;
    
    j = 0;
    while(j < len) {
        crcTmp ^= CmmBuf[j];
        
        for(i=0; i < 8; i++) {
            if(crcTmp & 0x01) {
                crcTmp >>= 1;
                crcTmp ^= 0xA001;
            } else {
                crcTmp >>= 1;
            }
        }
        j++;
    }

    crc = ((crcTmp % 0x100) << 8) + (crcTmp / 0x100);
    
    return crc;
}


/**
  ******************************************************************************
  * Function:     Modbus_Excep_Response()
  * Description:  错误数据发送
  * Parameter:    function --功能码，exception --错误码
  * Return:       1 --发送成功
  * Others:       add by zlk, 2017-06-14
  ******************************************************************************
  */ 
uint8 Modbus_Excep_Response(uint8 function, uint8 exception)
{
    uint16 crc;
    uint8 excepRsp[6] = {0};

    excepRsp[0] = 0x01;
    excepRsp[1] = function + 0x80;
    excepRsp[2] = exception;

    crc = CrC_Check_Compute(excepRsp, 3);

    excepRsp[3] = crc / 0x100;
    excepRsp[4] = crc % 0x100;
    
    Modbus_String_Send(excepRsp, 5);

    return 1;
}


/**
  ******************************************************************************
  * Function:     Modbus_Led_Set()
  * Description:  LED 控制
  * Parameter:    registerAdd --LED，data --状态
  * Return:       0 --设置成功，1 --设置失败
  * Others:       add by zlk, 2017-06-14
  ******************************************************************************
  */ 
uint8 Modbus_Led_Set(uint16 registerAdd, uint16 data)
{
    int ret = 0;
    uint16 crc;
    unsigned char bufRsp[8] = {0};

    switch (registerAdd) {
      case 0x0004:
          if (1 == data) {
              Led4_Set(LED_ON);
          } else if (0 == data) {
              Led4_Set(LED_OFF);
          } else {
              ret = Modbus_Excep_Response(registerAdd, DATA_ERR);
          }
          break;
          
      case 0x0005:
          if (1 == data) {
              Led5_Set(LED_ON);
          } else if (0 == data) {
              Led5_Set(LED_OFF);
          } else {
              ret = Modbus_Excep_Response(registerAdd, DATA_ERR);
          }
          break;

      case 0x0006:
          if (1 == data) {
              Led6_Set(LED_ON);
          } else if (0 == data) {
              Led6_Set(LED_OFF);
          } else {
              ret = Modbus_Excep_Response(registerAdd, DATA_ERR);
          }
          break;

      case 0x0007:
          if (1 == data) {
              Led7_Set(LED_ON);
          } else if (0 == data) {
              Led7_Set(LED_OFF);
          } else {
              ret = Modbus_Excep_Response(registerAdd, DATA_ERR);
          }
          break;

      default:
          ret = Modbus_Excep_Response(registerAdd, REGISTERADD_ERR);
    }

    if (1 == ret) {
        return 1;
    }

    bufRsp[0] = 0x01;
    bufRsp[1] = 0x06;
    bufRsp[2] = registerAdd / 0x10;
    bufRsp[3] = registerAdd % 0x10;
    bufRsp[4] = data / 0x10;
    bufRsp[5] = data %0x10;
    
    crc = CrC_Check_Compute(bufRsp, 6);
    bufRsp[6] = crc / 0x100;
    bufRsp[7] = crc % 0x100;
     
    Modbus_String_Send(bufRsp, 8);
    
    return 0;
}


/**
  ******************************************************************************
  * Function:     Modbus_Manager()
  * Description:  管理函数
  * Parameter:    stmodbusInfo --接收数据
  * Return:       void
  * Others:       add by zlk, 2017-06-14
  ******************************************************************************
  */ 
void Modbus_Manager(ST_VAR_MODBUD_REQ_REQ stmodbusInfo)
{
    uint8 ret;

    switch (stmodbusInfo.function) {
        case 0x06:
            ret =  Modbus_Led_Set(stmodbusInfo.registerAdd, stmodbusInfo.data);
            break;
            
        default:
            Modbus_Excep_Response(stmodbusInfo.function, FUNCTION_ERR);
            break;
    }
}


/**
  ******************************************************************************
  * Function:     Modbus_Conversion_Handle()
  * Description:  数据处理
  * Parameter:    usartRev --串口接收数据
  * Return:       0 --成功，1 --失败
  * Others:       add by zlk, 2017-06-14
  ******************************************************************************
  */ 
uint8 Modbus_Conversion_Handle(USART_IO_INFO usartRev)
{
    ST_VAR_MODBUD_REQ_REQ stmodbusInfo = {0};
    uint16 crc;
    int i = 0;
    unsigned short len = usartRev.dataLenPre;
    unsigned char *buf;

    buf = malloc(len + 1);
    memset(buf, 0, len + 1);

    memcpy(buf, usartRev.buf, len);
    for (i = 0; i < len; i++ ) {
        
//        MODBUS_DBG("0x%x", buf[i]);
    }

    crc = CrC_Check_Compute(buf, len - 2);
    if ((buf[len - 2] == (crc / 0x100)) && (buf[len - 1] == (crc % 0x100))) {
//        MODBUS_DBG("检验正确");
    } else {
        free(buf);
        return 1;
//        MODBUS_DBG("校验失败，请检查数据");
    }

    stmodbusInfo.deviceAdd = buf[0];
    stmodbusInfo.function = buf[1];
    stmodbusInfo.registerAdd = buf[3] + (buf[2] << 8);
    stmodbusInfo.data = buf[5] + (buf[4] << 8);
    stmodbusInfo.checkCode = buf[7] + (buf[6] << 8);

    if (0x01 == stmodbusInfo.deviceAdd) {
        Modbus_Manager(stmodbusInfo);
    } else {
        
    }
    
    free(buf);
    
    return 0;
}


