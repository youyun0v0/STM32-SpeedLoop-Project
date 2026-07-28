#include "stm32f10x.h"
#include "bsp_gpio.h"
#include "Debug.h"
#include "Timer.h"

int main(void)
{
    uint32_t control_count = 0;

    BSP_GPIO_InitPin(GPIOA, GPIO_Pin_0, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    Debug_Init();
    Timer_Init();

    Debug_Print("SpeedLoop Project Week10 Start!");
    Debug_PrintConfig();

    while (1)
    {
        if (Timer_TakeControlFlag()) //每当10ms中断来临
        {
            control_count++;

            if ((control_count % 50) == 0) //相当于实现delay 500ms
            {
                if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0) == Bit_SET)
                {
                    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
                }
                else
                {
                    GPIO_SetBits(GPIOA, GPIO_Pin_0);
                }
            }//翻转LED显示

            if ((control_count % 100) == 0) //每秒打印一次时间
            {
                Debug_PrintTick(Timer_GetTickMs()); //打印内容不要放在中断服务函数里面，否则会很卡
            }
        }
    }
}