#include "stm32f10x.h"
#include "timer.h"
int main()
{
	RCC -> APB2ENR |= 1 << 4;
	IP_Timer_Config_Advance(TIM1);
	GPIOC -> CRH |= 1 << 20;
	while(1)
	{
		GPIOC -> BSRR |= 1 << 29;
		IP_delay(2000);
		GPIOC -> BSRR |= 1 << 13;
		IP_delay(2000);
	}
}
