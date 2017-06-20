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
#include "hwtimer.h"

//OS
#include "includes.h"


#define DBG_MSG(fmt, args...) \
    do {\
        UsartPrintf(USART1, "%s-->%s(%d)-->", __FILE__, __func__, __LINE__); \
        UsartPrintf(USART1, fmt, ##args);\
        UsartPrintf(USART1, "\r\n");\
    } while (0)


USART_IO_INFO usart1IOInfo;
USART_IO_INFO usart2IOInfo;


OS_EVENT *commMbox;


//邮箱发送
#define MBOX_SEND_PRIO          8
#define MBOX_SEND_TASK_SIZE     512
OS_STK MBOX_SEND_TASK_STK[MBOX_SEND_TASK_SIZE];

//邮箱接收
#define MBOX_REV_PRIO           9
#define MBOX_REV_TASK_SIZE      512
OS_STK MBOX_REV_TASK_STK[MBOX_REV_TASK_SIZE];



void Send_Task()
{
    int err;
    
    while (1) {
       err = OSMboxPost(commMbox, "aaaaaaaaaaaaa");

       OSTimeDly(2000);
    }
}


void Receive_Task()
{
    void *msg;
    
    while (1) {
        msg = OSMboxAccept(commMbox);
        if (NULL != msg) {
            DBG_MSG("msg = %s", msg);
        }
        
        OSTimeDly(1000);
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
    
    IIC_Init();

    ADXL345_Init();

    RTOS_TimerInit();
}


int main(void)
{
    SHT20_INFO sht20;
    ADXL345_INFO adxl345;
    
    Hardware_Init();											//硬件初始化

    OSInit();

    commMbox = OSMboxCreate((void *) 0);

    OSTaskCreate(Send_Task, (void *)0, &MBOX_SEND_TASK_STK[MBOX_SEND_TASK_SIZE - 1], MBOX_SEND_PRIO);
    OSTaskCreate(Receive_Task, (void *)0, &MBOX_REV_TASK_STK[MBOX_REV_TASK_SIZE - 1], MBOX_REV_PRIO);



    OSStart();
    
#if 0
    Led4_Set(LED_ON);
    
    SHT20_GetValue(&sht20);
    UsartPrintf(USART1, "temp = %0.1f, humi = %0.1f\r\n", sht20.tempreture, sht20.humidity);

    ADXL345_GetValue(&adxl345);
    UsartPrintf(USART1, "x = %0.1f, y = %0.1f, z = %0.1f\r\n",
                adxl345.incidence_Xf, adxl345.incidence_Yf, adxl345.incidence_Zf);

    DelayXms(2000);
#endif
    
}

