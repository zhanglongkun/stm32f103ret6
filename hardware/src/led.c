/**
  ******************************************************************************
  * @FileName:     led.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-22 10:58:40
  * @Description:  This file provides all the led.c functions. 
  ******************************************************************************
  */ 


//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//LEDͷ�ļ�
#include "led.h"
#include "delay.h"


LED_STATUS ledStatus;

/**
  ******************************************************************************
  * Function:     Led_Init()
  * Description:  ��ʼ�� LED
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void Led_Init(void)
{
    GPIO_InitTypeDef gpioInitStrcut;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);	//��GPIOA��GPIOC��ʱ��

    gpioInitStrcut.GPIO_Mode = GPIO_Mode_Out_PP;									//����Ϊ�������ģʽ
    gpioInitStrcut.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 |GPIO_Pin_10;					//��ʼ��Pin7��8��10
    gpioInitStrcut.GPIO_Speed = GPIO_Speed_50MHz;									//���ص����Ƶ��
    GPIO_Init(GPIOC, &gpioInitStrcut);												//��ʼ��GPIOC

    gpioInitStrcut.GPIO_Pin = GPIO_Pin_12;											//��ʼ��Pin12
    GPIO_Init(GPIOA, &gpioInitStrcut);												//��ʼ��GPIOA

    Led4_Set(LED_OFF);
    Led5_Set(LED_OFF);
    Led6_Set(LED_OFF);
    Led7_Set(LED_OFF);
}


void Led4_Set(LED_ENUM status)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_7, status != LED_ON ? Bit_SET : Bit_RESET);	//status���������LED_ON�򷵻�Bit_SET�����򷵻�Bit_RESET����ͬ
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
  * Description:  ��ˮ�ƣ�ѭ��1000��
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
        Led4_Set(LED_OFF);      //����LED4������ʱ500ms��Ȼ��Ϩ��LED4
        
        Led5_Set(LED_ON);
        DelayXms(500);
        Led5_Set(LED_OFF);      //����LED5������ʱ500ms��Ȼ��Ϩ��LED5
        
        Led6_Set(LED_ON);
        DelayXms(500);
        Led6_Set(LED_OFF);      //����LED6������ʱ500ms��Ȼ��Ϩ��LED6
        
        Led7_Set(LED_ON);
        DelayXms(500);
        Led7_Set(LED_OFF);      //����LED7������ʱ500ms��Ȼ��Ϩ��LED7
    }
}
