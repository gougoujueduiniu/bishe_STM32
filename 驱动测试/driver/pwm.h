#ifndef __PWM_H__
#define __PWM_H__

#include "stm32f10x.h"
#include "delay.h"


#define OUT1_ON GPIOB->BSRR = GPIO_Pin_5;
#define OUT2_ON GPIOB->BSRR = GPIO_Pin_4;
#define OUT3_ON GPIOB->BSRR = GPIO_Pin_8;
#define OUT4_ON GPIOB->BSRR = GPIO_Pin_7;
#define OUT1_OFF GPIOB->BRR = GPIO_Pin_5;
#define OUT2_OFF GPIOB->BRR = GPIO_Pin_4;
#define OUT3_OFF GPIOB->BRR = GPIO_Pin_8;
#define OUT4_OFF GPIOB->BRR = GPIO_Pin_7;

void Driver_Init(u16 per,u16 pre);
void Go_Ahead(void);
void Back(void);
void Stop(void);
void Turn_Right(u16 pwm_left,u16 pwm_right);
void Turn_Left(u16 pwm_left,u16 pwm_right);
void SET_PWM(u16 pwm_left,u16 pwm_right);
void Clockwise_Rotation(void);
void Counterclockwise_Rotation(void);
void Turn90(char chazhi,char direnction);
void Turn180(void);
#endif

