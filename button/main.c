#include <stm32f10x.h>

/* Delay bang SysTick, khong can ngat */
void delay_ms(uint32_t time){
	while(time){
		SysTick -> CTRL=5; 
		SysTick -> VAL = 0; 
		SysTick -> LOAD = 72000-1; 
		while(!(SysTick -> CTRL & (1<<16))); 
		time--; 
	}
}

int main(void)
{
    uint8_t led_on = 0U;

    SystemCoreClockUpdate();

    /* Bat clock GPIOA va GPIOC */
    RCC->APB2ENR |= (1U << 2) | (1U << 4);

    /* PC13: LED tat ban dau (active LOW) */
    GPIOC->BSRR = (1U << 13);

    /* PC13: output push-pull, 2 MHz */
    GPIOC->CRH &= ~(0xFU << 20);
    GPIOC->CRH |=  (0x2U << 20);

    /* PA0: input pull-up */
    GPIOA->BSRR = (1U << 0);
    GPIOA->CRL &= ~(0xFU << 0);
    GPIOA->CRL |=  (0x8U << 0);

    while (1)
    {
        if ((GPIOA->IDR & (1U << 0)) == 0U)
        {
            /* Chong doi khi nhan */
            delay_ms(20);

            if ((GPIOA->IDR & (1U << 0)) == 0U)
            {
                /* Dao trang thai LED */
                led_on ^= 1U;

                if (led_on)
                {
                    GPIOC->BRR = (1U << 13);  /* Sang */
                }
                else
                {
                    GPIOC->BSRR = (1U << 13); /* Tat */
                }

                /* Cho tha nut on dinh moi nhan lan tiep */
                do
                {
                    while ((GPIOA->IDR & (1U << 0)) == 0U)
                    {
                    }

                    delay_ms(20);
                }
                while ((GPIOA->IDR & (1U << 0)) == 0U);
            }
        }
    }
}