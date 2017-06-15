//单片机头文件
#include "stm32f10x.h"

//硬件驱动
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

#include "user.h"

USART_IO_INFO usart1IOInfo = {0};
USART_IO_INFO usart2IOInfo = {0};


//任务堆栈计算
#define CHECKTASK_TASK_PRI    8
#define CHECKTASK_STK_SIZE    64
OS_STK CHECKTASK_TASK_STK[CHECKTASK_STK_SIZE];


//网络初始化任务
#define NET_TASK_PRIO		 9 
#define NET_STK_SIZE		 256
OS_STK NET_TASK_STK[NET_STK_SIZE];


//心跳任务
#define HEAD_TASK_PRIO		 10 
#define HEAD_STK_SIZE		 256
OS_STK HEAD_TASK_STK[HEAD_STK_SIZE];



static void Check_Stack_Task()
{
    unsigned int ret;
    OS_STK_DATA pelcoData;

    while (1) {
        ret = OSTaskStkChk(CHECKTASK_TASK_PRI, &pelcoData);

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
    while (1) {
        Heart_Data_Send();
    }
}

void Hardware_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//中断控制器分组设置

    Delay_Init();										//Systick初始化，用于普通的延时

    Led_Init();											//LED接口初始化

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
    Hardware_Init();											//硬件初始化

    GSM_Device_Init();

    
    OSTaskCreateExt(Net_Task, (void *)0, &NET_TASK_STK[NET_STK_SIZE - 1], CHECKTASK_TASK_PRI,
                    CHECKTASK_TASK_PRI, &NET_TASK_STK[0], NET_STK_SIZE, NULL, OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK);


    
#if 0
   OSTaskCreateExt(Check_Stack_Task, (void *)0, &CHECKTASK_TASK_STK[CHECKTASK_STK_SIZE - 1], CHECKTASK_TASK_PRI,
                   CHECKTASK_TASK_PRI, &CHECKTASK_TASK_STK[0], CHECKTASK_STK_SIZE, NULL, OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK);
#endif
    

#if 0
    SHT20_INFO sht20;
    ADXL345_INFO adxl345;

    Hardware_Init();											//硬件初始化
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

