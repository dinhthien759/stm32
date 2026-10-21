#include <stm32f10x.h>

void delay(uint32_t time){
	while(time){
		SysTick -> CTRL=5; 
		SysTick -> VAL = 0; 
		SysTick -> LOAD = 72000-1; 
		while(!(SysTick -> CTRL & (1<<16))); 
		time--; 
	}
}

int main(){
	RCC-> APB2ENR |=1<<4;
	GPIOC -> CRH &= ~(0xFU << 20); 
	GPIOC ->CRH |= 0x3U << 20; 
	
	while(1){
		// su dung odr de set bit 
	//	GPIOC -> ODR |=1<<0; 
		//su dung bsrr de set bit 
	//	GPIOC -> BSRR |=1 << 13; 
		// su dung brr de reset bit 
	//	GPIOC ->BRR |=1 <<13; 
		
		
		GPIOC -> BSRR |= 1<<29; //on pc13
		delay(500); 
		GPIOC -> BSRR |= 1<<13; //off pc13
		delay(500); 
	}
	
	
}