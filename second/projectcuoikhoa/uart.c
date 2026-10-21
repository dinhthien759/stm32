#include "uart.h"

void UART1_init(void) {
    GPIO_InitTypeDef x;
    USART_InitTypeDef y;
    
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    
   
    x.GPIO_Pin = GPIO_Pin_10;
    x.GPIO_Mode = GPIO_Mode_AF_PP;
    x.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &x);
    

    x.GPIO_Pin = GPIO_Pin_11;
    x.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    x.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &x);
    
    y.USART_BaudRate = 115200;
    y.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    y.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    y.USART_Parity = USART_Parity_No;
    y.USART_StopBits = USART_StopBits_1;
    y.USART_WordLength = USART_WordLength_8b;
    
    USART_Init(USART3, &y);
    USART_Cmd(USART3, ENABLE);
}

void UART1_sendChar(char c) {
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    USART_SendData(USART3, (uint16_t)c);
}

void UART1_sendString(char* str) {
    while (*str) {
        UART1_sendChar(*str++);
    }
}

char UART1_nhan(void) {

    if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == SET) {
        return (char)USART_ReceiveData(USART3);
    }
    return 0; 
}