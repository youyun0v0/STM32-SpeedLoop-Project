# STM32 SpeedLoop Project

本项目是第 9-16 周 STM32 速度闭环阶段的主工程。当前存档节点为：**第 11 周完成，PWM 与真实电机开环驱动已验证**。

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

### 第 11 周：PWM、TB6612 与真实电机开环测试

已完成：

- 将 `Hardware/PWM.c/.h` 的主输出固定为 `TIM3_CH1 / PA6`，PWM 频率为 `1 kHz`。
- 实现 `PWM_SetDuty(0-100)` 和 `PWM_GetDuty()`，占空比参数会被限制在 `0-100%`。
- 使用 PA6 LED 验证 `0%、25%、50%、75%、100%` 五档亮度变化。
- 重写 `Hardware/Motor.c/.h`，使用 `PA4/PA5` 控制方向，使用 `PA6` 控制 PWM。
- 使用逻辑分析仪直接在 TB6612FNG 输入端验证 `PWMA、AIN1、AIN2、STBY、VCC`。
- 接入 `GA25-370 12V` 减速电机、`TB6612FNG` 驱动板和可调电源。
- 电机在 `2V` 直连测试中正常转动，电流约 `0.058A`，确认电机和六芯转接线正常。
- `6V / 10% PWM` 启动转矩不足；提高到 `40%` 后电机成功转动。
- 最终使用 `6V / 60% PWM` 完成正转、停止、反转循环，空载电流约 `0.048-0.071A`。
- 将 PA0 运行灯改为 `TIM2_CH1` 低亮度 PWM，保持每 `500 ms` 闪烁一次，亮灯阶段占空比为 `5%`。

当前第 11 周达标结论：

```text
TIM3_CH1 / PA6 能输出 1kHz PWM。
PA4/PA5 能正确控制 TB6612FNG 正反转方向。
GA25-370 能在 6V / 60% PWM 下完成正转、停止和反转。
当前仍是开环占空比控制，Motor_SetSpeed(60) 不代表实际速度为 60。
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
| 电机 | GA25-370，额定电压 12V，带编码器 |
| 电机驱动 | TB6612FNG，当前使用 A 路 |
| 电机电源 | 可调直流电源，测试时使用 6V / 0.5A 限流 |

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

### TB6612FNG 与电机

| STM32 / 电源 | TB6612FNG | 作用 |
|---|---|---|
| `PA6 / TIM3_CH1` | `PWMA` | A 路 PWM 控制量 |
| `PA4` | `AIN1` | 方向控制 1 |
| `PA5` | `AIN2` | 方向控制 2 |
| STM32 `3.3V` | `VCC` | 驱动板逻辑电源 |
| STM32 `3.3V` | `STBY` | 拉高并使能驱动板 |
| STM32 `GND` | `GND` | 逻辑地 |
| 可调电源正极 | `VM` | 电机电源，Week11 使用 6V |
| 可调电源负极 | `GND` | 与 STM32、驱动板共地 |
| 电机动力线 | `AO1 / AO2` | GA25-370 电机两端 |

编码器其余四根线本周未接入，留到 Week12。

### 逻辑分析仪验证接线

| 逻辑分析仪 | TB6612FNG 输入端 | 作用 |
|---|---|---|
| `D0 / CH0` | `PWMA` | 验证 PWM 频率和占空比 |
| `D1 / CH1` | `AIN1` | 验证正方向电平 |
| `D2 / CH2` | `AIN2` | 验证反方向电平 |
| `D3 / CH3` | `STBY` | 验证驱动板保持使能 |
| `D4 / CH4` | `VCC` | 验证逻辑电源为高电平状态 |
| `GND` | `GND` | 与 STM32、驱动板共地 |

逻辑分析仪只测数字侧，禁止连接 `VM、AO1、AO2`。

---

## 外设配置

| 外设 | 引脚/通道 | 参数 | 用途 |
|---|---|---|---|
| TIM2_CH1 | `PA0` | PWM，100Hz，亮灯阶段 5% duty | 低亮度运行灯 |
| USART1 TX | `PA9` | `GPIO_Mode_AF_PP`, `115200 8N1` | Debug 串口输出 |
| TIM2 | 内部时钟 | `PSC=7200-1`, `ARR=100-1` | 10ms 固定周期中断 |
| GPIOA | `PA4 / PA5` | 推挽输出 | TB6612FNG AIN1/AIN2 |
| TIM3_CH1 | `PA6` | `PSC=72-1`, `ARR=1000-1`, 1kHz | TB6612FNG PWMA |
| NVIC | `TIM2_IRQn` | 抢占优先级 2，子优先级 1 | 允许 TIM2 更新中断 |

TIM2 周期计算：

```text
72,000,000 / 7200 / 100 = 100 Hz
1 / 100 Hz = 10 ms
```

TIM3 电机 PWM 频率计算：

```text
72,000,000 / 72 / 1000 = 1,000 Hz
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
  PWM.h
  PWM.c
  Motor.h
  Motor.c

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
  Encoder.c/.h
  Servo.c/.h
  OLED.c/.h
  ADC.c/.h

System/
  MyDMA.c/.h
```

注意：`Hardware/LED.c/.h` 默认控制 `PA1/PA2`；当前运行灯使用 `PA0 / TIM2_CH1`，由 `PWM_LED_Init()` 和 `PWM_SetLEDDuty()` 控制。

---

## 模块职责

### `main.c`

负责主流程：

- 初始化 `Debug` 串口输出。
- 初始化 `TIM2` 固定周期模块。
- 初始化 `PA0 / TIM2_CH1` 低亮度运行灯。
- 初始化 `Motor`，并使电机默认停止。
- 在主循环中等待 `Timer_TakeControlFlag()`。
- 每 500ms 在 `0%` 和 `5%` LED duty 之间切换。
- 按 6 秒测试周期执行停止、正转、停止、反转。
- 在电机状态切换时输出串口日志。

当前主循环核心逻辑：

```text
while (1)
  -> Timer_TakeControlFlag()
  -> control_count++
  -> 每 50 次切换 PA0 的 0% / 5% duty
  -> 第 1 秒 Motor_SetSpeed(60)
  -> 第 3 秒 Motor_Stop()
  -> 第 4 秒 Motor_SetSpeed(-60)
  -> 第 6 秒 Motor_Stop() 并开始下一轮
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

### `PWM`

文件：

```text
Hardware/PWM.h
Hardware/PWM.c
```

职责：

- 使用 `TIM3_CH1 / PA6` 输出 `1 kHz` 电机 PWM。
- 将电机占空比限制在 `0-100%`，并换算为 TIM3 的 CCR1。
- 复用已经由 `Timer_Init()` 配置好的 `TIM2_CH1 / PA0`，输出低亮度运行灯 PWM。
- 电机 PWM 与运行灯 PWM 使用不同定时器，不会互相修改频率。

当前主要接口：

```c
void PWM_Init(void);
void PWM_SetDuty(uint8_t duty_percent);
uint8_t PWM_GetDuty(void);
void PWM_LED_Init(void);
void PWM_SetLEDDuty(uint8_t duty_percent);
```

初始化顺序要求：

```text
Timer_Init()
  -> PWM_LED_Init()
  -> Motor_Init()
     -> PWM_Init()
```

### `Motor`

文件：

```text
Hardware/Motor.h
Hardware/Motor.c
```

职责：

- 将 `-100` 到 `100` 的有符号控制量拆成方向和占空比。
- 正数：`PA4=1，PA5=0`，电机正转。
- 负数：`PA4=0，PA5=1`，电机反转。
- 零：`PA4=0，PA5=0，PWM=0%`，电机停止。
- 初始化结束时调用 `Motor_Stop()`，防止上电后意外转动。

当前接口：

```c
void Motor_Init(void);
void Motor_SetSpeed(int8_t speed);
void Motor_Stop(void);
```

这里的 `speed` 仍然只是开环 PWM 百分比控制量，不是编码器测得的实际速度。

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
void Debug_PrintDuty(uint8_t duty_percent);
```

---

## 当前串口输出样例

Week11 电机循环测试程序启动后，VSCode 串口监视器中可见：

```text
SpeedLoop Project Week11 Motor Test Start!
chip=STM32F103C8T6
uart=USART1(PA9)
baud=115200
led=PA0
motor pwm=PA6
motor dir=PA4/PA5
motor forward speed=60
motor stop
motor reverse speed=-60
motor stop, next cycle
```

说明：

- 程序已经进入 Week11 电机测试版本。
- 串口日志与正转、停止、反转动作同步。
- PA0 继续低亮度闪烁，说明 TIM2 节拍和主循环没有卡死。
- 日志中的 `60` 表示 `60% PWM`，不是实际转速。

---

## 逻辑分析仪验证记录

### Week10：TIM2 周期

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

### Week11：TB6612FNG 输入信号

逻辑分析仪探头直接接在 TB6612FNG 输入端，而不是只测 STM32 引脚：

```text
D0 -> PWMA
D1 -> AIN1
D2 -> AIN2
D3 -> STBY
D4 -> VCC
GND -> TB6612FNG GND
```

验证结果：

```text
PWMA：1kHz PWM，初始 10% duty 波形验证正确
正转阶段：AIN1 高，AIN2 低
停止阶段：AIN1 低，AIN2 低，PWMA 低
反转阶段：AIN1 低，AIN2 高
STBY：始终为高
VCC：始终为高
```

结论：

```text
STM32 到 TB6612FNG 输入端的 PWM、方向和使能信号均正确。
```

### Week11：真实电机测试

测试对象与结果：

| 测试 | 条件 | 结果 |
|---|---|---|
| 电机直连验证 | `2V`，短时空载 | 正常转动，约 `0.058A` |
| 初始驱动测试 | `6V / 10% PWM` | 未启动，控制波形正常 |
| 提高启动控制量 | `6V / 40% PWM` | 成功启动并完成正反转 |
| 最终记录 | `6V / 60% PWM` | 正常正反转，约 `0.048-0.071A` |

`10% PWM` 未启动并不表示驱动故障。其近似平均控制电压只有：

```text
6V * 10% = 0.6V
```

电机在 `2V` 直连时才能稳定观察到转动，因此提高占空比后启动是符合预期的。

---

## 当前限制

当前项目还没有完成这些功能：

- 串口接收电脑命令。
- 编码器读取实际速度。
- PID 速度闭环。
- CSV 日志输出和曲线分析。
- 带负载电流、启动电流和堵转电流测试。

当前 `Motor_SetSpeed()` 仍然是开环占空比命令。项目还不知道电机实际转速，也不会根据负载变化自动补偿。

这些将在后续 Week12-Week16 逐步完成。堵转测试不作为当前学习阶段的手动实验项目。

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

### Week 11 完成记录

```text
TIM3_CH1 / PA6 输出 1kHz PWM
  -> PWM_SetDuty(0-100)
  -> PA6 LED 五档亮度验证
  -> PA4/PA5 方向控制
  -> Motor_SetSpeed() 统一正反转与占空比
  -> TB6612FNG 输入端逻辑分析仪验证
  -> GA25-370 电机直连验证
  -> 6V / 60% PWM 真实正反转测试
  -> 空载电流约 0.048-0.071A
  -> PA0 改为 5% duty 低亮度运行灯
```

---

## 下一步

下一阶段进入 Week12：编码器计数与实际速度测量。

重点目标：

```text
确认 GA25-370 编码器 VCC、GND、A、B 线序
将编码器 A/B 相接入 STM32 定时器编码器接口
固定时间窗读取增量计数
计算带符号的实际转速
串口输出计数值和实际速度
```

关键边界：

```text
PWM 是控制量，不是速度反馈。
Week12 先建立可靠的编码器测量链路，不急着加入 PID。
PID 速度闭环要到 Week14 再做。
```
