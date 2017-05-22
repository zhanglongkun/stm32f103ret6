/**
  ******************************************************************************
  * @FileName:     sht20.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-21 ���� 1:33:33
  * @Description:  This file provides all the sht20.c functions. 
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "sht20.h"
#include "delay.h"
#include "i2c.h"
#include <string.h>
#include "usart.h"

const int16_t POLYNOMIAL = 0x131;



/**
  ******************************************************************************
  * Function:     SHT20_Reset()
  * Description:  ��λ
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void SHT20_Reset(void)
{
    I2C_WriteByte(SHT20_ADDRESS, SHT20_SOFT_RESET, (void *)0);
}

/**
  ******************************************************************************
  * Function:     SHT20_read_user_reg()
  * Description:  SHT20��ȡ�û��Ĵ���
  * Parameter:    void
  * Return:       ��ȡ�����û��Ĵ�����ֵ
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
unsigned char  SHT20_read_user_reg(void)
{
    unsigned char val = 0;

    I2C_ReadByte(SHT20_ADDRESS, SHT20_READ_REG, &val);

    return val;
}

/**
  ******************************************************************************
  * Function:     SHT2x_CheckCrc()
  * Description:  ���������ȷ��
  * Parameter:    data����ȡ��������
                  nbrOfBytes����ҪУ�������
                  checksum����ȡ����У�Ա���ֵ
  * Return:       0 --�ɹ���1 --ʧ��
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
char SHT2x_CheckCrc(char data[], char nbrOfBytes, char checksum)
{
    char crc = 0;
    char bit = 0;
    char byteCtr = 0;

    //calculates 8-Bit checksum with given polynomial
    for(byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr) {
        crc ^= (data[byteCtr]);
        for ( bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
            else crc = (crc << 1);
        }
    }

    if(crc != checksum)
        return 1;
    else
        return 0;
}

/**
  ******************************************************************************
  * Function:     SHT2x_CalcTemperatureC()
  * Description:  �¶ȼ���
  * Parameter:    u16sT����ȡ�����¶�ԭʼ����
  * Return:       �������¶�����
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
float SHT2x_CalcTemperatureC(unsigned short u16sT)
{
    float temperatureC = 0;            // variable for result

    u16sT &= ~0x0003;           // clear bits [1..0] (status bits)

    temperatureC = -46.85 + 175.72 / 65536 * (float)u16sT; //T= -46.85 + 175.72 * ST/2^16

    return temperatureC;
}

/**
  ******************************************************************************
  * Function:     SHT2x_CalcRH()
  * Description:  ʪ�ȼ���
  * Parameter:    u16sRH����ȡ����ʪ��ԭʼ����
  * Return:       ������ʪ������
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
float SHT2x_CalcRH(unsigned short u16sRH)
{
    float humidityRH = 0;              // variable for result

    u16sRH &= ~0x0003;          // clear bits [1..0] (status bits)
    
    humidityRH = ((float)u16sRH * 0.00190735) - 6;

    return humidityRH;
}


/**
  ******************************************************************************
  * Function:     SHT2x_MeasureHM()
  * Description:  ������ʪ��
  * Parameter:    cmd�������¶Ȼ���ʪ��
                  pMeasurand����Ϊ���򱣴�Ϊushortֵ���˵�ַ
  * Return:       �������
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
float SHT2x_MeasureHM(unsigned char cmd, unsigned short *pMeasurand)
{
    char  checksum = 0;  //checksum
    char  data[2];    //data array for checksum verification
    unsigned char addr = 0;
    unsigned short tmp = 0;
    float t = 0;

    addr = SHT20_ADDRESS << 1;

    IIC_Start();

    IIC_SendByte(addr);
    if(IIC_WaitAck(50000)) //�ȴ�Ӧ��
    return 0.0;

    IIC_SendByte(cmd);
    if(IIC_WaitAck(50000)) //�ȴ�Ӧ��
    return 0.0;

    IIC_Start();

    IIC_SendByte(addr + 1);
    while(IIC_WaitAck(50000)) //�ȴ�Ӧ��
    {
        IIC_Start();
        IIC_SendByte(addr + 1);
    }

    DelayXms(70);

    data[0] = IIC_RecvByte();
    IIC_Ack();
    data[1] = IIC_RecvByte();
    IIC_Ack();

    checksum = IIC_RecvByte();
    IIC_NAck();

    IIC_Stop();

    SHT2x_CheckCrc(data, 2, checksum);
    tmp = (data[0] << 8) + data[1];
    if(cmd == SHT20_Measurement_T_HM) {
        t = SHT2x_CalcTemperatureC(tmp);
    } else {
        t = SHT2x_CalcRH(tmp);
    }

    if(pMeasurand) {
        *pMeasurand = (unsigned short)t;
    }

    return t;
}


/**
  ******************************************************************************
  * Function:     SHT20_GetValue()
  * Description:  ��ȡ��ʪ������
  * Parameter:    info --��ʪ�Ƚṹ��ָ��
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void SHT20_GetValue(SHT20_INFO *info)
{
    unsigned char val = 0;
    SHT20_INFO sht20Info;

    IIC_SpeedCtl(5);

    SHT20_read_user_reg();
    DelayUs(100);

    sht20Info.tempreture = SHT2x_MeasureHM(SHT20_Measurement_T_HM, (void *)0);
    DelayXms(70);

    sht20Info.humidity = SHT2x_MeasureHM(SHT20_Measurement_RH_HM, (void *)0);
    DelayXms(25);

    SHT20_read_user_reg();
    DelayXms(25);

    I2C_WriteByte(SHT20_ADDRESS, SHT20_WRITE_REG, &val);
    DelayUs(100);

    SHT20_read_user_reg();
    DelayUs(100);

    SHT20_Reset();
    DelayUs(100);

    memcpy(info, &sht20Info, sizeof(SHT20_INFO));
}


