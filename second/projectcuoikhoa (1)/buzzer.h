#ifndef BUZZER_H
#define BUZZER_H

#include "stm32f10x.h"

void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);


void Buzzer_Beep(void); 

#endif