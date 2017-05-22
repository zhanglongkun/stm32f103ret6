/**
  ******************************************************************************
  * @FileName:     tcrt5000.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-22 12:01:15
  * @Description:  This file provides all the tcrt5000.c functions.
  ******************************************************************************
  */


//单片机头文件
#include "stm32f10x.h"

//硬件驱动
#include "adc.h"
#include "tcrt5000.h"


TCRT5000_INFO t5000Info;

/**
  ******************************************************************************
  * Function:     TCRT5000_Init()
  * Description:  初始化控制脚和ADC1_ch13
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void TCRT5000_Init(void)
{
    GPIO_InitTypeDef gpioInitStrcut;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

    gpioInitStrcut.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInitStrcut.GPIO_Pin = GPIO_Pin_5;
    gpioInitStrcut.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStrcut);

    gpioInitStrcut.GPIO_Mode = GPIO_Mode_AIN;
    gpioInitStrcut.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOC, &gpioInitStrcut);

    ADC_ChInit(ADC1, 0);

    TCRT5000_Set(TCRT5000_ON);
}


/**
  ******************************************************************************
  * Function:     TCRT5000_GetValue()
  * Description:  获取红外接收管的AD值
  * Parameter:    times：采集次数，平均值
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void TCRT5000_GetValue(unsigned char times)
{
    t5000Info.voltag = ADC_GetValueTimes(ADC1, ADC_Channel_13, times);
}


/**
  ******************************************************************************
  * Function:     TCRT5000_Set()
  * Description:  控制脚
  * Parameter:    status：TCRT5000_ON-打开距离检测		TCRT5000_OFF：关闭距离检测
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void TCRT5000_Set(TCRT5000_ENUM status)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, status != TCRT5000_ON ? Bit_SET : Bit_RESET);

    t5000Info.status = status;
}
