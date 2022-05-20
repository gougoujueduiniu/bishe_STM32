#include "pwm.h"


void SET_PWM(u16 pwm_left,u16 pwm_right)
{
	TIM4->CCR1 = pwm_right;
	TIM4->CCR4 = pwm_left;
}


void Go_Ahead()
{
	OUT1_ON;
	OUT2_OFF;
	OUT3_ON;
	OUT4_OFF;
	SET_PWM(350,350);
}

void Back()
{
	OUT1_OFF;
	OUT2_ON;
	OUT3_OFF;
	OUT4_ON;
	SET_PWM(200,200);
}

void Stop()
{
	OUT1_OFF;
	OUT2_OFF;
	OUT3_OFF;
	OUT4_OFF;	
	SET_PWM(0,0);
}

void Turn_Right(u16 pwm_left,u16 pwm_right)
{
//	OUT1_ON;
//	OUT2_OFF;
//	OUT3_ON;
//	OUT4_OFF;
	OUT1_ON;
	OUT2_OFF;
	OUT3_OFF;
	OUT4_ON;
	SET_PWM(pwm_left,pwm_right);
}

void Turn_Left(u16 pwm_left,u16 pwm_right)
{
//	OUT1_ON;
//	OUT2_OFF;
//	OUT3_ON;
//	OUT4_OFF;
	OUT1_OFF;
	OUT2_ON;
	OUT3_ON;
	OUT4_OFF;	
	SET_PWM(pwm_left,pwm_right);
}


//顺时针旋转
void Clockwise_Rotation()     
{
	OUT1_ON;
	OUT2_OFF;
	OUT3_OFF;
	OUT4_ON;
	SET_PWM(200,200);
}


//逆时针旋转
void Counterclockwise_Rotation() 
{
	OUT1_OFF;
	OUT2_ON;
	OUT3_ON;
	OUT4_OFF;
	SET_PWM(200,200);
}


void zhuanxiang()
{
	Go_Ahead();
	delay_ms(100);
	Stop();
	delay_ms(100);
	Turn_Left(150,150);
}

void Driver_Init(u16 per,u16 pre)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	//关闭PB4的JTRST复用
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST,ENABLE);
	

	GPIO_InitTypeDef GPIO_initstruct;	
	GPIO_initstruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_initstruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_initstruct);

	GPIO_initstruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9;
	GPIO_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_initstruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB,&GPIO_initstruct);

	TIM_TimeBaseInitTypeDef TIM_TimeBasestruct;	
	TIM_TimeBasestruct.TIM_ClockDivision = TIM_CKD_DIV1;         //设置时钟分割
	TIM_TimeBasestruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBasestruct.TIM_Period = per;                //设置重装载值
	TIM_TimeBasestruct.TIM_Prescaler = pre;            //设置时钟预分配系数

	TIM_TimeBaseInit(TIM4,&TIM_TimeBasestruct);

	TIM_OCInitTypeDef TIM_ocinitstruct;
	TIM_ocinitstruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_ocinitstruct.TIM_Pulse = 500;
	TIM_ocinitstruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_ocinitstruct.TIM_OutputState = TIM_OutputState_Enable;

	TIM_OC1Init(TIM4,&TIM_ocinitstruct);
	TIM_OC4Init(TIM4,&TIM_ocinitstruct);

  TIM_ARRPreloadConfig(TIM4,DISABLE);
	TIM_Cmd(TIM4,ENABLE);
	TIM_CtrlPWMOutputs(TIM4, ENABLE);

	Stop();

}


