#ifndef UART_H
#define UART_H

#include "stm32f10x.h"

// Khai báo dúng ch? hoa ch? thu?ng
void UART1_init(void);
void UART1_sendChar(char c);
void UART1_sendString(char* str);
char UART1_nhan(void);

#endif