#include "stm32f10x.h"
#include "delay.h"
#include "pwm.h"
#include "xunji.h"
#include "mlx90614.h"
#include "head.h"
#include "oled.h"
#include "adckey.h"
#include "exit.h" 
#include "usart2.h"
#include "utils.h"
#include "stdio.h"
#include "timer.h"
/*
四个按键对应ADC值
2048    0x800
2729    0xaa9
3042    0xc00
3275    0xccb
*/
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
  return ch;
}

char direction_flag;   //朝向
char place[2];				//当前小车坐标
//char order_direction;
char last_state;
char car_tmp;
uint16_t adc_num;
static unsigned char scream_flag = 0,FSC_Offset,SSC_Offset,TSC_Offset;
float temp;
u8 mima[5] = {2,4,6,8};
char ditu[][2]={{1,1},{1,2},{2,0},{2,2}};
char buff[64];

void run(char x,char y)
{
	Go_Ahead();
	while(1)
	{
		if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))
		{
			Stop();
			continue;
		}
		else
		{
			Go_Ahead();
		}
		car_tmp = Read_Xunji();
//*****************检测到交叉口后系统运行流程**********************
//1.根据当前车的朝向给对应的横纵坐标加减
//2.判断横纵坐标是否到达要求，未到达则继续前进，达到目标要求转向
//3.转向多少由对应坐标相减（可采用switch case判断）	
//*****************************************************************
		if(car_tmp == 0x07)
		{
			Go_Ahead();
			delay_ms(100);
			Stop();
			
			switch (direction_flag)
			{	
				case EAST:  place[0]++;break;
				case NORTH: place[1]++;break;
				case WEST:  place[0]--;break;
				case SOUTH: place[1]--;break;
			}
			OLED_ClearArea(0,48,128,64);
			OLED_ShowChar(32,48,place[0]+48,16);
			OLED_ShowChar(64,48,place[1]+48,16);
			OLED_Refresh();
			if(place[0]==x)
			{
				if(place[1]==y)
				{
					Stop();
					break;
				}
				else if(direction_flag%2==1)  
				{
					//执行转向函数
					Turn90(y-place[1],direction_flag);
				}
			}
			else if(place[1]==y)
			{
				if(place[0]==x)
				{
					Stop();
					break;
				}
				else if(direction_flag%2==0)
				{
					Turn90(x-place[0],direction_flag);
				}
			}
			Go_Ahead();
		}		
		else if(car_tmp == 0) //
		{
			if(last_state == ZUOYI)
			{
				SET_PWM(150,600);
			}
			else if(last_state == YOUYI)
			{
				SET_PWM(600,150);
			}
			else if(last_state == ZHONGJIAN)
			{
				Go_Ahead();
			}
		}
		else if(car_tmp == ZUOYI)
		{
			SET_PWM(350,550);
			last_state = ZUOYI;
		}
		else if(car_tmp == YOUYI)
		{
			SET_PWM(550,350);
			last_state = YOUYI;
		}
		else if(car_tmp == YOUER)
		{
			SET_PWM(450,350);
		}
		else if(car_tmp == ZUOER)
		{
			SET_PWM(350,450);
		}
		else if(car_tmp == ZHONGJIAN)
		{
			Go_Ahead();
			last_state = ZHIXING;
		}
	}
}


void Back_Door()
{
			if(place[0] == -1 && place[1]==0)
			{
				return;
			}
			if(place[1]!=0)
			{
				if(direction_flag%2==1)
				{
					Turn90(-1,direction_flag);
				}
				else if(direction_flag==NORTH)
				{
					Turn180();
				}
				run(place[0],0);	
			}
			if(direction_flag%2==0)
			{
				Turn90(-1,direction_flag);
			}
			else if(direction_flag==EAST)
			{
				Turn180();
			}	
			run(-1,0);
			Turn180();
			Stop();
}



void OpenBox()
{
	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
		{
			delay_ms(20);
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
			{
				SendData("ok");
				return;
			}
		}
	}
	
	return;
}

u8 VerifyPassword(u8 *password)
{
	char buf[64];
	u8 i=0;
	sprintf(buf,"#B%s",password);
	USART2_Clear();
	flag=0;
	SendData(buf);
	while(!flag);
	delay_ms(20);
	
	while(i<3)
	{
		if(strstr(usart2_rcv_buf,"#H")!=NULL)
		{
			return 0;
		}
		else if(strstr(usart2_rcv_buf,"#F")!=NULL)
		{
			return 1;
		}
		i++;
		OLED_ShowString(16,32,"verifing",16);
		OLED_ShowNum(88,32,i,1,16);
		OLED_Refresh();
		delay_ms(1000);
		OLED_ClearArea(0,32,128,48);
		OLED_Refresh();
	}
	
	return 0;
}


void Show_Num()
{
	unsigned char i = 0;
	for(i=0;i<10;i++)
	{
		OLED_ShowChar(10+i%5*24,i/5*16,0x30+i,16);
	}
	OLED_ShowChinese(16,48,4,16);
	OLED_ShowChinese(32,48,5,16);
	OLED_ShowChinese(80,48,6,16);
	OLED_ShowChinese(96,48,7,16);
}

void Show_FirstScream()
{
	OLED_ShowChinese(16,24,0,16);
	OLED_ShowChinese(16+16,24,1,16);
	OLED_ShowChinese(48+32,24,2,16);
	OLED_ShowChinese(64+32,24,3,16);
	while(1)
	{	
		adc_num = GET_ADConverter();
		if(adc_num - 0x900 < 0)
		{
			Clear_guangbiao(8+(FSC_Offset%2)*64,25);
			FSC_Offset -= 1;
			delay_ms(250);
		}	
		else if(adc_num - 0xac0 < 0)
		{
			Clear_guangbiao(8+(FSC_Offset%2)*64,25);
			FSC_Offset += 1;
			delay_ms(250);	
		}
		else if(adc_num - 0xcab > 0 && adc_num < 0xf00)
		{
			OLED_Clear();
			scream_flag = 1;
			SSC_Offset = 0;
			OLED_Refresh();
			return;
		}	
		Show_guangbiao(8+(FSC_Offset%2)*64,25);
		OLED_Refresh();
	}
}

void Show_thirdscream()
{
	OLED_ShowChinese(24,16,12,16);  //感
	OLED_ShowChinese(40,16,13,16);	//冒
	OLED_ShowChinese(88,16,14,16);	//创
	OLED_ShowChinese(104,16,15,16);	//伤
	OLED_ShowChinese(24,32,16,16);	//消
	OLED_ShowChinese(40,32,17,16);	//化
	OLED_ShowChinese(88,32,18,16);	//中
	OLED_ShowChinese(104,32,19,16);	//药
	
	while(1)
	{	
		adc_num = GET_ADConverter();
		if(adc_num - 0x900 < 0)   //左移
		{
			Clear_guangbiao(16+(TSC_Offset%2)*64,18+TSC_Offset/2*16);
			if(TSC_Offset==0)
			{
				TSC_Offset = 3;
			}
			else
			{
				TSC_Offset--;
			}
			delay_ms(250);
		}	
		else if(adc_num - 0xac0 < 0) // 右移
		{
			Clear_guangbiao(16+(TSC_Offset%2)*64,18+TSC_Offset/2*16);
			if(TSC_Offset==3)
			{
				TSC_Offset=0;
			}
			else
			{
				TSC_Offset++;
			}
			delay_ms(250);	
		}
		else if(adc_num - 0xba0 > 0 && adc_num < 0xc40) //返回
		{
			OLED_Clear();
			TSC_Offset = 0;
			while(GET_ADConverter()>0xf00);
			Back_Door();
			return;
		}
		else if(adc_num - 0xcab > 0 && adc_num < 0xf00) //确认
		{
			while(GET_ADConverter()>0xf00);
			delay_ms(1000);
			delay_ms(1000);
			if(ditu[TSC_Offset][0]==place[0] && ditu[TSC_Offset][1]==place[1])
			{
				continue;
			}
			if(direction_flag%2==1)
			{
				if((ditu[TSC_Offset][0]-place[0])*(direction_flag-2)<0)
				{
					run(ditu[TSC_Offset][0],ditu[TSC_Offset][1]);
				}
				else if((ditu[TSC_Offset][0]-place[0])*(direction_flag-2)>=0 && ditu[TSC_Offset][1]-place[1]!=0)
				{
					Turn90(ditu[TSC_Offset][1]-place[1],direction_flag);
					run(ditu[TSC_Offset][0],ditu[TSC_Offset][1]);
				}
				else
				{
					Turn180();
					run(ditu[TSC_Offset][0],ditu[TSC_Offset][1]);
				}
			}
			else
			{
				if((ditu[TSC_Offset][1]-place[1])*(direction_flag-3)>0)
				{
					run(ditu[TSC_Offset][0],ditu[TSC_Offset][1]);
				}
				else if((ditu[TSC_Offset][1]-place[1])*(direction_flag-3)<=0 && ditu[TSC_Offset][0]-place[0]!=0)
				{
					Turn90(ditu[TSC_Offset][0]-place[0],direction_flag);
					run(ditu[TSC_Offset][0],ditu[TSC_Offset][1]);
				}
				else
				{
					Turn180();
					run(ditu[TSC_Offset][0],ditu[TSC_Offset][1]);
				}
			}
		}	
		Show_guangbiao(16+(TSC_Offset%2)*64,18+TSC_Offset/2*16);
		OLED_Refresh();
	}
}



void Show_SecondScream()
{
	u8 offsetx[]={2,26,50,74,98,2,26,50,74,98,8,72};
	u8 offsety[]={2,18,49};
	u8 password[5] = {0};
	u8 i = 0;
	u8 tempbuffer[8];
	
	if(FSC_Offset%2 == 0)
	{
		//显示测温界面
		temp = SMBus_ReadTemp();
		OLED_Clear();
		if(temp > 33.0)
		{
			sprintf(tempbuffer,"%.1f",temp+0.5);
			OLED_ShowChinese(32,16,8,16);
			OLED_ShowChinese(48,16,9,16);
			OLED_ShowChinese(64,16,10,16);
			OLED_ShowChinese(80,16,11,16);
			OLED_ShowString(48,32,tempbuffer,16);
			OLED_Refresh();
			delay_ms(1000);
			OLED_Clear();
			if(temp<37.0)
			{			
				TSC_Offset = 0;
				Show_thirdscream();
			}
			else
			{
				OLED_ShowString(24,32,"warning",16);
				delay_ms(1000);
				OLED_Clear();
			}
			scream_flag = 0;
			FSC_Offset = 0;
		}	
	}
	else if(FSC_Offset%2 == 1)
	{
		//显示识别码输入界面
		while(GET_ADConverter()<0xf00);
		Show_Num();
		while(1)
		{
			adc_num = GET_ADConverter();
			if(adc_num - 0x900 < 0)
			{
				Clear_guangbiao(offsetx[SSC_Offset],offsety[SSC_Offset/5]);
				if(SSC_Offset == 0)
				{
					SSC_Offset = 11;
				}
				else
				{
					SSC_Offset--;
				}
				delay_ms(250);
			}	
			else if(adc_num - 0xac0 < 0)
			{
				Clear_guangbiao(offsetx[SSC_Offset],offsety[SSC_Offset/5]);
				if(SSC_Offset == 11)
				{
					SSC_Offset = 0;
				}
				else
				{
					SSC_Offset++;
				}
				delay_ms(250);	
			}
			else if(adc_num - 0xcab > 0 && adc_num < 0xf00)  // 确认键
			{
				if(SSC_Offset<10)
				{
					if(i < 4)
					{
						OLED_ShowChar(i*9,32,SSC_Offset+0x30,16);	
						OLED_Refresh();
						password[i] = SSC_Offset+48;
						i++;
					}
				}
				else if(SSC_Offset==10)
				{
					scream_flag = 0;
					FSC_Offset = 0;
					SSC_Offset = 0;
					while(GET_ADConverter()<0xf00);
					OLED_Clear();					
					return;
				}
				else if(SSC_Offset==11)
				{
					OLED_ClearArea(0,32,128,48);
					if(VerifyPassword(password))
					{
						scream_flag = 0;
						FSC_Offset = 0;
						SSC_Offset = 0;						
						OLED_ShowString(32,32,"Ture",16);
						OLED_Refresh();
						OpenBox();
						delay_ms(1000);
						OLED_Clear();
						return;
					}
					else
					{
						OLED_ShowString(24,32,"False",16);
						OLED_Refresh();
						delay_ms(1000);
						OLED_ClearArea(0,32,128,48);
						memset(password,0,5);
						i=0;
					}
				}
				while(GET_ADConverter()<0xf00);
			}
			else if(adc_num - 0xb6a && adc_num < 0xc64)  // 返回键
			{
				if(i>0)
				{
					OLED_ClearArea((i-1)*9,32,(i+1)*9,48);
					OLED_Refresh();
					i--;
				}
				delay_ms(500);
			}	
			Show_guangbiao(offsetx[SSC_Offset],offsety[SSC_Offset/5]);
			OLED_Refresh();
		}
	}
}


void Esp8266_Init()
{
	SendCmd("AT\r\n","\0",3000);
	GPIOC->ODR ^= (uint16_t)1<<13;
	delay_ms(1000);
	GPIOC->ODR ^= (uint16_t)1<<13;
	SendCmd("AT+CWMODE=1\r\n","\0",3000);
	SendCmd("AT+CWJAP_DEF=\"5113\",\"zxc511511\"\r\n","\0",3000);
	OLED_ShowString(0,0,"WIFI",16);
	OLED_ShowString(0,16,"CONNECTED",16);
	OLED_Refresh();
	SendCmd("AT+CIPMUX=0\r\n","\0",3000);
	SendCmd(IP_PORT,"OK",3000);
	SendData("#1");
	OLED_ShowString(0,32,"SERVER",16);
	OLED_ShowString(0,48,"CONNECTED",16);	
	OLED_Refresh();
	GPIOC->ODR ^= (uint16_t)1<<13;
	delay_ms(500);
	GPIOC->ODR ^= (uint16_t)1<<13;
	delay_ms(500);
	GPIOC->ODR ^= (uint16_t)1<<13;
	
//	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );	//使能或者失能指定的TIM中断
}


int main()
{
	delay_init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	Exit_Init();
	Xunji_Init();
	Driver_Init(999,71);  // 999 71
	Time_Init();
	SMBus_Init();
	AdcKey_Init();	
	USART2_Config();	
	OLED_Init();
	OLED_ColorTurn(0);//0正常显示，1 反色显示
  OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	direction_flag = 1;
	Esp8266_Init();
	place[0] = -1;
	place[1] = 0;
	GPIOC->BSRR = GPIO_Pin_13;
	delay_ms(1000);
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);
	OLED_Clear();
	while(1)
	{
		switch (scream_flag)
		{
			case 0: Show_FirstScream();break;
			case 1: Show_SecondScream();break;
		}
//		delay_ms(1000);
//		OLED_Refresh();		
	}
}


//药盒检测中断函数


void TIM3_IRQHandler()	 //定时器3中断函数
{
	char buf[8];
	
	if(flag == 1)
	{
		USART2_GetRcvData(buff,strlen(usart2_rcv_buf));
		flag = 0;
		USART2_Clear();
		SendData(buff);
		memset(buff,0,64);
	}
	else
	{
		sprintf(buf,"#C(%d,%d)",place[0],place[1]);
		USART2_Clear();
		SendData(buf);
	}

	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}


//药盒检测函数
void EXTI0_IRQHandler()
{
	delay_ms(20);
	if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
	{
		if(yaohe == 1)
		{
			SendData("ok");
			yaohe = 0;
		}
	}
	
	EXTI_ClearITPendingBit(EXTI_Line0);
}


//避障中断
//void EXTI1_IRQHandler()
//{
//	delay_ms(20);
//	if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))
//	{
//		Stop();
//		while(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1));
//	}
//	
//	EXTI_ClearITPendingBit(EXTI_Line1);
//}

