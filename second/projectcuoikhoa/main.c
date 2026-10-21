#include "stm32f10x.h"
#include <stdio.h>    // Thu vi?n h? tr? hàm sprintf ghép chu?i
#include "led.h"
#include "button.h"
#include "buzzer.h"
#include "uart.h"
#include "dht11.h"

void delay(uint32_t time) {
    while(time--);
}

int main() {
    LED_init();
    Button_init();
    Buzzer_Init();
    UART1_init();
    DHT11_Init();
    
    GPIO_ResetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_7); 
    Buzzer_Off();
    
    
    delay(500000);
    UART1_sendString("STM32 Khoi dong!\r\n");
    
    uint8_t count = 0;
    uint32_t dht_timer = 0; 
    char buffer[64];        
    uint8_t nhiet_do, do_am;
    
    while(1) {
        // ==========================================
        // 1. NHÁNH QUÉT NÚT B?M (Không dùng delay)
        // ==========================================
        if (Button_isclicked() == 1) {
            count++;
            Buzzer_Beep();
            
            
            if (count > 2) {
                count = 0;
            }

           
            sprintf(buffer, "--> Chuyen sang trang thai: %d\r\n", count);
            UART1_sendString(buffer);
            
            
            if (count == 1) {
                LED_on4();
                LED_off7();
            } 
            else if (count == 2) {
                LED_on7();
                LED_off4();
                dht_timer = 0; 
            } 
            else {
                GPIO_ResetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_7);
            }
        }
        
        // ==========================================
        // 2. NHÁNH Ð?C DHT11 (Ch? ch?y khi count == 2)
        // ==========================================
        // ==========================================
        // 2. NHÁNH Ð?C DHT11 (Ch? ch?y khi count == 2)
        // ==========================================
        if (count == 2) {
            dht_timer++;
            
            if (dht_timer > 800000) {
                dht_timer = 0; 
                
                if (DHT11_Read(&nhiet_do, &do_am) == 1) {
                    uint8_t warning = 0;
                    
                    
                    if (nhiet_do > 35 || do_am > 90) {
                        warning = 1;
                        Buzzer_On(); 
                    } else {
                        warning = 0;
                        Buzzer_Off(); 
                    }
                    

                    sprintf(buffer, "{\"btn\":%d,\"t\":%d,\"h\":%d,\"w\":%d}\n", count, nhiet_do, do_am, warning);
                    UART1_sendString(buffer);
                }
            }
        } else {
           
            Buzzer_Off(); 
        }
    }
}