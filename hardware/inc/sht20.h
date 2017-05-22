/**
  ******************************************************************************
  * @@FileName：   sht20.h 
  * @author：      zlk
  * @version：     V1.0
  * @Date:         2017-5-21 下午 1:33:22
  * @Description   This file contains all the functions prototypes for the BOARD 
  ******************************************************************************
  */ 


#ifndef _SHT20_H_
#define _SHT20_H_


/*SHT20 设备操作相关宏定义，详见手册*/
#define SHT20_ADDRESS  0X40
#define SHT20_Measurement_RH_HM  0XE5
#define SHT20_Measurement_T_HM  0XE3
#define SHT20_Measurement_RH_NHM  0XF5
#define SHT20_Measurement_T_NHM  0XF3
#define SHT20_READ_REG  0XE7
#define SHT20_WRITE_REG  0XE6
#define SHT20_SOFT_RESET  0XFE

typedef struct
{
    float tempreture;
    float humidity;
} SHT20_INFO;


void SHT20_GetValue(SHT20_INFO *info);


#endif  /* _SHT20_H_ */


