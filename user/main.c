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

//os
#include "includes.h"


#define TASK_PRI 5

USART_IO_INFO usart1IOInfo = {0};
USART_IO_INFO usart2IOInfo = {0};


static void Check_Stack_Task()
{
    unsigned int ret;
    OS_STK_DATA pelcoData;

    while (1) {
        ret = OSTaskStkChk(TASK_PRI, &pelcoData);

        UsartPrintf(USART1, "ret = %d, free = %d, used = %d\r\n", ret, pelcoData.OSFree, pelcoData.OSUsed);
        
        OSTimeDly(10);
    }
}


void Hardware_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//�жϿ�������������

    Delay_Init();										//Systick��ʼ����������ͨ����ʱ

    Led_Init();											//LED�ӿڳ�ʼ��

    Key_Init();

    Exti_Key_Init();

    Usart1_Init(115200);
    
    GSM_Init();

    IIC_Init();

    RTOS_TimerInit();

    ADXL345_Init();
}


int main(void)
{
    Hardware_Init();											//Ӳ����ʼ��

    GSM_Device_Init();

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

