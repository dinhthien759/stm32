#include "button.h"
static void Button_Delay(__IO uint32_t nCount) {
    while(nCount--) {}
}
 void Button_init()
{
	GPIO_InitTypeDef y;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	y.GPIO_Mode = GPIO_Mode_IPU;
	y.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA,&y);
	
	
	
}
uint8_t Button_isclicked()
{
if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == Bit_RESET)
	{
		Button_Delay(200);
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == Bit_RESET) {
            
            while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == Bit_RESET);
            return 1; 
        }
    }
    return 0;
	}





