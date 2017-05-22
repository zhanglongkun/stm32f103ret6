/**
  ******************************************************************************
  * @FileName:     beep.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-22 11:45:45
  * @Description:  This file provides all the beep.c functions. 
  ******************************************************************************
  */ 



//单片机头文件
#include "stm32f10x.h"

//硬件驱动
#include "beep.h"


BEEP_INFO beepInfo = {0};


/**
  ******************************************************************************
  * Function:     Beep_Init()
  * Description:  初始化 beep
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void Beep_Init(void)
{
    GPIO_InitTypeDef gpioInitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//打开GPIOA的时钟

    gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;				//设置为输出
    gpioInitStruct.GPIO_Pin = GPIO_Pin_4;						//将初始化的Pin脚
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;				//可承载的最大频率

    GPIO_Init(GPIOA, &gpioInitStruct);							//初始化GPIO

    Beep_Set(BEEP_OFF);											//初始化完成后，关闭蜂鸣器
}

/**
  ******************************************************************************
  * Function:     Beep_Set()
  * Description:  蜂鸣器控制
  * Parameter:    status --开关
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void Beep_Set(_Bool status)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, status == BEEP_ON ? Bit_SET : Bit_RESET);		//如果status等于BEEP_ON，则返回Bit_SET，否则返回Bit_RESET

    beepInfo.Beep_Status = status;
}

