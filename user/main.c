//单片机头文件
#include "stm32f10x.h"

//硬件驱动
#include "delay.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "exti.h"


void Hardware_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//中断控制器分组设置

    Delay_Init();										//Systick初始化，用于普通的延时

    Led_Init();											//LED接口初始化

    Key_Init();
    
    Exti_PC11_Config();
}


int main(void)
{
    Hardware_Init();											//硬件初始化

    WaterLights();
}

