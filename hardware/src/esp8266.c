/**
  ******************************************************************************
  * @FileName:     esp8266.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-6-9 9:21:47
  * @Description:  This file provides all the esp8266.c functions. 
  ******************************************************************************
  */ 


#include "esp8266.h"


#define NET_DEVICE_RST_ON		GPIO_ResetBits(GPIOA, GPIO_Pin_0)
#define NET_DEVICE_RST_OFF		GPIO_SetBits(GPIOA, GPIO_Pin_0)


#define ESP8266_DBG(fmt, args...) \
    do {\
        UsartPrintf(USART1, "%s-->%s(%d)-->", __FILE__, __func__, __LINE__); \
        UsartPrintf(USART1, fmt, ##args);\
        UsartPrintf(USART1, "\r\n");\
    } while (0)


WIFI_INFO wifiInfo = {"JQM-201", "jqmkj123", 0, 0, 0, 0};
    
SERVICE_INFO_8266 service = {"106.75.148.220", "6666"};

/**
  ******************************************************************************
  * Function:     ESP8266_IO_Init()
  * Description:  网络 IO 初始化
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-07-14
  ******************************************************************************
  */ 
void ESP8266_IO_Init(void)
{
    GPIO_InitTypeDef gpioInitStruct;
    USART_InitTypeDef usartInitStruct;
    NVIC_InitTypeDef nvicInitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    //PA2   TXD
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);

    //PA3   RXD
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);

    usartInitStruct.USART_BaudRate = 115200;
    usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //接收和发送
    usartInitStruct.USART_Parity = USART_Parity_No;                             //无校验
    usartInitStruct.USART_StopBits = USART_StopBits_1;                          //1位停止位
    usartInitStruct.USART_WordLength = USART_WordLength_8b;                     //8位数据位
    USART_Init(USART2, &usartInitStruct);

    USART_Cmd(USART2, ENABLE);                                                  //使能串口

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                              //使能接收中断

    nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvicInitStruct);

    ESP8266_IO_ClearRecive();
    
    return;
}


/**
  ******************************************************************************
  * Function:     ESP8266_Init()
  * Description:  ESP8266 初始化
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-07-14
  ******************************************************************************
  */ 
void ESP8266_Init(void)
{
	GPIO_InitTypeDef gpioInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//ESP8266复位引脚
	gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_0;					//GPIOA0-复位
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);

    ESP8266_IO_Init();

    return;
}


/**
  ******************************************************************************
  * Function:     ESP8266_Device_InitStep()
  * Description:  连接wifi、服务器
  * Parameter:    void
  * Return:       0 --成功，1 --失败
  * Others:       add by zlk, 2017-07-14
  ******************************************************************************
  */ 
uint8 ESP8266_Device_InitStep(void)
{
	unsigned char errCount = 0;
    USART_IO_INFO gsmRevBuf = {0};
    char cfgBuffer[32] = {0};
    
    switch (wifiInfo.initStep) {
        case 0:
            UsartPrintf(USART_DEBUG, "STA Tips:	AT+CWMODE=1\r\n");
            if(!ESP8266_Device_SendCmd("AT+CWMODE=1\r\n", "OK", &gsmRevBuf)) 										//station模式
                wifiInfo.initStep++;

            break;

        case 1:
            UsartPrintf(USART_DEBUG, "STA Tips:	AT+CIPMODE=0\r\n");
            if(!ESP8266_Device_SendCmd("AT+CIPMODE=0\r\n", "OK", &gsmRevBuf)) 										//关闭透传模式---指令模式
                wifiInfo.initStep++;
            break;
        
        case 2:
            if (!ESP8266_Device_SendCmd("AT+CWJAP=\"JQM-201\",\"jqmkj123\"\r\n", "GOT IP", &gsmRevBuf)) {
                wifiInfo.initStep++;
            }
            
            OSTimeDly(100);
            ESP8266_DBG("000000000, buffer = %s", gsmRevBuf.buf);
            
            wifiInfo.initStep++;
            break;
            
        case 3:

            memset(cfgBuffer, 0, sizeof(cfgBuffer));

            strcpy(cfgBuffer, "AT+CIPSTART=\"TCP\",\"");
            strcat(cfgBuffer, service.ip);
            strcat(cfgBuffer, "\",");
            strcat(cfgBuffer, service.port);
            strcat(cfgBuffer, "\r\n");
            UsartPrintf(USART_DEBUG, "STA Tips: %s", cfgBuffer);

            while(ESP8266_Device_SendCmd(cfgBuffer, "CONNECT", &gsmRevBuf))              //连接平台，检索“CONNECT”，如果失败会进入循环体
            {
                ESP8266_DBG("buffer = %s", gsmRevBuf.buf);
                
                OSTimeDly(1000);

                if(++errCount >= 10)
                {
                    UsartPrintf(USART_DEBUG, "PT info Error,Use APP -> 8266\r\n");
                    break;
                }

                memset(&gsmRevBuf, 0, sizeof(USART_IO_INFO));
            }

                if(errCount != 10) {
#if(NET_DEVICE_MODE == 1)
                    ESP8266_EnterTrans();
#else
                    wifiInfo.initStep++;
                    ESP8266_DBG("连接成功");
                    ESP8266_DBG("buffer = %s", gsmRevBuf.buf);
#if 0
                    memset(&gsmRevBuf, 0, sizeof(USART_IO_INFO));
                    if (!ESP8266_Device_SendCmd("AT+CIPSTATUS\r\n", "CONNECT OK", &gsmRevBuf)) {
                        ESP8266_DBG("buffer = %s", gsmRevBuf.buf);
                    }
                    ESP8266_DBG("buffer = %s", gsmRevBuf.buf);
                    
//                    ESP8266_EnterTrans();
#endif
                    return 0;
#endif
                }
        
            break;

        default:
            break;
            
    }
    return 1;
}


/**
  ******************************************************************************
  * Function:     ESP8266_Device_Init()
  * Description:  网络初始化
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-07-14
  ******************************************************************************
  */ 
void ESP8266_Device_Init(void)
{
    uint8 res = 1;
    ESP8266_DBG("复位");
    NET_DEVICE_Reset();
    ESP8266_DBG("");
    ESP8266_QuitTrans();	//退出透传模式

    while (res) {
        ESP8266_DBG("");
        res = ESP8266_Device_InitStep();
    }

    ESP8266_DBG("初始化成功");
    NET_DEVICE_SendData("hello", sizeof("hello"));
    OSTimeDly(500);
}


/**
  ******************************************************************************
  * Function:     ESP8266_IO_WaitRecive()
  * Description:  等待数据接收完成
  * Parameter:    void
  * Return:       0 --未接收到数据或者数据接收还未结束，1 --接收完成
  * Others:       add by zlk, 2017-07-14
  ******************************************************************************
  */ 
uint8 ESP8266_IO_WaitRecive(void)
{
    if(usart2IOInfo.dataLen == 0) //如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
        return REV_WAIT;

    if(usart2IOInfo.dataLen == usart2IOInfo.dataLenPre) //如果上一次的值和这次相同，则说明接收完毕
    {
        usart2IOInfo.dataLen = 0; //清0接收计数
    	
        return REV_OK; //跳出
    }

    usart2IOInfo.dataLenPre = usart2IOInfo.dataLen; //置为相同

    return REV_WAIT;
}


/**
  ******************************************************************************
  * Function:     ESP8266_Device_SendCmd()
  * Description:  发送命令
  * Parameter:    cmd --命令，res --要返回的数据，revBuf --接收到的数据
  * Return:       返回值
  * Others:       add by zlk, 2017-07-14
  ******************************************************************************
  */ 
uint8 ESP8266_Device_SendCmd(char *cmd, char *res, USART_IO_INFO *revBuf)
{
    unsigned char timeout = 300;
    
    ESP8266_SendString((uint8 *)cmd, strlen((uint8 *)cmd));

    while (timeout--) {
        if (ESP8266_IO_WaitRecive() == REV_OK) {
            memcpy(revBuf, &usart2IOInfo, sizeof(USART_IO_INFO));

            if (strstr((const char *)revBuf->buf, res) != NULL) {
                ESP8266_IO_ClearRecive();
                return 0;
            }
        }

        DelayUs(200);
    }
    
    ESP8266_IO_ClearRecive();

    return 1;
}


/**
  ******************************************************************************
  * Function:     ESP8266_IO_ClearRecive()
  * Description:  清除缓存
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-07-14
  ******************************************************************************
  */ 
void ESP8266_IO_ClearRecive(void)
{
    usart2IOInfo.dataLen = 0;

    memset(usart2IOInfo.buf, 0, sizeof(usart2IOInfo.buf));
}


/**
  ******************************************************************************
  * Function:     ESP8266_SendString()
  * Description:  发送数据
  * Parameter:    str --发送的数据，len --发送数据的大小
  * Return:       返回值
  * Others:       add by zlk, 2017-07-14
  ******************************************************************************
  */ 
void ESP8266_SendString(unsigned char *str, unsigned char len)
{
    unsigned short count = 0;

    for(; count < len; count++)
    {
        USART_SendData(ESP8266_IO, *str++);									//发送数据
        while(USART_GetFlagStatus(ESP8266_IO, USART_FLAG_TC) == RESET);		//等待发送完成
    }
}


/**
  ******************************************************************************
  * Function:     ESP8266_ReceiveString()
  * Description:  接收服务器数据
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-07-14
  ******************************************************************************
  */ 
void ESP8266_ReceiveString(void)
{
    unsigned char ret;
    int i = 0;
    USART_IO_INFO revBuf;

    while (1) {
        memset(&revBuf, 0, sizeof(revBuf));
        ESP8266_DBG("等待接收中.....");
        ret = Net_Device_GetData(&revBuf);
        if (ret == 0) {
            UsartPrintf(USART_DEBUG, "%s", revBuf.buf);
        }
        
        OSTimeDly(10);
    }
}


/**
  ******************************************************************************
  * Function:     ESP8266_QuitTrans()
  * Description:  退出透传
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-07-14
  ******************************************************************************
  */ 
void ESP8266_QuitTrans(void)
{
    USART_IO_INFO gsmRevBuf = {0};
    
    while((ESP8266_IO->SR & 0X40) == 0);	//等待发送空
    ESP8266_IO->DR = '+';
    OSTimeDly(3); 					//大于串口组帧时间(10ms)

    while((ESP8266_IO->SR & 0X40) == 0);	//等待发送空
    ESP8266_IO->DR = '+';        
    OSTimeDly(3); 					//大于串口组帧时间(10ms)

    while((ESP8266_IO->SR & 0X40) == 0);	//等待发送空
    ESP8266_IO->DR = '+';        
    OSTimeDly(20);					//等待100ms

    ESP8266_Device_SendCmd("AT+CIPMODE=0\r\n", "OK", &gsmRevBuf); //关闭透传模式
}


/**
  ******************************************************************************
  * Function:     ESP8266_EnterTrans()
  * Description:  进入透传模式
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-07-14
  ******************************************************************************
  */ 
void ESP8266_EnterTrans(void)
{
    USART_IO_INFO gsmRevBuf = {0};
    
    ESP8266_Device_SendCmd("AT+CIPMUX=0\r\n", "OK", &gsmRevBuf);	//单链接模式

    ESP8266_Device_SendCmd("AT+CIPMODE=1\r\n", "OK", &gsmRevBuf);//使能透传

    ESP8266_Device_SendCmd("AT+CIPSEND\r\n", ">", &gsmRevBuf);	//发送数据

    OSTimeDly(20);								//等待100ms
}


/**
  ******************************************************************************
  * Function:     NET_DEVICE_Reset()
  * Description:  设备复位
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-07-14
  ******************************************************************************
  */ 
void NET_DEVICE_Reset(void)
{
#if (NET_DEVICE_MODE == 1)
    ESP8266_QuitTrans();	//退出透传模式
    UsartPrintf(USART_DEBUG, "Tips:	QuitTrans\r\n");
#endif

    UsartPrintf(USART_DEBUG, "Tips:	ESP8266_Reset\r\n");

    NET_DEVICE_RST_ON;		//复位
    OSTimeDly(50);

    NET_DEVICE_RST_OFF;		//结束复位
    OSTimeDly(200);
}



/**
  ******************************************************************************
  * Function:     NET_DEVICE_SendData()
  * Description:  数据发送，如果发送失败，则重新连接服务器在发送
  * Parameter:    data --发送的数据，len --数据的长度
  * Return:       void
  * Others:       add by zlk, 2017-07-14
  ******************************************************************************
  */ 
void NET_DEVICE_SendData(unsigned char *data, unsigned short len)
{
#if 0
    ESP8266_DBG("NET_DEVICE_SendData");
    ESP8266_SendString(data, len);  						//发送设备连接请求数据
#endif

    USART_IO_INFO gsmRevBuf = {0};
#if (NET_DEVICE_MODE == 1)
    ESP8266_DBG("NET_DEVICE_SendData");
    ESP8266_SendString(data, len);  						//发送设备连接请求数据
#else
    char cmdBuf[32];

AGAIN:
    OSTimeDly(10);								//等待一下

    ESP8266_IO_ClearRecive();							//清空接收缓存
    sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//发送命令
    if(!ESP8266_Device_SendCmd(cmdBuf, ">", &gsmRevBuf))			//收到‘>’时可以发送数据
    {
        ESP8266_SendString(data, len);  					//发送设备连接请求数据
        ESP8266_DBG("send success");
    } else {
        ESP8266_DBG("buffer = %s", gsmRevBuf.buf);
        ESP8266_DBG("connect again");
        if (!NET_DEVICE_ReLink()) {
            goto AGAIN;
        } else {
            ESP8266_DBG("reconnect server failed");
        }
    }
#endif
    return;
}


/**
  ******************************************************************************
  * Function:     NET_DEVICE_ReLink()
  * Description:  重新连接服务器，最多尝试连接10次
  * Parameter:    void
  * Return:       0 --成功，1 --失败
  * Others:       add by zlk, 2017-07-14
  ******************************************************************************
  */ 
uint8 NET_DEVICE_ReLink(void)
{
    USART_IO_INFO gsmRevBuf = {0};
    char cfgBuffer[32] = {0};
	unsigned char errCount = 0;
    
#if (NET_DEVICE_MODE == 1)
    ESP8266_QuitTrans();    //退出透传模式
    UsartPrintf(USART_DEBUG, "Tips: QuitTrans\r\n");
#endif
    
    //连接前先关闭一次
    if (ESP8266_Device_SendCmd("AT+CIPCLOSE\r\n", "OK", &gsmRevBuf)) {
        ESP8266_DBG("close error, buffer = %s", gsmRevBuf.buf);
//        return 1;
    }
    
    OSTimeDly(500);
    
    memset(cfgBuffer, 0, sizeof(cfgBuffer));

    strcpy(cfgBuffer, "AT+CIPSTART=\"TCP\",\"");
    strncat(cfgBuffer, service.ip, 15);
    strcat(cfgBuffer, "\",");
    strncat(cfgBuffer, service.port, 4);
    strcat(cfgBuffer, "\r\n");
    
//    sprintf(cfgBuffer, "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n", service.ip, service.port);
    UsartPrintf(USART_DEBUG, "STA Tips: %s", cfgBuffer);

//    while(ESP8266_Device_SendCmd((char *)cfgBuffer, "CONNECT", &gsmRevBuf))              //连接平台，检索“CONNECT”，如果失败会进入循环体
    while(ESP8266_Device_SendCmd("AT+CIPSTART=\"TCP\",\"106.75.148.220\",6666\r\n", "CONNECT", &gsmRevBuf))              //连接平台，检索“CONNECT”，如果失败会进入循环体
    {
        ESP8266_DBG("connect server error, buffer = %s", &gsmRevBuf.buf);
        
        OSTimeDly(200);

        if(++errCount >= 10)
        {
            UsartPrintf(USART_DEBUG, "PT info Error,Use APP -> 8266\r\n");
            break;
        }

        memset(&gsmRevBuf, 0, sizeof(USART_IO_INFO));
    }

    if(errCount != 10) {
#if (NET_DEVICE_MODE == 1)
        ESP8266_EnterTrans();
#else
        ESP8266_DBG("连接成功");
        return 0;
    }
#endif
    return 1;
}

