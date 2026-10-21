#include "stm32f10x.h"

void USART1_Init(void);
void USART1_SendChar(char c);
void USART1_SendString(char *str);

int main(void)
{
    USART1_Init();

    while (1)
    {
        USART1_SendString("Hello STM32!\r\n");

        for (volatile int i = 0; i < 1000000; i++);
    }
}