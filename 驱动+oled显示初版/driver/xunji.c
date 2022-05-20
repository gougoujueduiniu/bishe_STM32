#include "xunji.h"


void Xunji_Init()
{
	GPIO_InitTypeDef GPIO_Initstruct;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO ,ENABLE);

// 

////  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
//	
	GPIO_Initstruct.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOA,&GPIO_Initstruct);
	
	GPIO_Initstruct.GPIO_Pin = GPIO_Pin_12;
//	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_Initstruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_Initstruct);
//	GPIOA->ODR = (uint16_t)0x0000;
}



char Read_Xunji()
{
	uint16_t tmp;
	char ch;
	tmp = GPIOA->IDR;
	tmp &= 0x0700;
	ch = tmp>>8;
//	ch = tmp ^ 0x1f;
	
	return ch;
}

