/**
  ******************************************************************************
  * @@FileName£º   tcrt5000.h
  * @author£º      zlk
  * @version£º     V1.0
  * @Date:         2017-5-22 12:00:37
  * @Description   This file contains all the functions prototypes for the BOARD
  ******************************************************************************
  */


#ifndef _TCRT5000_H_
#define _TCRT5000_H_


typedef struct
{
    float voltag;
    _Bool status;
} TCRT5000_INFO;

extern TCRT5000_INFO t5000Info;

typedef enum
{
    TCRT5000_OFF = 0,
    TCRT5000_ON
} TCRT5000_ENUM;


void TCRT5000_Init(void);

void TCRT5000_GetValue(unsigned char times);

void TCRT5000_Set(TCRT5000_ENUM status);

#endif  /* _TCRT5000_H_ */

