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
    usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //���պͷ���
    usartInitStruct.USART_Parity = USART_Parity_No;                             //��У��
    usartInitStruct.USART_StopBits = USART_StopBits_1;                          //1λֹͣλ
    usartInitStruct.USART_WordLength = USART_WordLength_8b;                     //8λ����λ
    USART_Init(USART2, &usartInitStruct);

    USART_Cmd(USART2, ENABLE);                                                  //ʹ�ܴ���

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                              //ʹ�ܽ����ж�

    nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvicInitStruct);

    ESP8266_IO_ClearRecive();
    
    return;
}


void ESP8266_Init(void)
{
	GPIO_InitTypeDef gpioInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//ESP8266��λ����
	gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_0;					//GPIOA0-��λ
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);

    ESP8266_IO_Init();

    return;
}


uint8 ESP8266_Device_InitStep(void)
{
	unsigned char errCount = 0;
    USART_IO_INFO gsmRevBuf = {0};
    char cfgBuffer[32] = {0};
    
    switch (wifiInfo.initStep) {
        case 0:
            UsartPrintf(USART_DEBUG, "STA Tips:	AT+CWMODE=1\r\n");
            if(!ESP8266_Device_SendCmd("AT+CWMODE=1\r\n", "OK", &gsmRevBuf)) 										//stationģʽ
                wifiInfo.initStep++;

            break;

        case 1:
            UsartPrintf(USART_DEBUG, "STA Tips:	AT+CIPMODE=0\r\n");
            if(!ESP8266_Device_SendCmd("AT+CIPMODE=0\r\n", "OK", &gsmRevBuf)) 										//�ر�͸��ģʽ---ָ��ģʽ
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

            while(ESP8266_Device_SendCmd(cfgBuffer, "CONNECT", &gsmRevBuf))              //����ƽ̨��������CONNECT�������ʧ�ܻ����ѭ����
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
                    ESP8266_DBG("���ӳɹ�");
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


void ESP8266_Device_Init(void)
{
    uint8 res = 1;
    UsartPrintf(USART_DEBUG, "ffff\r\n");
    ESP8266_DBG("111111");
    NET_DEVICE_Reset();
    ESP8266_DBG("aaaaaaaaa");
    ESP8266_QuitTrans();	//�˳�͸��ģʽ

    while (res) {
        ESP8266_DBG("vvvvvvvvvvvv");
        res = ESP8266_Device_InitStep();
    }

    ESP8266_DBG("2222222222");
    NET_DEVICE_SendData("hello", sizeof("hello"));
    OSTimeDly(500);
}


uint8 ESP8266_IO_WaitRecive(void)
{
    if(usart2IOInfo.dataLen == 0) //������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
        return REV_WAIT;

    if(usart2IOInfo.dataLen == usart2IOInfo.dataLenPre) //�����һ�ε�ֵ�������ͬ����˵���������
    {
        usart2IOInfo.dataLen = 0; //��0���ռ���
    	
        return REV_OK; //����
    }

    usart2IOInfo.dataLenPre = usart2IOInfo.dataLen; //��Ϊ��ͬ

    return REV_WAIT;
}


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


void ESP8266_IO_ClearRecive(void)
{
    usart2IOInfo.dataLen = 0;

    memset(usart2IOInfo.buf, 0, sizeof(usart2IOInfo.buf));
}


void ESP8266_SendString(unsigned char *str, unsigned char len)
{
    unsigned short count = 0;

    for(; count < len; count++)
    {
        USART_SendData(ESP8266_IO, *str++);									//��������
        while(USART_GetFlagStatus(ESP8266_IO, USART_FLAG_TC) == RESET);		//�ȴ��������
    }
}


void ESP8266_ReceiveString(void)
{
    unsigned char ret;
    int i = 0;
    USART_IO_INFO revBuf;

    while (1) {
        memset(&revBuf, 0, sizeof(revBuf));
//        NET_DEVICE_SendData("hello", sizeof("hello"));
        ESP8266_DBG("tttttttttt.....");
        ret = Net_Device_GetData(&revBuf);
        if (ret == 0) {
            UsartPrintf(USART_DEBUG, "%s", revBuf.buf);
        }
        
        OSTimeDly(10);
    }
}


void ESP8266_QuitTrans(void)
{
    USART_IO_INFO gsmRevBuf = {0};
    
    while((ESP8266_IO->SR & 0X40) == 0);	//�ȴ����Ϳ�
    ESP8266_IO->DR = '+';
    OSTimeDly(3); 					//���ڴ�����֡ʱ��(10ms)

    while((ESP8266_IO->SR & 0X40) == 0);	//�ȴ����Ϳ�
    ESP8266_IO->DR = '+';        
    OSTimeDly(3); 					//���ڴ�����֡ʱ��(10ms)

    while((ESP8266_IO->SR & 0X40) == 0);	//�ȴ����Ϳ�
    ESP8266_IO->DR = '+';        
    OSTimeDly(20);					//�ȴ�100ms

    ESP8266_Device_SendCmd("AT+CIPMODE=0\r\n", "OK", &gsmRevBuf); //�ر�͸��ģʽ
}


void ESP8266_EnterTrans(void)
{
    USART_IO_INFO gsmRevBuf = {0};
    
    ESP8266_Device_SendCmd("AT+CIPMUX=0\r\n", "OK", &gsmRevBuf);	//������ģʽ

    ESP8266_Device_SendCmd("AT+CIPMODE=1\r\n", "OK", &gsmRevBuf);//ʹ��͸��

    ESP8266_Device_SendCmd("AT+CIPSEND\r\n", ">", &gsmRevBuf);	//��������

    OSTimeDly(20);								//�ȴ�100ms
}


void NET_DEVICE_Reset(void)
{
#if(NET_DEVICE_MODE == 1)
    ESP8266_QuitTrans();	//�˳�͸��ģʽ
    UsartPrintf(USART_DEBUG, "Tips:	QuitTrans\r\n");
#endif

    UsartPrintf(USART_DEBUG, "Tips:	ESP8266_Reset\r\n");

    NET_DEVICE_RST_ON;		//��λ
    OSTimeDly(50);

    NET_DEVICE_RST_OFF;		//������λ
    OSTimeDly(200);
}



void NET_DEVICE_SendData(unsigned char *data, unsigned short len)
{
#if 0
    ESP8266_DBG("NET_DEVICE_SendData");
    ESP8266_SendString(data, len);  						//�����豸������������
#endif

    USART_IO_INFO gsmRevBuf = {0};
#if(NET_DEVICE_MODE == 1)
    ESP8266_DBG("NET_DEVICE_SendData");
    ESP8266_SendString(data, len);  						//�����豸������������
#else
    char cmdBuf[32];
    ESP8266_DBG("344");

AGAIN:
    OSTimeDly(10);								//�ȴ�һ��

    ESP8266_IO_ClearRecive();							//��ս��ջ���
    sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
    if(!ESP8266_Device_SendCmd(cmdBuf, ">", &gsmRevBuf))			//�յ���>��ʱ���Է�������
    {
        ESP8266_SendString(data, len);  					//�����豸������������
        ESP8266_DBG("send success");
    } else {
    
#if 0
        ESP8266_Device_SendCmd("AT+CIPCLOSE\r\n", "OK", &gsmRevBuf);

        ESP8266_Device_SendCmd("AT+CIPSTART=\"TCP\",\"106.75.148.220\",6666\r\n", "CONNECT", &gsmRevBuf);
#endif

#if 1
        ESP8266_DBG("buffer = %s", gsmRevBuf.buf);
        ESP8266_DBG("connect again");
        if (!NET_DEVICE_ReLink()) {
            goto AGAIN;
        } else {
            ESP8266_DBG("reconnect server failed");
        }
#endif
    }
#endif

    return;
}


uint8 NET_DEVICE_ReLink(void)
{
    USART_IO_INFO gsmRevBuf = {0};
    char cfgBuffer[32] = {0};
	unsigned char errCount = 0;
    
#if 0
    ESP8266_Device_SendCmd("AT+CIPCLOSE\r\n", "OK", &gsmRevBuf);
    
    ESP8266_Device_SendCmd("AT+CIPSTART=\"TCP\",\"106.75.148.220\",6666\r\n", "CONNECT", &gsmRevBuf);
#endif
#if 1
#if(NET_DEVICE_MODE == 1)
    ESP8266_QuitTrans();    //�˳�͸��ģʽ
    UsartPrintf(USART_DEBUG, "Tips: QuitTrans\r\n");
#endif
    
    //����ǰ�ȹر�һ��
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

//    while(ESP8266_Device_SendCmd((char *)cfgBuffer, "CONNECT", &gsmRevBuf))              //����ƽ̨��������CONNECT�������ʧ�ܻ����ѭ����
    while(ESP8266_Device_SendCmd("AT+CIPSTART=\"TCP\",\"106.75.148.220\",6666\r\n", "CONNECT", &gsmRevBuf))              //����ƽ̨��������CONNECT�������ʧ�ܻ����ѭ����
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
#if(NET_DEVICE_MODE == 1)
        ESP8266_EnterTrans();
#else
        ESP8266_DBG("���ӳɹ�");
        ESP8266_DBG("buffer = %s", gsmRevBuf.buf);
        return 0;
    }
#endif
    return 1;
#endif
}

