/**
  ******************************************************************************
  * @FileName:     beep.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-22 11:45:45
  * @Description:  This file provides all the beep.c functions. 
  ******************************************************************************
  */ 



//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//Ӳ������
#include "beep.h"


BEEP_INFO beepInfo = {0};


/**
  ******************************************************************************
  * Function:     Beep_Init()
  * Description:  ��ʼ�� beep
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void Beep_Init(void)
{
    GPIO_InitTypeDef gpioInitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//��GPIOA��ʱ��

    gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;				//����Ϊ���
    gpioInitStruct.GPIO_Pin = GPIO_Pin_4;						//����ʼ����Pin��
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;				//�ɳ��ص����Ƶ��

    GPIO_Init(GPIOA, &gpioInitStruct);							//��ʼ��GPIO

    Beep_Set(BEEP_OFF);											//��ʼ����ɺ󣬹رշ�����
}

/**
  ******************************************************************************
  * Function:     Beep_Set()
  * Description:  ����������
  * Parameter:    status --����
  * Return:       void
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void Beep_Set(_Bool status)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, status == BEEP_ON ? Bit_SET : Bit_RESET);		//���status����BEEP_ON���򷵻�Bit_SET�����򷵻�Bit_RESET

    beepInfo.Beep_Status = status;
}

