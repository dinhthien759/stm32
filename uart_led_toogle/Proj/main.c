#include "stm32f10x.h"

static void Clock_Init(void)
{
    /* Bat HSI (RC noi 8 MHz) */
    RCC->CR |= (1U << 0);                 /* HSION = 1 */
    while ((RCC->CR & (1U << 1)) == 0U)   /* Cho HSIRDY */
    {
    }

    /* Chon HSI lam SYSCLK: SW[1:0] = 00 */
    RCC->CFGR &= ~(3U << 0);

    /* Cho xac nhan SYSCLK dang dung HSI: SWS[1:0] = 00 */
    while ((RCC->CFGR & (3U << 2)) != 0U)
    {
    }

    /*
     * AHB  = SYSCLK / 1 = 8 MHz
     * APB1 = HCLK   / 1 = 8 MHz
     * APB2 = HCLK   / 1 = 8 MHz
     */
    RCC->CFGR &= ~((15U << 4) |
                   (7U << 8) |
                   (7U << 11));

    SystemCoreClock = 8000000U;
}

static void GPIO_Init(void)
{
    /* Cap clock cho GPIOA: IOPAEN = 1 */
    RCC->APB2ENR |= (1U << 2);

    /* Dat output PA0 = 0 truoc khi cau hinh output */
    GPIOA->BRR = (1U << 0);

    /*
     * PA0: output push-pull, 2 MHz
     * CNF0  = 00
     * MODE0 = 10
     */
    GPIOA->CRL &= ~(15U << 0);
    GPIOA->CRL |=  (2U << 0);

    /*
     * PA9: USART1_TX, alternate function push-pull, 50 MHz
     * CNF9  = 10
     * MODE9 = 11
     * Nibble PA9 = 0xB
     */
    GPIOA->CRH &= ~(15U << 4);
    GPIOA->CRH |=  (11U << 4);

    /*
     * PA10: USART1_RX, input floating
     * CNF10  = 01
     * MODE10 = 00
     * Nibble PA10 = 0x4
     */
    GPIOA->CRH &= ~(15U << 8);
    GPIOA->CRH |=  (4U << 8);
}

static void USART1_Init(void)
{
    /* Cap clock USART1: USART1EN = 1 */
    RCC->APB2ENR |= (1U << 14);

    /* Reset USART1 ve trang thai ban dau */
    RCC->APB2RSTR |=  (1U << 14);
    RCC->APB2RSTR &= ~(1U << 14);

    /*
     * CR1: M = 0 -> 8 data bits
     *      PCE = 0 -> khong parity
     * CR2: STOP = 00 -> 1 stop bit
     * CR3: khong hardware flow control
     */
    USART1->CR1 = 0U;
    USART1->CR2 = 0U;
    USART1->CR3 = 0U;

    /*
     * USART1 dung PCLK2 = 8 MHz.
     * BRR = round(8000000 / 9600) = 833 = 0x0341
     */
    USART1->BRR = 0x0341U;

    /*
     * RE = bit 2: bat nhan
     * TE = bit 3: bat truyen
     * UE = bit 13: bat USART
     */
    USART1->CR1 = (1U << 2) |
                  (1U << 3) |
                  (1U << 13);
}

static void USART1_SendChar(char c)
{
    /* Cho TXE = 1: thanh ghi du lieu truyen dang trong */
    while ((USART1->SR & (1U << 7)) == 0U)
    {
    }

    USART1->DR = (uint8_t)c;
}

static void USART1_SendString(const char *str)
{
    while (*str != '\0')
    {
        USART1_SendChar(*str);
        str++;
    }
}

static uint8_t USART1_ReceiveChar(void)
{
    /* Cho RXNE = 1: da nhan duoc du lieu */
    while ((USART1->SR & (1U << 5)) == 0U)
    {
    }

    /* Doc DR de lay du lieu va xoa RXNE */
    return (uint8_t)(USART1->DR & 0xFFU);
}

int main(void)
{
    uint8_t command;

    Clock_Init();
    GPIO_Init();
    USART1_Init();

    USART1_SendString("Ready: 1 = ON, 0 = OFF\r\n");

    while (1)
    {
        command = USART1_ReceiveChar();

        if (command == '1')
        {
            /* BSRR bit 0: set PA0 = 1 */
            GPIOA->BSRR = (1U << 0);
            USART1_SendString("LED ON\r\n");
        }
        else if (command == '0')
        {
            /* BRR bit 0: reset PA0 = 0 */
            GPIOA->BRR = (1U << 0);
            USART1_SendString("LED OFF\r\n");
        }
        /* Ky tu khac, ke ca CR/LF, duoc bo qua */
    }
}