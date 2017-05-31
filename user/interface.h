/**
  ******************************************************************************
  * @@FileName£º   interface.h 
  * @author£º      zlk
  * @version£º     V1.0
  * @Date:         2017-5-31 9:49:43
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef _INTERFACE_H_
#define _INTERFACE_H_


typedef struct _ST_VAR_PELCO_D
{
    unsigned char syncByte;
    unsigned char addressCode;
    unsigned short instructionCode;
    unsigned char dataCode1;
    unsigned char dataCode2;
    unsigned char checkCode;
}ST_VAR_PELCO_D;


#endif  /* _INTERFACE_H_ */


