# STM32 SpeedLoop Project

本项目是第 9-16 周 STM32 速度闭环阶段的主工程。当前存档节点为：**第 10 周完成，固定周期控制循环已验证**。

项目目标不是重新堆外设 demo，而是在同一个 `SpeedLoop_Project` 中逐步完成：

```text
工程恢复
  -> GPIO / UART 最小调试链路
  -> TIM2 固定周期任务
  -> PWM 输出
  -> 编码器速度反馈
  -> 串口命令
  -> PID 速度闭环
  -> 日志与曲线分析
```

---

## 当前完成状态

### 第 9 周：工程恢复、GPIO 与 UART

已完成：

- 从 `5-2_DMA+ADC_Multi-Channel` 复制并确定 `SpeedLoop_Project` 为第 9-16 周主工程。
- `PA0` 运行指示灯可以周期闪烁。
- `USART1 TX / PA9` 可以通过 USB-TTL 输出到 VSCode 串口监视器。
- 新增 `Hardware/Serial.c/.h`，封装串口发送。
- 新增 `App/Debug.c/.h`，封装调试输出。
- 串口可输出启动信息和 `tick`。

### 第 10 周：定时器与固定周期控制循环

已完成：

- 将 `System/Timer.c/.h` 改造为 `TIM2` 内部时钟固定周期模块。
- `TIM2` 每 `10 ms` 产生一次更新中断。
- 中断中只更新时间和置位周期任务标志。
- `main.c` 通过 `Timer_TakeControlFlag()` 执行周期任务。
- `PA0` 每 `500 ms` 翻转一次。
- 串口每 `1 s` 输出一次运行时间。
- 使用 USB 逻辑分析仪 + PulseView 验证 `PA0` 完整周期约 `1 s`。

当前第 10 周达标结论：

```text
TIM2 10ms 固定周期任务工作正常。
PA0 每 500ms 翻转一次，完整周期约 1s。
串口 tick 按 1000ms 递增。
```

---

## 硬件与工具链

| 项目 | 当前内容 |
|---|---|
| 主控 | STM32F103C8T6 |
| 库类型 | STM32F10x Standard Peripheral Library |
| 工具链 | VSCode EIDE / Keil 工程 |
| 主工程 | `SpeedLoop_Project` |
| 下载方式 | ST-Link，当前可正常下载运行 |
| 串口模块 | USB-SERIAL CH340 / USB-TTL |
| 串口监视器 | VSCode 串口监视器 |
| 逻辑分析仪 | USB 逻辑分析仪，PulseView |

---

## 当前接线

### 运行灯

| 功能 | STM32 引脚 | 说明 |
|---|---|---|
| 运行指示 LED | `PA0` | 当前运行灯，Week10 用于周期验证 |
| LED GND | `GND` | 必须共地 |

### USART1 TX

| STM32 | USB-TTL / CH340 | 作用 |
|---|---|---|
| `PA9 / USART1_TX` | `RXD` | STM32 发，USB-TTL 收 |
| `GND` | `GND` | 共地 |

串口参数：

```text
baudrate: 115200
data bits: 8
stop bits: 1
parity: none
flow control: none
mode: Tx only
```

### 逻辑分析仪验证接线

| 逻辑分析仪 | STM32 | 作用 |
|---|---|---|
| `D0 / CH0` | `PA0` | 采集运行灯电平 |
| `GND` | `GND` | 共地 |

本次只测数字电平，不接逻辑分析仪 `VCC`。

---

## 外设配置

| 外设 | 引脚/通道 | 参数 | 用途 |
|---|---|---|---|
| GPIOA | `PA0` | `GPIO_Mode_Out_PP`, `50MHz` | 运行灯 / 周期验证 |
| USART1 TX | `PA9` | `GPIO_Mode_AF_PP`, `115200 8N1` | Debug 串口输出 |
| TIM2 | 内部时钟 | `PSC=7200-1`, `ARR=100-1` | 10ms 固定周期中断 |
| NVIC | `TIM2_IRQn` | 抢占优先级 2，子优先级 1 | 允许 TIM2 更新中断 |

TIM2 周期计算：

```text
72,000,000 / 7200 / 100 = 100 Hz
1 / 100 Hz = 10 ms
```

---

## 当前代码结构

当前重点文件：

```text
User/
  main.c

App/
  Debug.h
  Debug.c

Hardware/
  Serial.h
  Serial.c

BSP/
  bsp_gpio.h
  bsp_gpio.c
  bsp_timer.h
  bsp_timer.c

System/
  Timer.h
  Timer.c
  Delay.h
  Delay.c
```

当前工程中已有但尚未作为主线完成验证的模块：

```text
Hardware/
  PWM.c/.h
  Encoder.c/.h
  Motor.c/.h
  Servo.c/.h
  OLED.c/.h
  ADC.c/.h

System/
  MyDMA.c/.h
```

注意：`Hardware/LED.c/.h` 默认控制 `PA1/PA2`，而当前运行灯使用 `PA0`。

---

## 模块职责

### `main.c`

负责主流程：

- 初始化 `PA0` 运行灯。
- 初始化 `Debug` 串口输出。
- 初始化 `TIM2` 固定周期模块。
- 在主循环中等待 `Timer_TakeControlFlag()`。
- 每 500ms 翻转一次 `PA0`。
- 每 1s 输出一次 `tick_ms`。

当前主循环核心逻辑：

```text
while (1)
  -> Timer_TakeControlFlag()
  -> control_count++
  -> 每 50 次翻转 PA0
  -> 每 100 次打印 tick_ms
```

### `Timer`

文件：

```text
System/Timer.h
System/Timer.c
```

职责：

- 使用 `TIM2` 建立 `10 ms` 固定周期。
- 在 `TIM2_IRQHandler()` 中累加 `s_tick_ms`。
- 在 `TIM2_IRQHandler()` 中置位 `s_control_loop_flag`。
- 向 `main.c` 提供周期任务入口。

当前接口：

```c
void Timer_Init(void);
uint8_t Timer_TakeControlFlag(void);
uint32_t Timer_GetTickMs(void);
```

设计原则：

```text
中断里只做很少的事。
复杂逻辑和串口打印放在 main.c 主循环里。
```

### `Serial`

文件：

```text
Hardware/Serial.h
Hardware/Serial.c
```

职责：

- 初始化 `USART1 TX / PA9`。
- 发送单个字符。
- 发送字符串。
- 发送无符号整数。

当前接口：

```c
void Serial_Init(void);
void Serial_SendChar(char ch);
void Serial_SendString(const char *str);
void Serial_SendNumber(uint32_t number);
```

### `Debug`

文件：

```text
App/Debug.h
App/Debug.c
```

职责：

- 封装调试输出格式。
- 避免 `main.c` 里散落底层串口发送细节。

当前接口：

```c
void Debug_Init(void);
void Debug_Print(const char *message);
void Debug_PrintTick(uint32_t tick);
void Debug_PrintConfig(void);
```

---

## 当前串口输出样例

Week10 程序启动后，VSCode 串口监视器中可见：

```text
SpeedLoop Project Week10 Start!
chip=STM32F103C8T6
uart=USART1(PA9)
baud=115200
led=PA0
tick=1000
tick=2000
tick=3000
tick=4000
tick=5000
```

说明：

- 程序已经进入 Week10 版本。
- `Timer_GetTickMs()` 正常按 10ms 累加。
- 串口打印频率约为 1Hz。
- 主循环没有卡死。

---

## 逻辑分析仪验证记录

工具：

```text
USB 逻辑分析仪 + PulseView
```

接线：

```text
D0  -> PA0
GND -> STM32 GND
```

采样设置：

```text
Sample count: 100 k samples
Sample rate: 20 kHz
Capture time: 5 s
```

代码条件：

```text
TIM2 周期：10 ms
PA0 翻转条件：control_count % 50 == 0
```

测量结果：

```text
PA0 高电平持续时间：约 500 ms
PA0 低电平持续时间：约 500 ms
完整周期：约 1 s
```

结论：

```text
Week10 TIM2 固定周期任务验证通过。
```

---

## 当前限制

当前项目还没有完成这些功能：

- 串口接收电脑命令。
- PWM 执行器输出的完整验证。
- 电机驱动板接线与安全测试。
- 编码器读取实际速度。
- PID 速度闭环。
- CSV 日志输出和曲线分析。

这些将在后续 Week11-Week16 逐步完成。

---

## 阶段记录

### Week 9 完成记录

```text
主工程固定
  -> PA0 运行灯
  -> USART1 TX 串口输出
  -> Serial 底层发送模块
  -> Debug 调试输出层
  -> tick 运行计数
```

### Week 10 完成记录

```text
TIM2 内部时钟
  -> 10ms 更新中断
  -> 中断置 control_loop_flag
  -> main.c 主循环处理周期任务
  -> PA0 每 500ms 翻转
  -> 串口每 1s 输出 tick_ms
  -> 逻辑分析仪验证完整周期约 1s
```

---

## 下一步

下一阶段进入 Week11：PWM 输出与执行器控制。

重点目标：

```text
保留 TIM2 作为 10ms 系统节拍
新增 TIM3_CH1 / PA6 作为 PWM 输出
实现 PWM_SetDuty(0-100)
先验证 PWM 波形，再考虑电机驱动板
```

关键边界：

```text
PWM 是控制量，不是速度反馈。
编码器速度反馈要到 Week12 再做。
PID 速度闭环要到 Week14 再做。
```
