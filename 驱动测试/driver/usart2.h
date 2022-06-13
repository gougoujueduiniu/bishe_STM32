#ifndef USART2_H_H
#define USART2_H_H

#include <stm32f10x.h>
#include <stdint.h>
#define MAX_RCV_LEN  1024

extern unsigned char flag;
extern unsigned char yaohe;

extern void USART2_Config(void);
extern void USART2_Write(USART_TypeDef* USARTx, uint8_t *Data,uint8_t len);
extern void USART2_Clear(void);
extern volatile unsigned char  gprs_ready_flag;
extern volatile unsigned char  gprs_ready_count;
extern uint32_t USART2_GetRcvNum(void);

extern unsigned char  usart2_rcv_buf[MAX_RCV_LEN];
extern volatile unsigned int   usart2_rcv_len;
void SendCmd(char* cmd, char* result, int timeOut);
void SendData(char *buf);

void  USART2_GetRcvData(uint8_t *buf, uint32_t rcv_len);
#endif

