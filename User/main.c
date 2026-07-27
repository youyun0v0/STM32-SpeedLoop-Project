#include "stm32f10x.h"
#include "bsp_gpio.h"
#include "Delay.h"
#include "Debug.h"

int main(void)
{
    uint32_t tick = 0;

    BSP_GPIO_InitPin(GPIOA, GPIO_Pin_0, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    Debug_Init();

    Debug_Print("SpeedLoop Project Start!");

    while (1)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_0);
        Debug_PrintTick(tick);
        tick++;
        Delay_ms(500);
        GPIO_SetBits(GPIOA, GPIO_Pin_0);
        Delay_ms(500);
    }
}