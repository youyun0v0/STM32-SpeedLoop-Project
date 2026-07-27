#include "stm32f10x.h"
#include "Serial.h"
#include "Debug.h"

void Debug_Init(void)
{
    Serial_Init();
}

void Debug_Print(const char *message)
{
    Serial_SendString(message);
    Serial_SendString("\r\n");
}

void Debug_PrintTick(uint32_t tick)
{
    Serial_SendString("tick=");
    Serial_SendNumber(tick);
    Serial_SendString("\r\n");
}

void Debug_PrintConfig(void)
{
    Debug_Print("chip=STM32F103C8T6");
    Debug_Print("uart=USART1(PA9)");
    Debug_Print("baud=115200");
    Debug_Print("led=PA0");
}
