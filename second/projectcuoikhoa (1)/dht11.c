#include "dht11.h"

// L?y t?n s? ho?t d?ng th?c t? c?a chip t? thu vi?n h? th?ng
extern uint32_t SystemCoreClock; 

// T?o tr? micro-giây chính xác tuy?t d?i b?ng ph?n c?ng SysTick
void DHT11_Delay_us(uint32_t us) {
    SysTick->LOAD = (SystemCoreClock / 1000000) * us - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = 5; // B?t SysTick v?i xung nh?p lõi (Core Clock)
    while (!(SysTick->CTRL & 0x10000)); // Ch? c? d?m d?m ngu?c v? 0
    SysTick->CTRL = 0; // T?t SysTick
}

void DHT11_Delay_ms(uint32_t ms) {
    while(ms--) {
        DHT11_Delay_us(1000);
    }
}

void DHT11_Set_Output(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void DHT11_Set_Input(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // Ch? d? Input kéo lên (Pull-up)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void DHT11_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    DHT11_Set_Output();
    GPIO_SetBits(GPIOA, GPIO_Pin_3); 
    DHT11_Delay_ms(1000); 
}

uint8_t DHT11_Read_Bit(void) {
    uint8_t retry = 0;
    while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == RESET && retry < 100) {
        retry++; DHT11_Delay_us(1);
    }
    
    retry = 0;
    DHT11_Delay_us(40); // Ði?m l?y m?u: Ch? 40us
    
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == SET) {
        while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == SET && retry < 100) {
            retry++; DHT11_Delay_us(1);
        }
        return 1;
    } else {
        return 0; 
    }
}

uint8_t DHT11_Read_Byte(void) {
    uint8_t i, dat = 0;
    for (i = 0; i < 8; i++) {
        dat <<= 1;
        dat |= DHT11_Read_Bit();
    }
    return dat;
}

uint8_t DHT11_Read(uint8_t *temp, uint8_t *humi) {
    uint8_t buf[5];
    uint8_t i, retry = 0;

    DHT11_Set_Output();
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);
    DHT11_Delay_ms(20); // Kéo th?p ít nh?t 18ms (ch?n 20ms cho an toàn)
    GPIO_SetBits(GPIOA, GPIO_Pin_3);
    DHT11_Delay_us(30); 
    
    DHT11_Set_Input();

    // Ch? DHT11 ph?n h?i (Kéo th?p 80us)
    while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == SET && retry < 100) {
        retry++; DHT11_Delay_us(1);
    }
    if(retry >= 100) return 0;

    // DHT11 gi? th?p 80us
    retry = 0;
    while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == RESET && retry < 100) {
        retry++; DHT11_Delay_us(1);
    }
    if(retry >= 100) return 0;
    
    // DHT11 kéo cao 80us chu?n b? g?i d? li?u
    retry = 0;
    while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == SET && retry < 100) {
        retry++; DHT11_Delay_us(1);
    }

    // Ð?c 5 byte
    for(i = 0; i < 5; i++) {
        buf[i] = DHT11_Read_Byte();
    }

    if((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4]) {
        *humi = buf[0]; 
        *temp = buf[2]; 
        return 1;
    }
    return 0; // Sai Checksum
}