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
  * Function:     Exti_Key_Init()
  * Description:  ��ʼ�� key �ж�
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void Exti_Key_Init(void)
{
    GPIO_InitTypeDef gpioInit;
    EXTI_InitTypeDef extiInit;
    NVIC_InitTypeDef nvicInit;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

    gpioInit.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
    gpioInit.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &gpioInit);

    gpioInit.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &gpioInit);

    nvicInit.NVIC_IRQChannel = EXTI15_10_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);
    
    nvicInit.NVIC_IRQChannel = EXTI2_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource11);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource12);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2);

    extiInit.EXTI_Line = EXTI_Line11 | EXTI_Line12 | EXTI_Line13 | EXTI_Line2;
    extiInit.EXTI_Mode = EXTI_Mode_Interrupt;
    extiInit.EXTI_Trigger = EXTI_Trigger_Falling;  //�½����ж�
    extiInit.EXTI_LineCmd = ENABLE;

    EXTI_Init(&extiInit);
}


/**
  ******************************************************************************
  * Function:     EXTI15_10_IRQHandler()
  * Description:  key2��key3��key5 �жϴ����� (10 - 15 ���ж�)
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void EXTI15_10_IRQHandler(void)
{
    //ȷ���Ƿ������ EXTI Line�ж�
    if(EXTI_GetITStatus(EXTI_Line11) != RESET) {
        // LED6 ȡ��
        GPIO_WriteBit(GPIOC, GPIO_Pin_7, (BitAction)((1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_7))));
        
        EXTI_ClearITPendingBit(EXTI_Line11); //����жϱ�־λ
    }
    
    if(EXTI_GetITStatus(EXTI_Line12) != RESET) {
        // LED6 ȡ��
        GPIO_WriteBit(GPIOC, GPIO_Pin_7, (BitAction)((1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_7))));
        
        EXTI_ClearITPendingBit(EXTI_Line12); //����жϱ�־λ
    }
    
    if(EXTI_GetITStatus(EXTI_Line13) != RESET) {
        // LED6 ȡ��
        GPIO_WriteBit(GPIOC, GPIO_Pin_7, (BitAction)((1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_7))));
        
        EXTI_ClearITPendingBit(EXTI_Line13); //����жϱ�־λ
    }
}

/**
  ******************************************************************************
  * Function:     EXTI2_IRQHandler()
  * Description:  key4 �жϴ�����
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-05-23
  ******************************************************************************
  */ 
void EXTI2_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line2) != RESET) {
        // LED6 ȡ��
        GPIO_WriteBit(GPIOC, GPIO_Pin_7, (BitAction)((1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_7))));
        
        EXTI_ClearITPendingBit(EXTI_Line2); //����жϱ�־λ
    }
}
