#ifndef DHT11_H
#define DHT11_H

#include "stm32f10x.h"

// Kh?i t?o chân PA3 cho DHT11
void DHT11_Init(void);

// Hàm d?c d? li?u. Tr? v? 1 n?u thành công, 0 n?u l?i (h?ng c?m bi?n/sai checksum)
uint8_t DHT11_Read(uint8_t *temp, uint8_t *humi);

#endif