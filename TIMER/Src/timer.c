#include "timer.h"
/*===================IP_Timer========================*/
volatile uint32_t timer_cnt = 0U;

void TIM1_UP_IRQHandler(void)
{
    if (TIM1->SR & (1 << 0))
    {
        TIM1->SR &= ~(1 << 0);   // xoa co update interrupt
        timer_cnt++; // moi lan ARR dem tran thi tang timer_cnt len 1
    }
}
/**
  * @brief  Cau hinh bo dinh thoi nang cao (Advanced Timer).
  * @param  TIMx: Con tro toi bo Timer can cau hinh
  * @retval None
  */
void IP_Timer_Config_Advance(TIM_TypeDef *TIMx)
{
    if (TIMx == TIM1) {
        RCC->APB2ENR |= (1 << 11);   // bat clock TIM1 - Tclock = 72Mhz
    }
    else if (TIMx == TIM8) {
        RCC->APB2ENR |= (1 << 13);   // bat clock TIM8 - Tclock = 72Mhz
    }
    else {
        return;
    }

    // moi 1us thi cap nhat ngat 1 lan
    TIMx->PSC = 35;
    TIMx->ARR = 1;

    TIMx->CR1 |= (1 << 7);           // ARPE
    TIMx->DIER |= (1 << 0);          // bat update interrupt

    TIMx->EGR |= (1 << 0);           // nap ngay PSC/ARR
    TIMx->SR &= ~(1 << 0);           // xoa co UIF

    if (TIMx == TIM1) {
        NVIC_EnableIRQ(TIM1_UP_IRQn);
    }

    TIMx->CR1 |= (1 << 0);           // bat timer
}

void IP_delay_us(uint32_t time)
{
    uint32_t start = timer_cnt;
    while ((timer_cnt - start) < time);
}
/**
  * @brief  Tao ham tre theo don vi mili giay.
  * @param  ms: Thoi gian tre (ms)
  * @retval None
  */
void IP_delay(uint32_t ms)
{
    IP_delay_us(ms * 1000);
}
/*===================IP_Timer========================*/