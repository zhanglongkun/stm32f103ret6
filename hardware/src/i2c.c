/**
  ******************************************************************************
  * @FileName:     i2c.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-20 17:19:09
  * @Description:  This file provides all the i2c.c functions. 
  ******************************************************************************
  */ 



//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//Ӳ������
#include "i2c.h"
#include "delay.h"
#include "usart.h"


IIC_INFO iicInfo;

void IIC_SpeedCtl(unsigned short speed)
{
    iicInfo.speed = speed;
}

void IIC_Init(void)
{
    GPIO_InitTypeDef gpioInitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//��GPIOB��ʱ��

    gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_OD;			//��©����������ȥ�л�������뷽��
    gpioInitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpioInitStruct);

    IIC_SpeedCtl(2);										//IIC�ٶȿ��ƣ���λ��΢��

    SDA_H;													//����SDA�ߣ����ڿ���״̬
    SCL_H;													//����SCL�ߣ����ڿ���״̬
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

_Bool IIC_WaitAck(unsigned int timeOut)
{
    SDA_H;DelayUs(iicInfo.speed);			//����SDA��
    SCL_H;DelayUs(iicInfo.speed);			//����SCL��

    while(SDA_R)							//�������SDA��Ϊ1����ȴ���Ӧ���ź�Ӧ��0
    {
        if(--timeOut)
        {
            UsartPrintf(USART1, "WaitAck TimeOut\r\n");
            
            IIC_Stop();						//��ʱδ�յ�Ӧ����ֹͣ����

            return IIC_Err;					//����ʧ��
        }

        DelayUs(iicInfo.speed);
    }

    SCL_L;									//����SCL�ߣ��Ա�����շ�����

    return IIC_OK;							//���سɹ�
}

void IIC_Ack(void)
{
    SCL_L;						//����SCL��
    SDA_L;						//����SDA��
    DelayUs(iicInfo.speed);
    SCL_H;						//����SCL��
    DelayUs(iicInfo.speed);
    SCL_L;						//����SCL��
}

void IIC_NAck(void)
{
    SCL_L;						//����SCL��
    SDA_H;						//����SDA��
    DelayUs(iicInfo.speed);
    SCL_H;						//����SCL��
    DelayUs(iicInfo.speed);
    SCL_L;						//����SCL��
}

void IIC_SendByte(unsigned char byte)
{
    unsigned char count = 0;

    SCL_L;							//����ʱ�ӿ�ʼ���ݴ���

    for(; count < 8; count++)		//ѭ��8�Σ�ÿ�η���һ��bit
    {
        if(byte & 0x80)				//�������λ
        SDA_H;
        else
        SDA_L;

        byte <<= 1;					//byte����1λ

        DelayUs(iicInfo.speed);
        SCL_H;
        DelayUs(iicInfo.speed);
        SCL_L;
    }
}

unsigned char IIC_RecvByte(void)
{
    unsigned char count = 0, receive = 0;

    SDA_H;							//����SDA�ߣ���©״̬�£����������Ա��ȡ����

    for(; count < 8; count++ )		//ѭ��8�Σ�ÿ�η���һ��bit
    {
        SCL_L;
        DelayUs(iicInfo.speed);
        SCL_H;

        receive <<= 1;				//����һλ

        if(SDA_R)					//���SDA��Ϊ1����receive����������ÿ���������Ƕ�bit0��+1��Ȼ����һ��ѭ����������һ��
        receive++;

        DelayUs(iicInfo.speed);
    }

    return receive;
}

_Bool I2C_WriteByte(unsigned char slaveAddr, unsigned char regAddr, unsigned char *byte)
{
    unsigned char addr = 0;

    addr = slaveAddr << 1;		//IIC��ַ��7bit��������Ҫ����1λ��bit0��1-��	0-д

    IIC_Start();				//��ʼ�ź�

    IIC_SendByte(addr);			//�����豸��ַ(д)
    if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
    return IIC_Err;

    IIC_SendByte(regAddr);		//���ͼĴ�����ַ
    if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
    return IIC_Err;

    if(byte)
    {
        IIC_SendByte(*byte);	//��������
        if(IIC_WaitAck(5000))	//�ȴ�Ӧ��
        return IIC_Err;
    }

    IIC_Stop();					//ֹͣ�ź�

    return IIC_OK;
}

_Bool I2C_ReadByte(unsigned char slaveAddr, unsigned char regAddr, unsigned char *val)
{
    unsigned char addr = 0;

    addr = slaveAddr << 1;		//IIC��ַ��7bit��������Ҫ����1λ��bit0��1-��	0-д

    IIC_Start();				//��ʼ�ź�

    IIC_SendByte(addr);			//�����豸��ַ(д)
    if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
        return IIC_Err;

    IIC_SendByte(regAddr);		//���ͼĴ�����ַ
    if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
        return IIC_Err;

    IIC_Start();				//�����ź�

    IIC_SendByte(addr + 1);		//�����豸��ַ(��)
    if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
        return IIC_Err;

    *val = IIC_RecvByte();		//����
    IIC_NAck();					//����һ����Ӧ���źţ������ȡ����

    IIC_Stop();					//ֹͣ�ź�

    return IIC_OK;
}

_Bool I2C_WriteBytes(unsigned char slaveAddr, unsigned char regAddr, unsigned char *buf, unsigned char num)
{
    unsigned char addr = 0;

    addr = slaveAddr << 1;		//IIC��ַ��7bit��������Ҫ����1λ��bit0��1-��	0-д

    IIC_Start();				//��ʼ�ź�

    IIC_SendByte(addr);			//�����豸��ַ(д)
    if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
    return IIC_Err;

    IIC_SendByte(regAddr);		//���ͼĴ�����ַ
    if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
    return IIC_Err;

    while(num--)				//ѭ��д������
    {
        IIC_SendByte(*buf);		//��������
        if(IIC_WaitAck(5000))	//�ȴ�Ӧ��
        return IIC_Err;

        buf++;					//����ָ��ƫ�Ƶ���һ��

        DelayUs(10);
    }

    IIC_Stop();					//ֹͣ�ź�

    return IIC_OK;
}

_Bool I2C_ReadBytes(unsigned char slaveAddr, unsigned char regAddr, unsigned char *buf, unsigned char num)
{
    unsigned short addr = 0;

    addr = slaveAddr << 1;		//IIC��ַ��7bit��������Ҫ����1λ��bit0��1-��	0-д

    IIC_Start();				//��ʼ�ź�

    IIC_SendByte(addr);			//�����豸��ַ(д)
    if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
    return IIC_Err;

    IIC_SendByte(regAddr);		//���ͼĴ�����ַ
    if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
    return IIC_Err;

    IIC_Start();				//�����ź�

    IIC_SendByte(addr + 1);		//�����豸��ַ(��)
    if(IIC_WaitAck(5000))		//�ȴ�Ӧ��
    return IIC_Err;

    while(num--)
    {
        *buf = IIC_RecvByte();
        buf++;					//ƫ�Ƶ���һ�����ݴ洢��ַ

        if(num == 0)
        {
            IIC_NAck();			//���һ��������Ҫ��NOACK
        }
        else
        {
            IIC_Ack();			//��ӦACK
        }
    }

    IIC_Stop();

    return IIC_OK;
}

