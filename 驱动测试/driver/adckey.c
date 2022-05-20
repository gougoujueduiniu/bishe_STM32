#include "adckey.h"



void GPIOinit()
{
	GPIO_InitTypeDef GPIO_Initstruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Initstruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Initstruct);
}


void Adc_Init()
{
	ADC_InitTypeDef ADC_Initstruct;
	ADC_DeInit(ADC1);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	
	
	ADC_Initstruct.ADC_Mode = ADC_Mode_Independent;
	ADC_Initstruct.ADC_ContinuousConvMode = DISABLE;
	ADC_Initstruct.ADC_DataAlign =  ADC_DataAlign_Right;
	ADC_Initstruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_Initstruct.ADC_NbrOfChannel = 1;
	ADC_Initstruct.ADC_ScanConvMode = DISABLE;
	
	ADC_Init(ADC1,&ADC_Initstruct);
	
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);
	
//	ADC_ResetCalibration(ADC1);
//  /* Check the end of ADC1 reset calibration register */
//  while(ADC_GetResetCalibrationStatus(ADC1));

//  /* Start ADC1 calibaration */
//  ADC_StartCalibration(ADC1);
//  /* Check the end of ADC1 calibration */
//  while(ADC_GetCalibrationStatus(ADC1));
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	ADC_StartCalibration(ADC1);	 //开启AD校准
 	while(ADC_GetCalibrationStatus(ADC1));
	
	
//	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	
}


void AdcKey_Init()
{
	GPIOinit();
	Adc_Init();
}

uint16_t GET_ADConverter()
{
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
	return (uint16_t)ADC1->DR;
}

