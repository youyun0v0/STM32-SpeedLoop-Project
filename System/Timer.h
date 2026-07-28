#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

void Timer_Init(void);
uint8_t Timer_TakeControlFlag(void);
uint32_t Timer_GetTickMs(void);

#endif