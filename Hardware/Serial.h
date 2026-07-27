#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"

//硬件接线：PA9 -> RXD    GND -> GND
void Serial_Init(void); //初始化USART1 TX
void Serial_SendChar(char ch); //发送一个字符
void Serial_SendString(const char *str); //发送一个字符串
void Serial_SendNumber(uint32_t number); //发送一个无符号整数

#endif
