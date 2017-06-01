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
  * Description:  led����
  * Parameter:    ledNum --led��ţ�status --led״̬
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
  * Description:  ����
  * Parameter:    ����
  * Return:       ����ֵ
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
  * Description:  �������ַ���ת���ɶ�Ӧ�Ľṹ��
  * Parameter:    usartRev --��������
  * Return:       0 --�ɹ���-1 --���ݳ���
  * Others:       add by zlk, 2017-05-31
  ******************************************************************************
  */ 
int Conversion_Handle(USART_IO_INFO usartRev)
{
    unsigned short checkCode = 0;
    ST_VAR_PELCO_D pelco;

    memset(&pelco, 0, sizeof(ST_VAR_PELCO_D));

    pelco.syncByte = usartRev.buf[0];
    pelco.addressCode = usartRev.buf[1];
    pelco.instructionCode = usartRev.buf[3] + usartRev.buf[2] << 8;
    pelco.dataCode1 = usartRev.buf[4];
    pelco.dataCode2 = usartRev.buf[5];
    pelco.checkCode = usartRev.buf[6];

    //�ж�У����
    checkCode = (usartRev.buf[1] + usartRev.buf[2] + usartRev.buf[3]
                 + usartRev.buf[4] + usartRev.buf[5]) % 256;

    if (checkCode != pelco.checkCode) {
      
      UsartPrintf(USART1, "data error\r\n");
      return -1;
    }

    UsartPrintf(USART1, "........................\r\n");
    UsartPrintf(USART1, "0x%x\r\n", pelco.syncByte);
    UsartPrintf(USART1, "0x%x\r\n", pelco.addressCode);
    UsartPrintf(USART1, "0x%x\r\n", pelco.instructionCode);
    UsartPrintf(USART1, "0x%x\r\n", pelco.dataCode1);
    UsartPrintf(USART1, "0x%x\r\n", pelco.dataCode2);
    UsartPrintf(USART1, "0x%x\r\n", pelco.checkCode);

    Manager(pelco);

    return 0;
}
