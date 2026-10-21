/*
 * STM32F103C8T6 - LED sang dan / tat dan (breathing) bang PWM
 * Muc thanh ghi, Keil uVision (Device: STM32F103C8, CMSIS + Startup)
 *
 * Phan cung: LED + dien tro 330R noi tu PA6 xuong GND (PA6 = TIM3_CH1)
 * Luu y: LED PC13 tren Blue Pill KHONG co kenh timer nen khong dung duoc cho PWM.
 *
 * Clock : HSE 8MHz x PLL9 = 72MHz, APB1 = 36MHz => clock TIM3 = 72MHz
 * PWM   : PSC = 71 -> 1MHz ; ARR = 999 -> tan so PWM = 1kHz, do phan giai 1000 muc
 */

#include "stm32f10x.h"

#define PWM_MAX   999u

static volatile uint32_t msTicks = 0;

void SysTick_Handler(void)
{
    msTicks++;
}

static void delay_ms(uint32_t ms)
{
    uint32_t start = msTicks;
    while ((msTicks - start) < ms);
}

/* ---------- Clock 72MHz tu HSE ---------- */
static void Clock_Init(void)
{
    RCC->CR |= RCC_CR_HSEON;                       /* bat HSE */
    while (!(RCC->CR & RCC_CR_HSERDY));

    FLASH->ACR |= FLASH_ACR_PRFTBE;                /* prefetch buffer */
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;             /* 2 wait state cho 72MHz */

    RCC->CFGR |= RCC_CFGR_PLLSRC;                  /* nguon PLL = HSE */
    RCC->CFGR |= RCC_CFGR_PLLMULL9;                /* x9 = 72MHz */
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;              /* APB1 = 36MHz (toi da) */

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;                  /* chon PLL lam SYSCLK */
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

/* ---------- GPIO PA6: Alternate function push-pull, 50MHz ---------- */
static void GPIO_Init_PA6(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;            /* clock GPIOA */

    GPIOA->CRL &= ~(0xFu << 24);                   /* xoa MODE6[1:0], CNF6[1:0] */
    GPIOA->CRL |=  (0xBu << 24);                   /* CNF=10 (AF PP), MODE=11 (50MHz) */
}

/* ---------- TIM3 CH1 PWM mode 1 ---------- */
static void TIM3_PWM_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;            /* clock TIM3 */

    TIM3->PSC  = 72 - 1;                           /* 72MHz / 72 = 1MHz */
    TIM3->ARR  = PWM_MAX;                          /* 1MHz / 1000 = 1kHz */
    TIM3->CCR1 = 0;                                /* duty ban dau = 0% */

    TIM3->CCMR1 &= ~(TIM_CCMR1_OC1M | TIM_CCMR1_CC1S);
    TIM3->CCMR1 |= (6u << 4);                      /* OC1M = 110: PWM mode 1 */
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;                /* preload CCR1 */

    TIM3->CCER |= TIM_CCER_CC1E;                   /* cho phep xuat kenh 1 ra chan */

    TIM3->CR1  |= TIM_CR1_ARPE;                    /* preload ARR */
    TIM3->EGR  |= TIM_EGR_UG;                      /* nap lai cac thanh ghi shadow */
    TIM3->CR1  |= TIM_CR1_CEN;                     /* chay timer */
}

int main(void)
{
    uint32_t duty;

    Clock_Init();
    SysTick_Config(72000);                         /* ngat moi 1ms */
    GPIO_Init_PA6();
    TIM3_PWM_Init();

    while (1)
    {
        /* Sang dan: 0 -> 100% */
        for (duty = 0; duty <= PWM_MAX; duty++)
        {
            TIM3->CCR1 = duty;
            delay_ms(2);
        }

        /* Tat dan: 100% -> 0 */
        for (duty = PWM_MAX; duty > 0; duty--)
        {
            TIM3->CCR1 = duty;
            delay_ms(2);
        }
    }
}