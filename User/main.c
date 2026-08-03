#include "stm32f10x.h"
#include "bsp_gpio.h"
#include "Debug.h"
#include "Timer.h"
#include "Motor.h"
#include "PWM.h"

#define STATUS_LED_DUTY 5

int main(void)
{
    uint32_t control_count = 0;
    uint8_t status_led_on = 0;

    Debug_Init();
    Timer_Init();
    PWM_LED_Init();
    Motor_Init();

    Debug_Print("SpeedLoop Project Week11 Motor Test Start!");
    Debug_PrintConfig();
    Debug_Print("motor pwm=PA6");
    Debug_Print("motor dir=PA4/PA5");

    while (1)
    {
        if (Timer_TakeControlFlag())
        {
            control_count++;

            if ((control_count % 50) == 0)
            {
                if (status_led_on)
                {
                    PWM_SetLEDDuty(0);
                    status_led_on = 0;
                }
                else
                {
                    PWM_SetLEDDuty(STATUS_LED_DUTY);
                    status_led_on = 1;
                }
            }

            /*
             * 6秒循环测试：
             * 0~1秒停止，1~3秒正转，3~4秒停止，4~6秒反转。
             * 不需要按复位键，逻辑分析仪随时开始采样都能等到下一轮波形。
             */
            if (control_count == 100)
            {
                Debug_Print("motor forward speed=60");
                Motor_SetSpeed(60);
            }
            else if (control_count == 300)
            {
                Debug_Print("motor stop");
                Motor_Stop();
            }
            else if (control_count == 400)
            {
                Debug_Print("motor reverse speed=-60");
                Motor_SetSpeed(-60);
            }
            else if (control_count == 600)
            {
                Debug_Print("motor stop, next cycle");
                Motor_Stop();
                control_count = 0;
            }
        }
    }
}
