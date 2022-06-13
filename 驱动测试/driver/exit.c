#include "exit.h"

void Exit_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	NVIC_InitTypeDef NVIC_Initstruct;
	
	
	NVIC_Initstruct.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Initstruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Initstruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Initstruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_Initstruct);
		
	NVIC_Initstruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_Init(&NVIC_Initstruct);
	//	EXTI_Initstruct.EXTI_Line = EXTI_Line0;
	
	EXTI_InitTypeDef EXTI_Initstruct;
	
	EXTI_Initstruct.EXTI_LineCmd = ENABLE;
	EXTI_Initstruct.EXTI_Line = EXTI_Line1;
	EXTI_Initstruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Initstruct.EXTI_Mode = EXTI_Mode_Interrupt;

	EXTI_Init(&EXTI_Initstruct);
	
	EXTI_Initstruct.EXTI_Line = EXTI_Line0;
	EXTI_Init(&EXTI_Initstruct);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	GPIO_InitTypeDef GPIO_Initstruct;

	GPIO_Initstruct.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_0;
//	GPIO_Initstruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_Initstruct);
	
	GPIO_Initstruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_Initstruct);
	
}

