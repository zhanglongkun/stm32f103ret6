//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//Ӳ������
#include "delay.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "exti.h"


void Hardware_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//�жϿ�������������

    Delay_Init();										//Systick��ʼ����������ͨ����ʱ

    Led_Init();											//LED�ӿڳ�ʼ��

    Key_Init();
    
    Exti_PC11_Config();
}


int main(void)
{
    Hardware_Init();											//Ӳ����ʼ��

    WaterLights();
}

