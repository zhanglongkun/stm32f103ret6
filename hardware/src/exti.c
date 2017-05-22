/**
  ******************************************************************************
  * @FileName:     exti.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-20 11:31:01
  * @Description:  This file provides all the exti.c functions.
  ******************************************************************************
  */

#include "stm32f10x.h"

/**
  ******************************************************************************
  * Function:     Exti_PC11_Config()
  * Description:  初始化 GPIOC 的 GPIO_Pin_11
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void Exti_PC11_Config(void)
{
    GPIO_InitTypeDef gpioInit;
    EXTI_InitTypeDef extiInit;
    NVIC_InitTypeDef nvicInit;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

    gpioInit.GPIO_Pin = GPIO_Pin_11;
    gpioInit.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &gpioInit);

    nvicInit.NVIC_IRQChannel = EXTI9_5_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource11);

    extiInit.EXTI_Line = EXTI_Line11;
    extiInit.EXTI_Mode = EXTI_Mode_Interrupt;
    extiInit.EXTI_Trigger = EXTI_Trigger_Falling;  //下降沿中断
    extiInit.EXTI_LineCmd = ENABLE;

    EXTI_Init(&extiInit);
}


/**
  ******************************************************************************
  * Function:     EXTI9_5_IRQHandler()
  * Description:  9 - 15 号中断
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void EXTI9_5_IRQHandler(void)
{
    //确保是否产生了 EXTI Line中断
    if(EXTI_GetITStatus(EXTI_Line11) != RESET) {
        // LED6 取反
        GPIO_WriteBit(GPIOA, GPIO_Pin_12, (BitAction)((1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_3))));
        
        EXTI_ClearITPendingBit(EXTI_Line5); //清除中断标志位
    }

}

