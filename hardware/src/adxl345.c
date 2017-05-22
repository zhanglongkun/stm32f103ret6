/**
  ******************************************************************************
  * @FileName:     adxl345.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-22 10:43:40
  * @Description:  This file provides all the adxl345.c functions. 
  ******************************************************************************
  */ 


#include <string.h>

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//Ӳ������
#include "adxl345.h"
#include "i2c.h"
#include "delay.h"


/**
  ******************************************************************************
  * Function:     ADXL345_Init()
  * Description:  ��ʼ�� ADXL345
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void ADXL345_Init(void)
{
    unsigned char devid = 0, val = 0;

    DelayUs(300);

    I2C_ReadByte(ADXL345_ADDRESS, 0x00, &devid);			//��ID	��ÿ�ζ�д֮ǰ����Ҫ��ID
    DelayUs(300);

    val = 0x2B;
    I2C_WriteByte(ADXL345_ADDRESS, DATA_FORMAT_REG, &val);	//�͵�ƽ�ж����,13λȫ�ֱ���,��������Ҷ���,16g����
    DelayUs(50);

    val = 0x0A;
    I2C_WriteByte(ADXL345_ADDRESS, BW_RATE, &val);			//��������ٶ�Ϊ100Hz
    DelayUs(50);

    val = 0x28;
    I2C_WriteByte(ADXL345_ADDRESS, POWER_CTL, &val);		//����ʹ��,����ģʽ
    DelayUs(50);

    val = 0;
    I2C_WriteByte(ADXL345_ADDRESS, INT_ENABLE, &val);		//��ʹ���ж�
    DelayUs(50);

    I2C_WriteByte(ADXL345_ADDRESS, OFSX, &val);				//X��ƫ��0
    DelayUs(50);

    I2C_WriteByte(ADXL345_ADDRESS, OFSY, &val);				//Y��ƫ��0
    DelayUs(50);

    I2C_WriteByte(ADXL345_ADDRESS, OFSZ, &val);				//Z��ƫ��0

    DelayUs(500);
}


/**
  ******************************************************************************
  * Function:     ADXL345_GetValue()
  * Description:  ��ȡ������ٶ�ֵ
  * Parameter:    info --ADXL345�ṹ��
  * Return:       -1 --��ȡʧ�ܣ�0 --��ȡ�ɹ�
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
int ADXL345_GetValue(ADXL345_INFO *info)
{
    int ret;
    unsigned char devid = 0;
    unsigned char dataTemp[6];
    ADXL345_INFO adxlInfo;

    IIC_SpeedCtl(5);													//����IIC�ٶ�

    DelayUs(200);
    ret = I2C_ReadByte(ADXL345_ADDRESS, 0x00, &devid);						//��ID	��ÿ�ζ�д֮ǰ����Ҫ��ID
    if (0 != ret) {
        return -1;
    }
    DelayUs(200);

    ret = I2C_ReadBytes(ADXL345_ADDRESS, 0x32, dataTemp, 6);					//��ȡԭʼ����ֵ(4mg/LSB)
    if (0 != ret) {
        return -1;
    }

    adxlInfo.incidence_X = (short)(dataTemp[0] + ((unsigned short)dataTemp[1] << 8));
    adxlInfo.incidence_Y = (short)(dataTemp[2] + ((unsigned short)dataTemp[3] << 8));
    adxlInfo.incidence_Z = (short)(dataTemp[4] + ((unsigned short)dataTemp[5] << 8));

    adxlInfo.incidence_Xf = (float)adxlInfo.incidence_X * 0.0039;		//����Ϊg
    adxlInfo.incidence_Yf = (float)adxlInfo.incidence_Y * 0.0039;		//ÿһ��LSB����3.9mg
    adxlInfo.incidence_Zf = (float)adxlInfo.incidence_Z * 0.0039;		//�ж��ٸ�LSB���ͳ���0.0039g�͵õ�����gΪ��λ�ļ���ֵ

    memcpy(info, &adxlInfo, sizeof(ADXL345_INFO));

    return 0;
}
