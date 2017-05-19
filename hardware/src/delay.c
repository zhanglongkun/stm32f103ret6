
//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//delayͷ�ļ�
#include "delay.h"



//��ʱϵ��
unsigned char UsCount = 0;
unsigned short MsCount = 0;


void Delay_Init(void)
{
    SysTick->CTRL &= ~(1 << 2);		//ѡ��ʱ��ΪHCLK(72MHz)/8		103--9MHz

    UsCount = 9;					//΢�뼶��ʱϵ��

    MsCount = UsCount * 1000;		//���뼶��ʱϵ��
}


void DelayUs(unsigned short us)
{
    unsigned int ctrlResult = 0;

    us &= 0x00FFFFFF;											//ȡ��24λ

    SysTick->LOAD = us * UsCount;								//װ������
    SysTick->VAL = 0;
    SysTick->CTRL = 1;											//ʹ�ܵ�������

    do
    {
        ctrlResult = SysTick->CTRL;
    }
    while((ctrlResult & 0x01) && !(ctrlResult & (1 << 16)));	//��֤�����С�����Ƿ񵹼�����0

    SysTick->CTRL = 0;											//�رյ�������
    SysTick->VAL = 0;
}


void DelayXms(unsigned short ms)
{
    unsigned int ctrlResult = 0;

    if(ms == 0)
        return;

    ms &= 0x00FFFFFF;											//ȡ��24λ

    SysTick->LOAD = ms * MsCount;								//װ������
    SysTick->VAL = 0;
    SysTick->CTRL = 1;											//ʹ�ܵ�������

    do
    {
        ctrlResult = SysTick->CTRL;
    }
    while((ctrlResult & 0x01) && !(ctrlResult & (1 << 16)));	//��֤�����С�����Ƿ񵹼�����0

    SysTick->CTRL = 0;											//�رյ�������
    SysTick->VAL = 0;
}


void DelayMs(unsigned short ms)
{
    unsigned char repeat = 0;
    unsigned short remain = 0;

    repeat = ms / 500;
    remain = ms % 500;

    while(repeat)
    {
        DelayXms(500);
        repeat--;
    }

    if(remain)
        DelayXms(remain);
}

