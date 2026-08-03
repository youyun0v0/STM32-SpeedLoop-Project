#include "stm32f10x.h"
#include "bsp_gpio.h"
#include "bsp_timer.h"
#include "PWM.h"

#define PWM_TIMER TIM3  //使用的定时器
#define PWM_PERIOD_COUNTS 1000  //PWM周期计数值

static uint8_t s_duty_percent = 0; //当前占空比百分比

void PWM_LED_Init(void)
{
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /*
     * TIM2的时基已经由Timer_Init()配置为10ms周期。
     * PA0是TIM2_CH1，这里只配置输出比较通道，不修改TIM2的PSC和ARR。
     */
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    //PA0实际连接的LED为高电平点亮：高电平持续时间就是LED亮度占空比
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    BSP_GPIO_InitPin(GPIOA, GPIO_Pin_0, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
}

void PWM_SetLEDDuty(uint8_t duty_percent)
{
    uint16_t period_counts;
    uint16_t compare;

    if (duty_percent > 100)
    {
        duty_percent = 100;
    }

    period_counts = (uint16_t)(TIM2->ARR + 1);
    compare = (uint16_t)(((uint32_t)period_counts * duty_percent) / 100);
    TIM_SetCompare1(TIM2, compare);
}

void PWM_Init(void)
{
    TIM_OCInitTypeDef TIM_OCInitStructure;

    //配置时基单元
    BSP_TIM_InternalClockConfig(PWM_TIMER);
    BSP_TIM_TimeBaseInit(PWM_TIMER, 72 - 1, PWM_PERIOD_COUNTS - 1, TIM_CounterMode_Up);

    //配置输出比较单元
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//CNT<CCR时输出有效电平
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高电平
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//使能输出
    TIM_OCInitStructure.TIM_Pulse = 0; //初始占空比为0%
    TIM_OC1Init(PWM_TIMER, &TIM_OCInitStructure);

    BSP_GPIO_InitPin(GPIOA, GPIO_Pin_6, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);

    TIM_Cmd(PWM_TIMER, ENABLE);
}

void PWM_SetDuty(uint8_t duty_percent)
{
    uint16_t compare;

    if (duty_percent > 100)
    {
        duty_percent = 100;
    } //限幅

    s_duty_percent = duty_percent;
    compare = (uint16_t)duty_percent * 10;

    TIM_SetCompare1(PWM_TIMER, compare); //设置CCR为compare
}

uint8_t PWM_GetDuty(void)
{
    return s_duty_percent;
}

void PWM_SetCompare1(uint16_t Compare)
{
    TIM_SetCompare1(PWM_TIMER, Compare);
}

void PWM_SetCompare2(uint16_t Compare)
{
    TIM_SetCompare2(PWM_TIMER, Compare);
}

void PWM_SetCompare3(uint16_t Compare)
{
    TIM_SetCompare3(PWM_TIMER, Compare);
}

void PWM_SetPSC(uint16_t PSC)
{
    TIM_PrescalerConfig(PWM_TIMER, PSC, TIM_PSCReloadMode_Immediate);
}
