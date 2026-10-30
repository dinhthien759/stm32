void uart_config(void){
    RCC -> APB2ENR |= 1 << 14 | 1 << 2; 
    
    GPIOA -> CRH &= ~(0xF <<4); 
    GPIOA -> CRH |= 0xB << 4; 

    GPIOA -> CRH &= ~(0xF <<8); 
    GPIOA -> CRH |= 0x8 << 8; 
    
    USART1 -> BRR = 7500U; 
    USART1 -> CR1 |= 1 <<2 | 1 << 3; 
    USART1 -> CR1 |= 1 << 13; 

    
}
void uartsend(uint8_t data){
    while(USART1 -> SR & (1<<7) == 0){}
    USART1 -> DR = data; 
}
uint8_t uartRead(void){
    if(USART1 -> SR & (1<<5))
        return USART1 -> DR; 
    return 0; 
}