//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//Ӳ������
#include "delay.h"
#include "led.h"
#include "sht20.h"
#include "usart.h"
#include "i2c.h"


void Hardware_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//�жϿ�������������

    Delay_Init();										//Systick��ʼ����������ͨ����ʱ

    Led_Init();											//LED�ӿڳ�ʼ��
    Usart1_Init(115200);
    
    IIC_Init();
}


int main(void)
{
    SHT20_INFO sht20;
    
    Hardware_Init();											//Ӳ����ʼ��
    
    Led4_Set(LED_ON);
    
    SHT20_GetValue(&sht20);
    UsartPrintf(USART1, "temp = %0.1f, humi = %0.1f\r\n", sht20.tempreture, sht20.humidity);

    DelayXms(2000);
    
}

