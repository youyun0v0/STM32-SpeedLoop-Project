#include "stm32f10x.h"
#include "bsp_gpio.h"
#include "PWM.h"
#include "Motor.h"

void Motor_Init(void)
{
    //PA4和PA5作为电机的方向控制脚
    BSP_GPIO_InitPin(GPIOA, GPIO_Pin_4, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    BSP_GPIO_InitPin(GPIOA, GPIO_Pin_5, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);

    PWM_Init();
    Motor_Stop();
}

void Motor_SetSpeed(int8_t speed)
{
    uint8_t duty;
    //速度限幅：-100 ~ 100
    if (speed > 100)
    {
        speed = 100;
    }
    else if (speed < -100)
    {
        speed = -100;
    }

    if (speed > 0)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_4);
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);
        duty = (uint8_t)speed;
    }
    else if (speed < 0)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);
        GPIO_SetBits(GPIOA, GPIO_Pin_5);
        duty = (uint8_t)(-speed);
    }
    else
    {
        Motor_Stop();
        return;
    }

    PWM_SetDuty(duty);
}

void Motor_Stop(void)
{
    //停止电机：两个方向控制脚都拉低，PWM占空比为0
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);
    PWM_SetDuty(0);
}
