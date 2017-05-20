/**
  ******************************************************************************
  * @FileName:     key.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-20 9:45:10
  * @Description:  This file provides all the key.c functions. 
  ******************************************************************************
  */ 

#include "stm32f10x.h"
#include "key.h"


void Key_Init(void)
{
    GPIO_InitTypeDef gpioInit;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

    gpioInit.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
    gpioInit.GPIO_Mode = GPIO_Mode_IPU;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpioInit);

    gpioInit.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &gpioInit);
}


_Bool KeyScan(GPIO_TypeDef* GPIOX, unsigned int NUM)
{
    if (GPIOC == GPIOX) {
        if (!GPIO_ReadInputDataBit(GPIOC, NUM)) {
            return KEYDOWN;
        } else {
            return KEYUP;
        }
    } else if (GPIOD == GPIOX) {
        if (!GPIO_ReadInputDataBit(GPIOD, NUM)) {
            return KEYDOWN;
        } else {
            return KEYUP;
        }
    }

    return KEYUP;
}

unsigned char Keyboard(void)
{
    if(KeyScan(GPIOC, KEY0) == KEYDOWN)
    {
        while(KeyScan(GPIOC, KEY0) != KEYUP);		
        return KEY0DOWN;
    }

    if(KeyScan(GPIOC, KEY1) == KEYDOWN)
    {
        while(KeyScan(GPIOC, KEY1) != KEYUP);		
        return KEY1DOWN;
    }

    if(KeyScan(GPIOC, KEY2) == KEYDOWN)
    {
        while(KeyScan(GPIOC, KEY2) != KEYUP);		
        return KEY2DOWN;
    }

    if(KeyScan(GPIOD, KEY3) == KEYDOWN)
    {
        while(KeyScan(GPIOD, KEY3) != KEYUP);	
        return KEY3DOWN;
    }

    return KEYNONE;
}



