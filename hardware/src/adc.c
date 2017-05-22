/**
  ******************************************************************************
  * @FileName:     adc.c
  * @Author:       zlk
  * @Version:      V1.0
  * @Date:         2017-5-22 14:17:28
  * @Description:  This file provides all the adc.c functions.
  ******************************************************************************
  */


#include "adc.h"


/**
  ******************************************************************************
  * Function:     ADC_ChInit()
  * Description:  ��ʼ��
  * Parameter:    ����
  * Return:       ����ֵ
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
void ADC_ChInit(ADC_TypeDef * ADCx, _Bool temp)
{
    ADC_InitTypeDef adcInitStruct;

    if(ADCx == ADC1)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    else if(ADCx == ADC2)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    else
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6); //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

    ADC_DeInit(ADCx); //��λADCx,������ ADCx ��ȫ���Ĵ�������Ϊȱʡֵ

    adcInitStruct.ADC_ContinuousConvMode = DISABLE; //ģ��ת�������ڵ���ת��ģʽ
    adcInitStruct.ADC_DataAlign = ADC_DataAlign_Right; //ADC�����Ҷ���
    adcInitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //ת��������������ⲿ��������
    adcInitStruct.ADC_Mode = ADC_Mode_Independent; //ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
    adcInitStruct.ADC_NbrOfChannel = 1; //˳����й���ת����ADCͨ������Ŀ
    adcInitStruct.ADC_ScanConvMode = DISABLE; //ģ��ת�������ڵ�ͨ��ģʽ
    ADC_Init(ADCx, &adcInitStruct);	//����adcInitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���

    if(ADCx == ADC1 && temp)
        ADC_TempSensorVrefintCmd(ENABLE); //�����ڲ��¶ȴ�����//ADC1ͨ��16

    ADC_Cmd(ADCx, ENABLE); //ʹ��ָ����ADC1

    ADC_ResetCalibration(ADCx);	//ʹ�ܸ�λУ׼

    while(ADC_GetResetCalibrationStatus(ADCx));	//�ȴ���λУ׼����

    ADC_StartCalibration(ADCx); //����ADУ׼

    while(ADC_GetCalibrationStatus(ADCx)); //�ȴ�У׼����
}

/**
  ******************************************************************************
  * Function:     ADC_GetValue()
  * Description:  ��ȡԭʼAD����
  * Parameter:    ADCx --adc�� ch --ͨ��
  * Return:       �������һ��ADC1�������ת�����
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
unsigned short ADC_GetValue(ADC_TypeDef * ADCx, unsigned char ch)
{
    //����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
    ADC_RegularChannelConfig(ADCx, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����

    ADC_SoftwareStartConvCmd(ADCx, ENABLE);		//ʹ��ָ����ADC1�����ת����������

    while(!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC )); //�ȴ�ת������

    return ADC_GetConversionValue(ADCx);	//�������һ��ADC1�������ת�����
}

/**
  ******************************************************************************
  * Function:     ADC_GetValueTimes()
  * Description:  ��ȡԭʼAD����ƽ��ֵ
  * Parameter:    ADCx --adc�� ch --ͨ���� times --����
  * Return:       ƽ��ֵ
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
float ADC_GetValueTimes(ADC_TypeDef * ADCx, unsigned char ch, unsigned char times)
{
    float adcValue = 0;
    unsigned char i = 0;

    for(; i < times; i++)
    {
        adcValue += (float)ADC_GetValue(ADC1, ch);
    }

    return adcValue / times;
}

/**
  ******************************************************************************
  * Function:     ADC_GetTemperature()
  * Description:  ��ȡ�¶�
  * Parameter:    void
  * Return:       �¶�ֵ
  * Others:       add by zlk, 2017-05-22
  ******************************************************************************
  */ 
float ADC_GetTemperature(void)
{
    float temp = ADC_GetValueTimes(ADC1, ADC_Channel_16, 10); //��ȡԭʼAD����

    temp = temp * 3.3 / 4096; //ת��Ϊ��ѹֵ

    return (1.43 - temp) / 0.0043 + 25; //�������ǰ�¶�ֵ
}
