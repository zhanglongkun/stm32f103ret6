/**
  ******************************************************************************
  * @FileName:     led.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-22 10:58:40
  * @Description:  This file provides all the led.c functions. 
  ******************************************************************************
  */ 


//单片机头文件
#include "stm32f10x.h"

//LED头文件
#include "led.h"
#include "delay.h"


LED_STATUS ledStatus;

/**
  ******************************************************************************
  * Function:     Led_Init()
  * Description:  初始化 LED
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void Led_Init(void)
{
    GPIO_InitTypeDef gpioInitStrcut;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);	//打开GPIOA和GPIOC的时钟

    gpioInitStrcut.GPIO_Mode = GPIO_Mode_Out_PP;									//设置为推挽输出模式
    gpioInitStrcut.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 |GPIO_Pin_10;					//初始化Pin7、8、10
    gpioInitStrcut.GPIO_Speed = GPIO_Speed_50MHz;									//承载的最大频率
    GPIO_Init(GPIOC, &gpioInitStrcut);												//初始化GPIOC

    gpioInitStrcut.GPIO_Pin = GPIO_Pin_12;											//初始化Pin12
    GPIO_Init(GPIOA, &gpioInitStrcut);												//初始化GPIOA

    Led4_Set(LED_OFF);
    Led5_Set(LED_OFF);
    Led6_Set(LED_OFF);
    Led7_Set(LED_OFF);
}


void Led4_Set(LED_ENUM status)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_7, status != LED_ON ? Bit_SET : Bit_RESET);	//status如果不等于LED_ON则返回Bit_SET，否则返回Bit_RESET。下同
    ledStatus.Led4Sta = status;
}


void Led5_Set(LED_ENUM status)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_8, status != LED_ON ? Bit_SET : Bit_RESET);
    ledStatus.Led5Sta = status;
}


void Led6_Set(LED_ENUM status)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_12, status != LED_ON ? Bit_SET : Bit_RESET);
    ledStatus.Led6Sta = status;
}


void Led7_Set(LED_ENUM status)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_10, status != LED_ON ? Bit_SET : Bit_RESET);
    ledStatus.Led7Sta = status;
}


/**
  ******************************************************************************
  * Function:     WaterLights()
  * Description:  流水灯，循环1000次
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void WaterLights(void)
{
    int i;
    for (i = 0; i < 1000; i++) {
        
        UsartPrintf(USART1, "led\r\n");
        Led4_Set(LED_ON);
        DelayXms(500);
        Led4_Set(LED_OFF);      //点亮LED4，并延时500ms，然后熄灭LED4
        
        Led5_Set(LED_ON);
        DelayXms(500);
        Led5_Set(LED_OFF);      //点亮LED5，并延时500ms，然后熄灭LED5
        
        Led6_Set(LED_ON);
        DelayXms(500);
        Led6_Set(LED_OFF);      //点亮LED6，并延时500ms，然后熄灭LED6
        
        Led7_Set(LED_ON);
        DelayXms(500);
        Led7_Set(LED_OFF);      //点亮LED7，并延时500ms，然后熄灭LED7
    }
}
