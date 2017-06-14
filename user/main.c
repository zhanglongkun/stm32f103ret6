#include <string.h>

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//Ӳ������
#include "delay.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "exti.h"
#include "sht20.h"
#include "i2c.h"
#include "adxl345.h"
#include "hwtimer.h"

//os
#include "includes.h"

#include "modbus.h"
#include "pelco.h"


unsigned char usart1Buf[64];
unsigned char usart1Len;
USART_IO_INFO usart1IOInfo;
USART_IO_INFO usart2IOInfo;

//pelco_d Э��
#define PELCO_D_TASK_PRI 7
#define PELCO_D_STK_SIZE  512
OS_STK PELCO_D_TASK_STK[PELCO_D_STK_SIZE];

//�����ջ����
#define CHECKTASK_TASK_PRI 8
#define CHECKTASK_STK_SIZE 64
OS_STK CHECKTASK_TASK_STK[CHECKTASK_STK_SIZE];

#define MODBUS_TASK_PRI 9
#define MODBUS_STK_SIZE 512
OS_STK MODBUS_TASK_STK[MODBUS_STK_SIZE];


/**
  ******************************************************************************
  * Function:     PELCO_D_Task()
  * Description:  pelco_d Э������
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-02
  ******************************************************************************
  */ 
void PELCO_D_Task()
{
    USART_IO_INFO usartRev;
    unsigned short i = 0;
    
    while (1) {
        if(Usart1_IO_WaitRecive() == REV_OK) {
            UsartPrintf(USART1, "buf = %s\r\n", usart1IOInfo.buf);

            for (i = 0; i < usart1IOInfo.dataLenPre; i++) {
                
                UsartPrintf(USART1, "%d ", usart1IOInfo.buf[i]);
            }
            UsartPrintf(USART1, "\r\n");

            memset(&usartRev, 0, sizeof(USART_IO_INFO));
            memcpy(&usartRev, &usart1IOInfo, sizeof(USART_IO_INFO));
            Conversion_Handle(usartRev);
            
            memset(&usart1IOInfo, 0, sizeof(USART_IO_INFO));
        }
        OSTimeDly(2);
    }
}


/**
  ******************************************************************************
  * Function:     Check_Stack_Task()
  * Description:  ����ջ��С����
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-02
  ******************************************************************************
  */ 
static void Check_Stack_Task()
{
    unsigned int ret;
    OS_STK_DATA pelcoData;

    while (1) {
        ret = OSTaskStkChk(PELCO_D_TASK_PRI, &pelcoData);

        UsartPrintf(USART1, "ret = %d, free = %d, used = %d\r\n", ret, pelcoData.OSFree, pelcoData.OSUsed);
        
        OSTimeDly(10);
    }
}

void Modbus_Task()
{
    USART_IO_INFO usartRev;
    unsigned short i = 0;
        
    while (1) {
        if(Usart1_IO_WaitRecive() == REV_OK) {

            memset(&usartRev, 0, sizeof(USART_IO_INFO));
            memcpy(&usartRev, &usart1IOInfo, sizeof(USART_IO_INFO));

            Modbus_Conversion_Handle(usartRev);
            
            memset(&usart1IOInfo, 0, sizeof(USART_IO_INFO));
        }
        OSTimeDly(2);
    }
}

void Hardware_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//�жϿ�������������

    Delay_Init();										//Systick��ʼ����������ͨ����ʱ

    Led_Init();											//LED�ӿڳ�ʼ��

//    Key_Init();
//    
//    Exti_Key_Init();
    
    Usart1_Init(115200);
    Usart2_Init(115200);
    RTOS_TimerInit();
    
//    IIC_Init();

//    ADXL345_Init();
}


int main(void)
{
    unsigned char err;
    OS_TMR *tmr;                    //�����ʱ�����
    
    Hardware_Init();											//Ӳ����ʼ��
    
    OSInit();
//    UsartPrintf(USART1, "11111111\r\n");


    
    OSTaskCreate(Modbus_Task, (void *)0, &MODBUS_TASK_STK[MODBUS_STK_SIZE - 1], MODBUS_TASK_PRI);



    
#if 0
    OSTaskCreateExt(PELCO_D_Task, (void *)0, &PELCO_D_TASK_STK[PELCO_D_STK_SIZE - 1], PELCO_D_TASK_PRI,
                    PELCO_D_TASK_PRI, &PELCO_D_TASK_STK[0], PELCO_D_STK_SIZE, NULL, OS_TASK_OPT_STK_CHK);
    UsartPrintf(USART1, "222\r\n");
    
    OSTaskCreateExt(Check_Stack_Task, (void *)0, &CHECKTASK_TASK_STK[CHECKTASK_STK_SIZE - 1], CHECKTASK_TASK_PRI,
                    CHECKTASK_TASK_PRI, &CHECKTASK_TASK_STK[0], CHECKTASK_STK_SIZE, NULL, OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK);

    OSTaskCreate(Check_Stack_Task, (void *)0, &CHECKTASK_TASK_STK[CHECKTASK_STK_SIZE - 1], CHECKTASK_TASK_PRI);
#endif
    
#if 0
    //��ʱ�ص�����
    tmr = OSTmrCreate(100, 100, OS_TMR_OPT_PERIODIC, (OS_TMR_CALLBACK)NULL, 0, (INT8U *)"tmr1", &err);
    OSTmrStart(tmr, &err);
#endif
//    UsartPrintf(USART1, "33\r\n");
    OSStart();
//    UsartPrintf(USART1, "444\r\n");
}

