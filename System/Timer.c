#include "stm32f10x.h"
#include "bsp_timer.h"
#include "Timer.h"

#define TIMER_PERIOD_MS 10 // 计时周期10ms

static volatile uint32_t s_tick_ms = 0;
static volatile uint8_t s_control_loop_flag = 0;

void Timer_Init(void)
{
    BSP_TIM_InternalClockConfig(TIM2); //内部时钟
    BSP_TIM_TimeBaseInit(TIM2, 7200 - 1, 100 - 1, TIM_CounterMode_Up);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    BSP_TIM_EnableUpdateIRQ(TIM2, TIM2_IRQn, 2, 1);

    TIM_Cmd(TIM2, ENABLE);
}

uint8_t Timer_TakeControlFlag(void) //取标志位，观察是否中断。若中断就清零标志位
{
    uint8_t flag;

    __disable_irq(); //取控制循环标志位时临时关闭中断
    flag = s_control_loop_flag;
    s_control_loop_flag = 0;
    __enable_irq();

    return flag;
}

uint32_t Timer_GetTickMs(void)
{
    return s_tick_ms;
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        s_tick_ms += TIMER_PERIOD_MS;
        s_control_loop_flag = 1;

        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
} // TIM2中断服务函数