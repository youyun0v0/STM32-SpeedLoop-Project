#ifndef __DEBUG_H
#define __DEBUG_H

#include "stm32f10x.h"

void Debug_Init(void);
void Debug_Print(const char *message); //快捷打印信息
void Debug_PrintTick(uint32_t tick); //程序是否卡死
void Debug_PrintConfig(void);
void Debug_PrintDuty(uint8_t duty_percent);

#endif
