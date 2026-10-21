#include "led.h"


void LED_init()
{
	GPIO_InitTypeDef x;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	x.GPIO_Pin = GPIO_Pin_4 |  GPIO_Pin_7 ;
	x.GPIO_Mode = GPIO_Mode_Out_PP;
	x.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&x);
	// set up xong cac chuc nang cua gpio cho vi dieu khien 
}
void LED_on4()
{
	GPIO_SetBits( GPIOA ,GPIO_Pin_4); // bat led o gpio pa4 
	
	
}
void LED_on7()
{
	GPIO_SetBits( GPIOA ,GPIO_Pin_7); // bat led o gpio pa7
	
	
}
void LED_off4()
{
	GPIO_ResetBits( GPIOA, GPIO_Pin_4);  // tat led pa4
}

void LED_off7()
{
	GPIO_ResetBits( GPIOA, GPIO_Pin_7);  // tat led pa7
}