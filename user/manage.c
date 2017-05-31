/**
  ******************************************************************************
  * @FileName:     manager.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-31 16:24:10
  * @Description:  This file provides all the manager.c functions. 
  ******************************************************************************
  */ 


#include "manage.h"


/**
  ******************************************************************************
  * Function:     Led_Set()
  * Description:  led控制
  * Parameter:    ledNum --led编号，status --led状态
  * Return:       void
  * Others:       add by zlk, 2017-05-31
  ******************************************************************************
  */   
void Led_Set(unsigned char ledNum, unsigned char status)
{
  switch (ledNum) {
      case 4:
          if (1 == status) {
              Led4_Set(LED_ON);
          } else {
              Led4_Set(LED_OFF);
          }
          break;
          
      case 5:
          if (1 == status) {
              Led5_Set(LED_ON);
          } else {
              Led5_Set(LED_OFF);
          }
          break;

      case 6:
          if (1 == status) {
              Led6_Set(LED_ON);
          } else {
              Led6_Set(LED_OFF);
          }
          break;

      case 7:
          if (1 == status) {
              Led7_Set(LED_ON);
          } else {
              Led7_Set(LED_OFF);
          }
          break;

      default:
          UsartPrintf(USART1, "parameter error\r\n");
          break;
  }

  return;
}


/**
  ******************************************************************************
  * Function:     Manager()
  * Description:  描述
  * Parameter:    参数
  * Return:       返回值
  * Others:       add by zlk, 2017-05-31
  ******************************************************************************
  */ 
void Manager(ST_VAR_PELCO_D pelco)
{
  if (0xff == pelco.syncByte) {
      if (0x01 == pelco.addressCode) {
          switch (pelco.instructionCode) {
              case 0x0001:
                  Led_Set(pelco.dataCode1, pelco.dataCode2);
                  break;
                  
              case 0x0002:
                  break;

              case 0x0004:
                  break;

              case 0x0008:
                  break;
                  
              case 0x0010:
                  break;

              default:
                  break;
          }
      }
  }

  return;
}


/**
  ******************************************************************************
  * Function:     Conversion_Handle()
  * Description:  将串口字符串转化成对应的结构体
  * Parameter:    usartRev --串口数据
  * Return:       0 --成功，-1 --数据出错
  * Others:       add by zlk, 2017-05-31
  ******************************************************************************
  */ 
int Conversion_Handle(USART_IO_INFO usartRev)
{
  unsigned short checkCode = 0;
  unsigned char num1, num2;
  unsigned char a1 = 0, a2 = 0;
  unsigned short i = 0, j = 0;
  unsigned char buf[8] = {0};
  ST_VAR_PELCO_D pelco;

  memset(&pelco, 0, sizeof(ST_VAR_PELCO_D));
  pbuf = buf;
  
  for (i = 0; i < usartRev.dataLenPre; i++) {
      //将两个字节合并成一个字节
      a1 = usartRev.buf[i];
      sscanf(&a1, "%x", &num1);
      
      a2 = usartRev.buf[++i];
      sscanf(&a2, "%x", &num2);
  
      num1 = num2 + (num1 << 4);

      switch (j) {
          case 0:
              pelco.syncByte = num1;
              j++;
              break;
              
          case 1:
              pelco.addressCode = num1;
              checkCode += num1;
              j++;
              break;
              
          case 2:
              pelco.instructionCode = num1;
              checkCode += num1;
              j++;
              break;
          
          case 3:
              pelco.instructionCode = num1 + (pelco.instructionCode << 8);
              checkCode += num1;
              j++;
              break;
              
          case 4:
              pelco.dataCode1 = num1;
              checkCode += num1;
              j++;
              break;
              
          case 5:
              pelco.dataCode2 = num1;
              checkCode += num1;
              j++;
              break;
          
          case 6:
              pelco.checkCode = num1;
              j++;
              break;
              
          default:
              UsartPrintf(USART1, "error\r\n");
              break;
      }

      
      UsartPrintf(USART1, "%d\r\n", num1);
  }

  //判断校验码
  checkCode = checkCode % 256;

  if (checkCode != pelco.checkCode) {
      
      UsartPrintf(USART1, "data error\r\n");
      return -1;
  }
  
  UsartPrintf(USART1, "........................\r\n");
  UsartPrintf(USART1, "%d\r\n", pelco.syncByte);
  UsartPrintf(USART1, "%d\r\n", pelco.addressCode);
  UsartPrintf(USART1, "%d\r\n", pelco.instructionCode);
  UsartPrintf(USART1, "%d\r\n", pelco.dataCode1);
  UsartPrintf(USART1, "%d\r\n", pelco.dataCode2);
  UsartPrintf(USART1, "%d\r\n", pelco.checkCode);

  Manager(pelco);

  return 0;
}
