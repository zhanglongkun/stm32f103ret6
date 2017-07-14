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
#include "sim808.h"
#include "esp8266.h"
#include "hwtimer.h"

//os
#include "includes.h"

#include "user.h"

USART_IO_INFO usart1IOInfo = {0};
USART_IO_INFO usart2IOInfo = {0};


//�����ջ����
#define CHECKTASK_TASK_PRIO    8
#define CHECKTASK_STK_SIZE    64
OS_STK CHECKTASK_TASK_STK[CHECKTASK_STK_SIZE];


//�����ʼ������
#define NET_TASK_PRIO		 9 
#define NET_STK_SIZE		 256
OS_STK NET_TASK_STK[NET_STK_SIZE];


//��������
#define HEART_TASK_PRIO		 10 
#define HEART_STK_SIZE		 256
OS_STK HEART_TASK_STK[HEART_STK_SIZE];


//���ݽ�������
#define DATA_TASK_PRIO       11
#define DATA_STK_SIZE        1024
OS_STK DATA_TASK_STK[DATA_STK_SIZE];

//ESP8266����
#define ESP8266_TASK_PRIO       12
#define ESP8266_STK_SIZE        1024
OS_STK ESP8266_TASK_STK[ESP8266_STK_SIZE];


static void Check_Stack_Task()
{
    unsigned int ret;
    OS_STK_DATA pelcoData;

    while (1) {
        ret = OSTaskStkChk(CHECKTASK_TASK_PRIO, &pelcoData);

        UsartPrintf(USART1, "ret = %d, free = %d, used = %d\r\n", ret, pelcoData.OSFree, pelcoData.OSUsed);
        
        OSTimeDly(10);
    }
}


void Net_Task()
{
    Net_Connect();
}


void Head_Task()
{
    Heart_Data_Send();
}


void Data_Task()
{
    Data_Process();
}

void ESP8266_Init_Task()
{
    ESP8266_Device_Init();
}

void ESP8266_ReceiveBuf_Task()
{
    ESP8266_Device_Init();
    ESP8266_ReceiveString();
}

void Hardware_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//�жϿ�������������

    Delay_Init();										//Systick��ʼ����������ͨ����ʱ

    Led_Init();											//LED�ӿڳ�ʼ��

    Key_Init();

    Exti_Key_Init();

    Usart1_Init(115200);
    
    ESP8266_Init();

    IIC_Init();

    RTOS_TimerInit();

    ADXL345_Init();
}


int main(void)
{
    Hardware_Init();											//Ӳ����ʼ��
    OSInit();

    
#if 0
    OSTaskCreateExt(ESP8266_ReceiveBuf_Task, (void *)0, &ESP8266_TASK_STK[ESP8266_STK_SIZE - 1], ESP8266_TASK_PRIO,
                    ESP8266_TASK_PRIO, &NET_TASK_STK[0], ESP8266_STK_SIZE, NULL, OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK);
#endif

    OSTaskCreateExt(ESP8266_ReceiveBuf_Task, (void *)0, &DATA_TASK_STK[DATA_STK_SIZE - 1], DATA_TASK_PRIO,
                    DATA_TASK_PRIO, &DATA_TASK_STK[0], DATA_STK_SIZE, NULL, OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK);

    OSStart();

    
#if 0
    OSTaskCreateExt(Net_Task, (void *)0, &NET_TASK_STK[NET_STK_SIZE - 1], CHECKTASK_TASK_PRIO,
                    CHECKTASK_TASK_PRIO, &NET_TASK_STK[0], NET_STK_SIZE, NULL, OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK);

    OSTaskCreateExt(Head_Task, (void *)0, &HEART_TASK_STK[HEART_STK_SIZE - 1], HEART_TASK_PRIO,
                    HEART_TASK_PRIO, &HEART_TASK_STK[0], HEART_STK_SIZE, NULL, OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK);

    OSTaskCreateExt(Data_Task, (void *)0, &DATA_TASK_STK[DATA_STK_SIZE - 1], DATA_TASK_PRIO,
                    DATA_TASK_PRIO, &DATA_TASK_STK[0], DATA_STK_SIZE, NULL, OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK);

#endif

    
#if 0
   OSTaskCreateExt(Check_Stack_Task, (void *)0, &CHECKTASK_TASK_STK[CHECKTASK_STK_SIZE - 1], CHECKTASK_TASK_PRI,
                   CHECKTASK_TASK_PRI, &CHECKTASK_TASK_STK[0], CHECKTASK_STK_SIZE, NULL, OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK);
#endif
    

#if 0
    SHT20_INFO sht20;
    ADXL345_INFO adxl345;

    Hardware_Init();											//Ӳ����ʼ��
    Led4_Set(LED_ON);

    while (1)
    {
        SHT20_GetValue(&sht20);
        UsartPrintf(USART1, "temp = %0.1f, humi = %0.1f\r\n", sht20.tempreture, sht20.humidity);

        ADXL345_GetValue(&adxl345);
        UsartPrintf(USART1, "x = %0.1f, y = %0.1f, z = %0.1f\r\n",
                    adxl345.incidence_Xf, adxl345.incidence_Yf, adxl345.incidence_Zf);

        DelayXms(2000);
    }
#endif

}

