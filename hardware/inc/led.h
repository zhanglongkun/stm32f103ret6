/**
  ******************************************************************************
  * @@FileName£º   led.h 
  * @author£º      zlk
  * @version£º     V1.0
  * @Date:         2017-5-22 10:57:07
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef _LED_H_
#define _LED_H_


typedef struct
{
    _Bool Led4Sta;
    _Bool Led5Sta;
    _Bool Led6Sta;
    _Bool Led7Sta;
} LED_STATUS;

extern LED_STATUS ledStatus;

typedef enum
{
    LED_OFF = 0,
    LED_ON
} LED_ENUM;


void Led_Init(void);

void Led4_Set(LED_ENUM status);

void Led5_Set(LED_ENUM status);

void Led6_Set(LED_ENUM status);

void Led7_Set(LED_ENUM status);

void WaterLights(void);

#endif  /* _LED_H_ */

