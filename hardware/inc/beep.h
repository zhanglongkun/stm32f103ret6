/**
  ******************************************************************************
  * @@FileName£º   beep.h 
  * @author£º      zlk
  * @version£º     V1.0
  * @Date:         2017-5-19 ÏÂÎç 11:07:36
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef _BEEP_H_
#define _BEEP_H_

typedef struct
{
    _Bool Beep_Status;
} BEEP_INFO;

#define BEEP_ON		1
#define BEEP_OFF	0

extern BEEP_INFO beepInfo;

void Beep_Init(void);

void Beep_Set(_Bool status);



#endif  /* _BEEP_H_ */

