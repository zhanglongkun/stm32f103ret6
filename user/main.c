#include <string.h>

//单片机头文件
#include "stm32f10x.h"

//硬件驱动
#include "delay.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "exti.h"
#include "sht20.h"
#include "i2c.h"
#include "adxl345.h"

//os
#include "includes.h"

#include "interface.h"
#include "manage.h"


void Hardware_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//中断控制器分组设置

    Delay_Init();										//Systick初始化，用于普通的延时

    Led_Init();											//LED接口初始化

//    Key_Init();
//    
//    Exti_Key_Init();
    
    Usart1_Init(115200);
    Usart2_Init(115200);
    
//    IIC_Init();

//    ADXL345_Init();
}

unsigned char usart1Buf[64];
unsigned char usart1Len;
USART_IO_INFO usart1IOInfo;
USART_IO_INFO usart2IOInfo;

int main(void)
{
    USART_IO_INFO usartRev;
    unsigned short i = 0;
    
    Hardware_Init();											//硬件初始化
    UsartPrintf(USART1, "111\r\n");
    
    while (1) {
        if(Usart1_IO_WaitRecive() == REV_OK) {
            UsartPrintf(USART1, "buf = %s\r\n", usart1IOInfo.buf);

            for (i = 0; i < usart1IOInfo.dataLenPre; i++) {
                
                UsartPrintf(USART1, "%d ", usart1IOInfo.buf[i]);
            }
            UsartPrintf(USART1, "\r\n");

            memset(&usartRev, 0, sizeof(USART_IO_INFO));
            memcpy(&usartRev, &usart1IOInfo, sizeof(USART_IO_INFO));
            Conversion_Handle(usartRev);
            
            memset(&usart1IOInfo, 0, sizeof(USART_IO_INFO));
        }
        DelayUs(200);
    }
    
#if 0
    unsigned char err;
    OS_TMR *tmr;                    //软件定时器句柄
    OS_STK osstk[100];
    
    Hardware_Init();											//硬件初始化
    
    OSInit();
    UsartPrintf(USART1, "11111111\r\n");

    OSTaskCreate(WaterLights, (void *)0, &osstk[99], 7);
    UsartPrintf(USART1, "222\r\n");
	tmr = OSTmrCreate(100, 100, OS_TMR_OPT_PERIODIC, (OS_TMR_CALLBACK)NULL, 0, (INT8U *)"tmr1", &err);
	OSTmrStart(tmr, &err);
    UsartPrintf(USART1, "33\r\n");
    OSStart();
    UsartPrintf(USART1, "444\r\n");
#endif
    
#if 0
    
    char cmdBuf[30] = {0};
    SHT20_INFO sht20;
    ADXL345_INFO adxl345;
    int timeOut = 200;
    Hardware_Init();											//硬件初始化
    
	sprintf(cmdBuf, "AT+GSV\r\n");		//发送命令
	
    while(1)
    {

//            UsartPrintf(USART2,"1111\r\n");
            Usart2_Send_Cmd(cmdBuf, strlen(cmdBuf));
            DelayMs(2000);
        
            UsartPrintf(USART2, "11111111\r\n");

            
            while(timeOut--)                                                //等待
            {
                UsartPrintf(USART1, "22222\r\n");
                if(Usart2_IO_WaitRecive() == REV_OK)                           //如果收到数据
                {
                    UsartPrintf(USART1, "USART2 = \r\n");
                    UsartPrintf(USART1, "%s\r\n", usart2IOInfo.buf);
                    memset(&usart2IOInfo, 0, sizeof(USART_IO_INFO));
                    break;
                }
            }
            
            UsartPrintf(USART1, "33333\r\n");
            DelayXms(1000);
        UsartPrintf(USART1, "aaaa\r\n");
        Usart2_Send_Cmd();
        DelayMs(2000);
        UsartPrintf(USART1, "1111111\r\n");
        
		if(Usart2_IO_WaitRecive() == REV_OK)
		{
            UsartPrintf(USART1, "buf = %s\r\n", usart2IOInfo.buf);
            
            UsartPrintf(USART1, "2222\r\n");
		}
        DelayMs(2000);
        UsartPrintf(USART1, "3333\r\n");
#endif

        
#if 0
        UsartPrintf(USART1, "1111111\r\n");
        Usart2_Send_Cmd();
        UsartPrintf(USART1, "2222\r\n");
        DelayMs(2000);
        
        UsartPrintf(USART1, "33333\r\n");
        UsartPrintf(USART1, "buf = %s", usartIOInfo.buf);
        UsartPrintf(USART1, "\r\n44444\r\n");
#endif

#if 0
	while(1)
	{
		
		switch(Keyboard())
		{
			case KEY0DOWN:
                Led4_Set(LED_ON);
			break;
			
			case KEY1DOWN:
                Led5_Set(LED_ON);
			break;
			
			case KEY2DOWN:
                Led6_Set(LED_ON);
			break;
			
			case KEY3DOWN:
                Led7_Set(LED_ON);
			break;
		}
	}
    
    Led4_Set(LED_ON);
    
    SHT20_GetValue(&sht20);
    UsartPrintf(USART1, "temp = %0.1f, humi = %0.1f\r\n", sht20.tempreture, sht20.humidity);

    ADXL345_GetValue(&adxl345);
    UsartPrintf(USART1, "x = %0.1f, y = %0.1f, z = %0.1f\r\n",
                adxl345.incidence_Xf, adxl345.incidence_Yf, adxl345.incidence_Zf);

    DelayXms(2000);
#endif
    
}

