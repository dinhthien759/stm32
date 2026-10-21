#include "buzzer.h"

// Hàm delay n?i b? cho còi
static void Buzzer_Delay(__IO uint32_t nCount) {
    while(nCount--) {}
}

void Buzzer_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // C?p clock cho Port B
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Cau hình chân PB0 làm Output
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Buzzer_On(void) {
    
    GPIO_SetBits(GPIOA, GPIO_Pin_9); 
}

void Buzzer_Off(void) {
    GPIO_ResetBits(GPIOA, GPIO_Pin_9);
}

void Buzzer_Beep(void) {
    Buzzer_On();
    Buzzer_Delay(100000); 
    Buzzer_Off();
}