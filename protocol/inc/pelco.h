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
#include <stdio.h>

#include "usart.h"
#include "led.h"


typedef struct _ST_VAR_PELCO_D
{
    unsigned char syncByte;
    unsigned char addressCode;
    unsigned short instructionCode;
    unsigned char dataCode1;
    unsigned char dataCode2;
    unsigned char checkCode;
} ST_VAR_PELCO_D;


void Manager(ST_VAR_PELCO_D pelco);

void Led_Set(unsigned char ledNum, unsigned char status);

int Conversion_Handle(USART_IO_INFO usartRev);


#endif  /* _MANAGER_H_ */


