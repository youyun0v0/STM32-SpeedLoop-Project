#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

//配置：TIM3_CH1 / PA6    PWM 频率：1kHz

void PWM_Init(void);
void PWM_SetDuty(uint8_t duty_percent); //设置占空比
uint8_t PWM_GetDuty(void); //查询占空比

//PA0 LED：复用TIM2_CH1的10ms周期，高电平点亮
//必须先调用Timer_Init()，再调用PWM_LED_Init()
void PWM_LED_Init(void);
void PWM_SetLEDDuty(uint8_t duty_percent);

void PWM_SetCompare1(uint16_t Compare);
void PWM_SetCompare2(uint16_t Compare);
void PWM_SetCompare3(uint16_t Compare);
void PWM_SetPSC(uint16_t PSC);

#endif
