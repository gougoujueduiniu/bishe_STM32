#include "timer.h"

void Time_Init()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;	 //����һ���ṹ�������������ʼ��GPIO

	NVIC_InitTypeDef NVIC_InitStructure;

	/* ������ʱ��3ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	//���TIMx���жϴ�����λ:TIM �ж�Դ
	TIM_TimeBaseInitStructure.TIM_Period = 1000;//�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = 35999;//����������ΪTIMxʱ��Ƶ��Ԥ��Ƶֵ��100Khz����Ƶ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);	
	 //ʹ�ܻ���ʧ��TIMx����
	/* �����жϲ����������ж� */
	
	/* ����NVIC���� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��TIM3_IRQn��ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;	//��ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;  //��Ӧ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//ʹ��
	NVIC_Init(&NVIC_InitStructure);	
	
	TIM_Cmd(TIM3,ENABLE);
	
}

