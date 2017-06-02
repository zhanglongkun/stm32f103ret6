/**
  ******************************************************************************
  * @FileName:     hwtimer.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-6-2 11:53:22
  * @Description:  This file provides all the hwtimer.c functions. 
  ******************************************************************************
  */ 



//OSͷ�ļ�
#include "includes.h"

//Ӳ������
#include "hwtimer.h"


TIM_INFO timInfo = {0, 0};


/**
  ******************************************************************************
  * Function:     Timer1_8_Init()
  * Description:  Timer1��8��PWM����
  * Parameter:    TIMx��TIM1 ���� TIM8
                  arr������ֵ
                  psc: ��Ƶֵ
  * Return:       void
  * Others:       add by zlk, 2017-06-02
  ******************************************************************************
  */ 
void Timer1_8_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc)
{
    GPIO_InitTypeDef gpioInitStruct;
    TIM_TimeBaseInitTypeDef timerInitStruct;
    TIM_OCInitTypeDef timerOCInitStruct;

    if(TIMx == TIM1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    }
    else
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
    }

    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpioInitStruct);

    timerInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStruct.TIM_Period = arr;
    timerInitStruct.TIM_Prescaler = psc;
    TIM_TimeBaseInit(TIMx, &timerInitStruct);

    timerOCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;				//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    timerOCInitStruct.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    timerOCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;		//�������:TIM����Ƚϼ��Ե�
    timerOCInitStruct.TIM_Pulse = 0;
    TIM_OC2Init(TIMx, &timerOCInitStruct);
    TIM_OC3Init(TIMx, &timerOCInitStruct);

    TIM_CtrlPWMOutputs(TIMx, ENABLE);							//MOE �����ʹ��

    TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);			//ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
    TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);			//ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���

    TIM_ARRPreloadConfig(TIMx, ENABLE);							//ARPEʹ��

    TIM_Cmd(TIMx, ENABLE);										//ʹ��TIMx
}


/**
  ******************************************************************************
  * Function:     Timer6_7_Init()
  * Description:  Timer6��7�Ķ�ʱ����
  * Parameter:    TIMx��TIM6 ���� TIM7
                  arr������ֵ
                  psc��Ƶֵ
  * Return:       void
  * Others:       add by zlk, 2017-06-02
  ******************************************************************************
  */ 
void Timer6_7_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc)
{
    TIM_TimeBaseInitTypeDef timerInitStruct;
    NVIC_InitTypeDef nvicInitStruct;

    if(TIMx == TIM6)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

        nvicInitStruct.NVIC_IRQChannel = TIM6_IRQn;
    }
    else
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

        nvicInitStruct.NVIC_IRQChannel = TIM7_IRQn;
    }

    timerInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStruct.TIM_Period = arr;
    timerInitStruct.TIM_Prescaler = psc;

    TIM_TimeBaseInit(TIMx, &timerInitStruct);

    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);					//ʹ�ܸ����ж�

    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    nvicInitStruct.NVIC_IRQChannelSubPriority = 1;

    NVIC_Init(&nvicInitStruct);

    TIM_Cmd(TIMx, ENABLE); //ʹ�ܶ�ʱ��
}


/**
  ******************************************************************************
  * Function:     RTOS_TimerInit()
  * Description:  RTOS��������ʱ��ʼ��
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-02
  ******************************************************************************
  */ 
void RTOS_TimerInit(void)
{
    Timer6_7_Init(TIM6, 10000 / OS_TICKS_PER_SEC, 7199);	//72MHz��7200��Ƶ-100us��50����ֵ�����ж�����Ϊ100us * 50 = 5ms
}


/**
  ******************************************************************************
  * Function:     TIM6_IRQHandler()
  * Description:  RTOS��������ʱ�ж�
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-02
  ******************************************************************************
  */ 
void TIM6_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM6, TIM_IT_Update) == SET)
    {
        //do something...
        OSIntEnter();
        OSTimeTick();
        OSIntExit();

        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}


/**
  ******************************************************************************
  * Function:     TIM7_IRQHandler()
  * Description:  Timer7�����жϷ�����
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-02
  ******************************************************************************
  */ 
void TIM7_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM7, TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    }
}
