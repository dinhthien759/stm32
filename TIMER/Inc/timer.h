#ifndef IP_TIMER_H
#define IP_TIMER_H
#include "stm32f10x.h"
#include "stdint.h"

void IP_delay(uint32_t ms);
void IP_Timer_Config_Advance(TIM_TypeDef *TIMx);
void IP_delay_us(uint32_t time);
#endif