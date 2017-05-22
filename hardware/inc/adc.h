/**
  ******************************************************************************
  * @@FileName£º   adc.h 
  * @author£º      zlk
  * @version£º     V1.0
  * @Date:         2017-5-22 14:17:05
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef _ADC_H_
#define _ADC_H_

#include "stm32f10x.h"


void ADC_ChInit(ADC_TypeDef * ADCx, _Bool temp);

unsigned short ADC_GetValue(ADC_TypeDef * ADCx, unsigned char ch);

float ADC_GetValueTimes(ADC_TypeDef * ADCx, unsigned char ch, unsigned char times);

float ADC_GetTemperature(void);

#endif  /* _ADC_H_ */

