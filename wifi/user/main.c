#include "stm32f10x.h"
#include "uart2.h"
#include "string.h"
#include "delay.h"
#include "usart.h"

char wifiname[]="AT+CWJAP_DEF=\"5113\",\"zxc511511\"\n";
char cmd[]="AT\n";
unsigned int cnt = 0;
char recv[1024] = {0};
char data;

void send_ch(u8 ch)
{
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART2->DR = ch;
	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET );
}

void SendCmd1()
{
	u8 i,len;
	len = strlen(wifiname);
    while(1)
    {
        memset(recv,0,1024);
				cnt = 0;
        for(i=0;i<len;i++)
				{
					send_ch(wifiname[i]);
				}
        if((NULL != strstr(recv, "OK")))	//判断是否有预期的结果
        {
            break;
        }
        else
        {
            delay_ms(100);
        }
    }
				delay_ms(1000);
		delay_ms(1000);
}

void SendCmd2()
{
	u8 i,len;
	len = strlen(cmd);
    while(1)
    {
        memset(recv,0,8);
				cnt = 0;
        for(i=0;i<len;i++)
				{
					send_ch(cmd[i]);
				}
				delay_ms(1000);
				printf("jieshoudao:");
				printf("%s\n",recv);
        if((NULL != strstr(recv, "OK")))	//判断是否有预期的结果
        {
            break;
        }
//        else
//        {
//            delay_ms(1000);
//        }
    }
		delay_ms(1000);
		delay_ms(1000);
}


void wifi_init()
{
	SendCmd1();
	SendCmd2();
	
}

int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	delay_init(72);
	uart_init();
	USART2_Config();
//	wifi_init();
	
	while(1)
	{
		SendCmd2();
	}
}

void USART2_IRQHandler()
{
	if(USART2->SR & USART_FLAG_RXNE)   //Receive Data Reg Full Flag
  {
		data = USART2->DR;
		recv[cnt++] = data;
		if(cnt>8)
		{
			cnt = 0;
		}
	}
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
}
