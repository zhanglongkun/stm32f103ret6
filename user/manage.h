/**
  ******************************************************************************
  * @@FileName£º   manager.h 
  * @author£º      zlk
  * @version£º     V1.0
  * @Date:         2017-5-31 16:24:04
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef _MANAGER_H_
#define _MANAGER_H_

#include <string.h>

#include "interface.h"
#include "manage.h"
#include "usart.h"
#include "led.h"


void Manager(ST_VAR_PELCO_D pelco);

void Led_Set(unsigned char ledNum, unsigned char status);

int Conversion_Handle(USART_IO_INFO usartRev);


#endif  /* _MANAGER_H_ */


