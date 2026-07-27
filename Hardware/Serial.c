#include "stm32f10x.h"
#include "Serial.h"
#include "bsp_gpio.h"

void Serial_Init(void)
{
    USART_InitTypeDef USART_InitStructure;

    /* 1. 配置 PA9 为 USART1_TX。
       GPIO_Mode_AF_PP 表示复用推挽输出：PA9 的输出由 USART1 外设接管。 */
    BSP_GPIO_InitPin(GPIOA, GPIO_Pin_9, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);

    /* 2. 打开 USART1 外设时钟。
       GPIOA 的时钟已经由 BSP_GPIO_InitPin() 打开，这里只需要打开 USART1。 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* 3. 配置串口参数。
       这些参数必须和电脑串口助手保持一致：115200, 8N1, no flow control。 */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位：8
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位：1
    USART_InitStructure.USART_Parity = USART_Parity_No; //无校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Tx; //仅发送
    USART_Init(USART1, &USART_InitStructure);

    /* 4. 使能 USART1。
       没有这一步，前面的配置只是写好了参数，外设还不会真正工作。 */
    USART_Cmd(USART1, ENABLE);
}

void Serial_SendChar(char ch)
{
    USART_SendData(USART1, (uint8_t)ch); //把字符交给USART1发送
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }//等待发送数据寄存器为空，可以继续写入下一个字符
}

void Serial_SendString(const char *str)
{
    while (*str != '\0')
    {
        Serial_SendChar(*str);
        str++;
    }//一个一个发
}

void Serial_SendNumber(uint32_t number)
{
    char buffer[11];
    int index = 0;

    if (number == 0)
    {
        Serial_SendChar('0');
        return;
    }

    while (number > 0 && index < 10)
    {
        buffer[index] = (char)('0' + number % 10);
        number /= 10;
        index++;
    }

    while (index > 0)
    {
        index--;
        Serial_SendChar(buffer[index]);
    }
} //数字转字符串