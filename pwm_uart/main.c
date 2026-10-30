#include "stm32f10x.h"

static void Clock_Init(void)
{
    /* Bat HSI 8 MHz */
    RCC->CR |= (1U << 0);
    while ((RCC->CR & (1U << 1)) == 0U)
    {
    }

    /* SYSCLK = HSI */
    RCC->CFGR &= ~(3U << 0);
    while ((RCC->CFGR & (3U << 2)) != 0U)
    {
    }

    /* AHB, APB1, APB2 deu chia 1 */
    RCC->CFGR &= ~((15U << 4) |
                   (7U << 8) |
                   (7U << 11));

    SystemCoreClock = 8000000U;
}

static void GPIO_Init(void)
{
    /* Cap clock AFIO va GPIOA */
    RCC->APB2ENR |= (1U << 0) | (1U << 2);

    /* Khong remap USART1 va TIM2 */
    AFIO->MAPR &= ~((1U << 2) | (3U << 8));

    /*
     * PA0 = TIM2_CH1
     * Alternate function push-pull, output 2 MHz
     * CNF0 = 10, MODE0 = 10 -> 0xA
     */
    GPIOA->CRL &= ~(15U << 0);
    GPIOA->CRL |=  (10U << 0);

    /*
     * PA9 = USART1_TX
     * Alternate function push-pull, output 50 MHz
     */
    GPIOA->CRH &= ~(15U << 4);
    GPIOA->CRH |=  (11U << 4);

    /* PA10 = USART1_RX, input floating */
    GPIOA->CRH &= ~(15U << 8);
    GPIOA->CRH |=  (4U << 8);
}

static void USART1_Init(void)
{
    RCC->APB2ENR |= (1U << 14);

    /* Reset USART1 */
    RCC->APB2RSTR |=  (1U << 14);
    RCC->APB2RSTR &= ~(1U << 14);

    /* 8 data bits, no parity, 1 stop bit, no flow control */
    USART1->CR1 = 0U;
    USART1->CR2 = 0U;
    USART1->CR3 = 0U;

    /* PCLK2 = 8 MHz, baud = 9600 */
    USART1->BRR = 0x0341U;

    /* UE = 1, TE = 1, RE = 1 */
    USART1->CR1 = (1U << 13) |
                  (1U << 3) |
                  (1U << 2);
}

static void USART1_SendChar(char c)
{
    while ((USART1->SR & (1U << 7)) == 0U)
    {
    }

    USART1->DR = (uint8_t)c;
}

static void USART1_SendString(const char *str)
{
    while (*str != '\0')
    {
        USART1_SendChar(*str++);
    }
}

static uint8_t USART1_ReceiveChar(void)
{
    while ((USART1->SR & (1U << 5)) == 0U)
    {
    }

    return (uint8_t)(USART1->DR & 0xFFU);
}

static void USART1_SendPercent(uint16_t value)
{
    if (value == 100U)
    {
        USART1_SendString("100");
    }
    else
    {
        if (value >= 10U)
        {
            USART1_SendChar((char)('0' + value / 10U));
        }

        USART1_SendChar((char)('0' + value % 10U));
    }
}

static void TIM2_PWM_Init(void)
{
    /* Cap clock TIM2 */
    RCC->APB1ENR |= (1U << 0);

    /* Reset TIM2 */
    RCC->APB1RSTR |=  (1U << 0);
    RCC->APB1RSTR &= ~(1U << 0);

    /*
     * APB1 prescaler = 1 -> TIM2 clock = 8 MHz
     *
     * Counter clock = 8 MHz / (PSC + 1)
     *               = 8 MHz / 8 = 1 MHz
     *
     * PWM frequency = 1 MHz / (ARR + 1)
     *               = 1 MHz / 1000 = 1 kHz
     */
    TIM2->PSC = 7U;
    TIM2->ARR = 999U;

    /* Ban dau duty = 0%, LED tat */
    TIM2->CCR1 = 0U;

    /*
     * CC1S  = 00: channel 1 la output
     * OC1M  = 110: PWM mode 1
     * OC1PE = 1: preload CCR1
     */
    TIM2->CCMR1 = (6U << 4) | (1U << 3);

    /*
     * CC1E = 1: bat output channel 1
     * CC1P = 0: active high
     */
    TIM2->CCER = (1U << 0);

    /*
     * ARPE = 1: preload ARR
     * DIR = 0: dem len
     * CMS = 00: edge-aligned
     */
    TIM2->CR1 = (1U << 7);

    /* Nap PSC, ARR va CCR1 vao thanh ghi hoat dong */
    TIM2->EGR = (1U << 0);

    /* Xoa co update do lenh UG tao ra */
    TIM2->SR = 0U;

    /* CEN = 1: bat timer */
    TIM2->CR1 |= (1U << 0);
}

static void LED_SetBrightness(uint16_t percent)
{
    /*
     * Duty = CCR1 / (ARR + 1)
     * ARR + 1 = 1000
     *
     * 0%   -> CCR1 = 0
     * 50%  -> CCR1 = 500
     * 100% -> CCR1 = 1000
     */
    TIM2->CCR1 = percent * 10U;
}

int main(void)
{
    uint8_t c;
    uint16_t value = 0U;
    uint8_t hasDigit = 0U;
    uint8_t invalid = 0U;

    Clock_Init();
    GPIO_Init();
    TIM2_PWM_Init();
    USART1_Init();

    USART1_SendString("PWM ready. Send 0..100 followed by #\r\n");

    while (1)
    {
        c = USART1_ReceiveChar();

        if ((c == '#') || (c == '\r') || (c == '\n'))
        {
            /* Bo qua lenh rong, vi du LF sau CR */
            if ((hasDigit != 0U) || (invalid != 0U))
            {
                if ((invalid == 0U) && (hasDigit != 0U))
                {
                    LED_SetBrightness(value);

                    USART1_SendString("LED = ");
                    USART1_SendPercent(value);
                    USART1_SendString("%\r\n");
                }
                else
                {
                    USART1_SendString("ERROR: use 0..100#\r\n");
                }
            }

            /* Chuan bi nhan lenh tiep theo */
            value = 0U;
            hasDigit = 0U;
            invalid = 0U;
        }
        else if ((c >= '0') && (c <= '9'))
        {
            hasDigit = 1U;

            if (invalid == 0U)
            {
                value = value * 10U + (uint16_t)(c - '0');

                /* Ngung tich luy neu vuot 100, tranh tran so */
                if (value > 100U)
                {
                    invalid = 1U;
                }
            }
        }
        else
        {
            invalid = 1U;
        }
    }
}