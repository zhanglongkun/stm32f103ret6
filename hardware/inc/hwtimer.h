/**
  ******************************************************************************
  * @@FileName£º   hwtimer.h 
  * @author£º      zlk
  * @version£º     V1.0
  * @Date:         2017-6-2 11:57:38
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef _HWTIMER_H_
#define _HWTIMER_H_



#include "stm32f10x.h"


typedef struct
{
    unsigned char timer6Out : 2;
    unsigned char reverse : 6;
} TIM_INFO;

extern TIM_INFO timInfo;

#define OS_TIMER	TIM6


void Timer1_8_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc);

void Timer6_7_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc);

void RTOS_TimerInit(void);

#endif  /* _HWTIMER_H_ */

