/**
  ******************************************************************************
  * @@FileName：   key.h 
  * @author：      zlk
  * @version：     V1.0
  * @Date:         2017-5-20 9:45:06
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef _KEY_H_
#define _KEY_H_

#define KEY0			GPIO_Pin_11
#define KEY1			GPIO_Pin_13
#define KEY2			GPIO_Pin_12
#define KEY3			GPIO_Pin_2


/*******************************************
			按键按下与弹起
*******************************************/
#define KEYDOWN			1
#define KEYUP			0

#define KEY0DOWN		0

#define KEY1DOWN		3

#define KEY2DOWN		6

#define KEY3DOWN		9

#define KEYNONE			255



void Key_Init(void);

_Bool KeyScan(GPIO_TypeDef* GPIOX, unsigned int NUM);

unsigned char Keyboard(void);


#endif  /* _KEY_H_ */


