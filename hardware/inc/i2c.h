/**
  ******************************************************************************
  * @@FileName£º   i2c.h 
  * @author£º      zlk
  * @version£º     V1.0
  * @Date:         2017-5-20 17:19:02
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef _I2C_H_
#define _I2C_H_


typedef struct
{
    unsigned short speed;
} IIC_INFO;

extern IIC_INFO iicInfo;

void IIC_Init(void);

void IIC_Start(void);

void IIC_Stop(void);


#endif  /* _I2C_H_ */


