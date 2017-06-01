/**
  ******************************************************************************
  * @FileName:     gsm.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-31 21:24:29
  * @Description:  This file provides all the gsm.c functions. 
  ******************************************************************************
  */ 


#include "sim808.h"


GSM_DEVICE_INFO gsmDeviceInfo = {0};
SERVICE_INFO serviceInfo = {"106.75.148.220", "6666"}



/**
  ******************************************************************************
  * Function:     GSM_IO_Init()
  * Description:  ��ʼ������
  * Parameter:    baud --������
  * Return:       void
  * Others:       add by zlk, 2017-06-01
  ******************************************************************************
  */ 
void GSM_IO_Init(unsigned int baud)
{
    Usart2_Init(baud);

    return;
}


/**
  ******************************************************************************
  * Function:     GSM_Device_InitStep()
  * Description:  GSM����
  * Parameter:    void
  * Return:       sim_status --���е�״̬
  * Others:       add by zlk, 2017-06-01
  ******************************************************************************
  */ 
sim_status GSM_Device_InitStep()
{
    char cfgBuffer[32] = {0};
    
    switch (gsmDeviceInfo.initStep) {
        case 0:
            //���ģ���Ƿ�����
            UsartPrintf(USART1, "AT\r\n");
            if (GSM_Device_SendCmd("AT\r\n", "OK", 1)) {
                return SIM_COMMUNTION_ERR;
            }
            gsmDeviceInfo.initStep++;
            break;

        case 1:
            //sim���Ƿ����,���������ƶ��豸(ME)������.READY(��ʾ SIM������,ͬʱ����Ҫ��������)
            UsartPrintf(USART1, "AT+CPIN?\r\n");
            if (GSM_Device_SendCmd("AT+CPIN?\r\n", "READY", 1)) {
                return SIM_CPIN_ERR;
            }
            gsmDeviceInfo.initStep++;
            break;

        case 2:
            //����ע�ἰ״̬��ѯ
            UsartPrintf(USART1, "AT+CREG?\r\n");
            if (GSM_Device_SendCmd("AT+CREG?\r\n", "0,1", 1)) {
                return SIM_CREG_ERR;	
            }
            gsmDeviceInfo.initStep++;
            break;

        case 3:
            //��ѯ�ź�ǿ��
            UsartPrintf(USART1, "AT+CSQ\r\n");
            if (GSM_Device_SendCmd("AT+CSQ\r\n","OK", 1)) {
                return SIM_CSQ_ERR;	
            }
            gsmDeviceInfo.initStep++;
            break;

        case 4:
            //�������ע��״̬
            UsartPrintf(USART1, "AT+CGREG?\r\n");
            if (GSM_Device_SendCmd("AT+CGREG?\r\n","OK", 1)) {
                return SIM_CGREG_ERR;	
            }
            gsmDeviceInfo.initStep++;
            break;

 

        case 5:
            //����GPRSҵ��
            UsartPrintf(USART1, "AT+CGATT=1\r\n");
            if (GSM_Device_SendCmd("AT+CGATT=1\r\n", "OK", 1)) {
                return SIM_CGATT_ERR;	
            }
            gsmDeviceInfo.initStep++;
            break;
            
        case 6:
            //����PDP������,��������Э��,��������Ϣ
            UsartPrintf(USART1, "AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");
            if (GSM_Device_SendCmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n", "OK", 1)) {
                return SIM_CIPSHUT_ERR;   
            }
            gsmDeviceInfo.initStep++;
            break;

        case 7:
            //����ΪGPRS����ģʽ
            UsartPrintf(USART1, "AT+CIPCSGP=1,\"CMNET\"\r\n");
            if (GSM_Device_SendCmd("AT+CIPCSGP=1,\"CMNET\"\r\n", "OK", 1)) {
                return SIM_CIPCSGP_ERR;   
            }
            gsmDeviceInfo.initStep++;
            break;

        
        case 8:
            //��ȡIP��ַ
            UsartPrintf(USART1, "AT+CIFSR\r\n");
            if (!GSM_Device_SendCmd("AT+CIFSR\r\n", "ERROR", 1)) {
                return SIM_CIPCSGP_ERR;   
            }
            gsmDeviceInfo.initStep++;
            break;

        
        case 9:
            //���ӷ�����
            memset(cfgBuffer, 0, sizeof(cfgBuffer));

            strcpy(cfgBuffer, "AT+CIPSTART=\"TCP\",\"");
            strcat(cfgBuffer, serviceInfo.ip);
            strcat(cfgBuffer, "\",");
            strcat(cfgBuffer, serviceInfo.port);
            strcat(cfgBuffer, "\r\n");
            UsartPrintf(USART1, "STA Tips:	%s", cfgBuffer);
            
            if (GSM_Device_SendCmd(cfgBuffer, "CONNECT", 1)) {
                return SIM_CIPSTART_ERR;   
            }
            gsmDeviceInfo.initStep++;
            break;

        
        case 10:
            //״̬��ѯ
            UsartPrintf(USART1, "AT+CIPSTATUS\r\n");
            if (GSM_Device_SendCmd("AT+CIPSTATUS\r\n", "CONNECT OK", 1)) {
                return SIM_CIPSTART_ERR;   
            }
            gsmDeviceInfo.initStep++;
            break;

        default:
            UsartPrintf(USART1, "parameter error\r\n");
            break;
    }

    if (11 == gsmDeviceInfo.initStep) {
        return SIM_OK;
    }

    return SIM_COMTINUE;
}


/**
  ******************************************************************************
  * Function:     GSM_Device_Init()
  * Description:  GSM����
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-01
  ******************************************************************************
  */ 
uint8 GSM_Device_Init()
{
    sim_status res = 1;

    while (res) {
        res = GSM_Device_InitStep();
    }
}


/**
  ******************************************************************************
  * Function:     GSM_IO_WaitRecive()
  * Description:  �ж������Ƿ�������
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-01
  ******************************************************************************
  */ 
uint8 GSM_IO_WaitRecive()
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

/**
  ******************************************************************************
  * Function:     GSM_IO_ClearRecive()
  * Description:  �����������
  * Parameter:    void
  * Return:       void
  * Others:       add by zlk, 2017-06-01
  ******************************************************************************
  */ 
void GSM_IO_ClearRecive(void)
{
    usart2IOInfo.dataLen = 0;

    memset(usart2IOInfo.buf, 0, sizeof(usart2IOInfo.buf));
}


/**
  ******************************************************************************
  * Function:     GSM_SendString()
  * Description:  ���ݷ���
  * Parameter:    USARTx --���ڣ� str --���͵����ݣ� len --���ݳ���
  * Return:       void
  * Others:       add by zlk, 2017-06-01
  ******************************************************************************
  */ 
void GSM_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned char len)
{
    unsigned short count = 0;

    for(; count < len; count++)
    {
        USART_SendData(USARTx, *str++);									//��������
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ��������
    }
}


/**
  ******************************************************************************
  * Function:     GSM_Device_SendCmd()
  * Description:  ��������
  * Parameter:    cmd --�������ݣ�res --�����ַ�����mode --�Ƿ�������ڽ��յ�����
  * Return:       0 --���ͳɹ���1 --����ʧ��
  * Others:       add by zlk, 2017-06-01
  ******************************************************************************
  */ 
uint8 GSM_Device_SendCmd(char *cmd, char *res, _Bool mode)
{
    unsigned char timeout = 300;
    
    GSM_SendString(GSM_IO, (uint8 *)cmd, strlen((uint8 *)cmd));

    while (timeout--) {
        if (GSM_IO_WaitRecive() == REV_OK) {
            if (strstr((const char *)usart2IOInfo.buf, res) != 0) {
                if (mode) {
                    GSM_IO_ClearRecive();
                }
                return 0;
            }
        }

        DelayUs(200);
    }

    return 1;
}


