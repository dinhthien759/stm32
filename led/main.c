#include <stm32f10x.h>

#define DATA_PIN    (1U << 7)  // PA7 -> chan 14 (DS)
#define CLOCK_PIN   (1U << 5)  // PA5 -> chan 11 (SH_CP)
#define LATCH_PIN   (1U << 4)  // PA4 -> chan 12 (ST_CP)

static void HC595_Init(void)
{
    // Bat clock GPIOA
    RCC->APB2ENR |= (1U << 2);

    // DATA, CLOCK, LATCH ban dau o muc 0
    GPIOA->BRR = DATA_PIN | CLOCK_PIN | LATCH_PIN;

    // PA4, PA5, PA7: output push-pull, 2 MHz
    GPIOA->CRL &= ~((0xFU << 16) |
                    (0xFU << 20) |
                    (0xFU << 28));

    GPIOA->CRL |=  ((0x2U << 16) |
                    (0x2U << 20) |
                    (0x2U << 28));
}

static void HC595_Write(uint8_t data)
{
    uint32_t i;

    GPIOA->BRR = LATCH_PIN | CLOCK_PIN;
    for (i = 0; i < 8U; i++)
    {
        // Gui bit cao nhat truoc
        if ((data & 0x80U) != 0U)
        {
            GPIOA->BSRR = DATA_PIN;
        }
        else
        {
            GPIOA->BRR = DATA_PIN;
        }

        // Canh len CLOCK: dich 1 bit vao IC
        GPIOA->BSRR = CLOCK_PIN;
        GPIOA->BRR = CLOCK_PIN;
        data = (uint8_t)(data << 1);
    }
    // Canh len LATCH: cap nhat dong thoi 8 ngo ra
    GPIOA->BSRR = LATCH_PIN;
    GPIOA->BRR = LATCH_PIN;
}

void delay(uint32_t time){
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
    uint32_t i;

    SystemCoreClockUpdate();
    HC595_Init();
    HC595_Write(0x00);

    while (1)
    {
        for (i = 0; i < 8U; i++)
        {
            HC595_Write((uint8_t)(1U << i));
            delay(500);
        }
    }
}