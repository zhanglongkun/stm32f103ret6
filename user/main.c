//单片机头文件
#include "stm32f10x.h"

//硬件驱动
#include "delay.h"
#include "led.h"
#include "sht20.h"
#include "usart.h"
#include "i2c.h"


void Hardware_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//中断控制器分组设置

    Delay_Init();										//Systick初始化，用于普通的延时

    Led_Init();											//LED接口初始化
    Usart1_Init(115200);
    
    IIC_Init();
}


int main(void)
{
    SHT20_INFO sht20;
    
    Hardware_Init();											//硬件初始化
    
    Led4_Set(LED_ON);
    
    SHT20_GetValue(&sht20);
    UsartPrintf(USART1, "temp = %0.1f, humi = %0.1f\r\n", sht20.tempreture, sht20.humidity);

    DelayXms(2000);
    
}

