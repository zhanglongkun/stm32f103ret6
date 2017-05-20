/**
  ******************************************************************************
  * @FileName:     i2c.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-20 17:19:09
  * @Description:  This file provides all the i2c.c functions. 
  ******************************************************************************
  */ 

#include "stm32f10x.h"
#include "i2c.h"

//SDA		PB11
//SCL		PB10
#define SDA_H	GPIO_SetBits(GPIOB, GPIO_Pin_11)
#define SDA_L	GPIO_ResetBits(GPIOB, GPIO_Pin_11)
#define SDA_R	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)

#define SCL_H	GPIO_SetBits(GPIOB, GPIO_Pin_10)
#define SCL_L	GPIO_ResetBits(GPIOB, GPIO_Pin_10)

IIC_INFO iicInfo;


void IIC_SpeedCtl(unsigned short speed)
{
    iicInfo.speed = speed;
}


void IIC_Init(void)
{
    GPIO_InitTypeDef gpioInit;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    gpioInit.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    gpioInit.GPIO_Mode = GPIO_Mode_Out_OD;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &gpioInit);

    IIC_SpeedCtl(2);

    SDA_H;
    SCL_H;
}


void IIC_Start(void)
{
    SDA_H;						//����SDA��
    SCL_H;						//����SCL��
    DelayUs(iicInfo.speed);		//��ʱ���ٶȿ���

    SDA_L;						//��SCL��Ϊ��ʱ��SDA��һ���½��ش���ʼ�ź�
    DelayUs(iicInfo.speed);		//��ʱ���ٶȿ���
    SCL_L;						//ǯסSCL�ߣ��Ա㷢������
}


void IIC_Stop(void)
{
    SDA_L;						//����SDA��
    SCL_L;						//����SCL��
    DelayUs(iicInfo.speed);		//��ʱ���ٶȿ���

    SCL_H;						//����SCL��
    SDA_H;						//����SDA�ߣ���SCL��Ϊ��ʱ��SDA��һ�������ش���ֹͣ�ź�
    DelayUs(iicInfo.speed);
}

