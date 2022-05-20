#ifndef __delay_h__
#define __delay_h__
#include "stm32f10x.h"

void delay_init(uint8_t SYSCLK);
void delay_ms(uint16_t nus);
void delay_us(uint32_t nms);

#endif
