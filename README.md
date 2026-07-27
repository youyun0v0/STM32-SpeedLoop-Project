# STM32 SpeedLoop Project

## 快速总结

本项目是第 9-16 周 STM32 速度闭环阶段的主工程。

第 9 周已经完成主工程恢复、`PA0` 运行指示灯、`USART1 TX` 串口输出和最小 Debug 输出层。当前目标不是电机闭环，而是确认后续 8 周可以在同一个工程上继续扩展。

当前已验证：

- 工程来源：从 `5-2_DMA+ADC_Multi-Channel` 复制为主项目。
- 主工程位置：`D:\Workspace\04_Idle\STM32\SpeedLoop_Project`。
- `PA0` LED 可以周期闪烁，证明程序正在运行。
- `USART1 TX / PA9` 可以通过 USB-TTL 输出到 VSCode 串口监视器。
- 串口监视器可以看到启动信息和递增 `tick`。
- `Serial` 模块负责底层串口发送。
- `Debug` 模块负责调试输出格式。

第 9 周边界：

- 暂不做 PWM。
- 暂不接电机。
- 暂不读编码器。
- 暂不写 PID。
- 暂不做串口命令接收。
- 暂不整理 ADC、DMA、OLED 等已有模块。

---

## 当前阶段

第 9 周：工程恢复、GPIO 与 UART 最小闭环。

当前最小运行链路：

```text
main.c
  -> 初始化 PA0 运行灯
  -> Debug_Init()
      -> Serial_Init()
          -> USART1 TX / PA9
  -> Debug_Print("SpeedLoop Project Start!")
  -> Debug_PrintTick(tick)
  -> VSCode 串口监视器显示 tick
```

---

## 硬件与工具链

| 项目 | 当前内容 |
|---|---|
| 主控 | STM32F103C8T6 |
| 工具链 | VSCode EIDE / Keil 工程 |
| 库类型 | STM32F10x Standard Peripheral Library |
| 主工程 | `SpeedLoop_Project` |
| 下载方式 | 当前可正常下载运行 |
| 串口模块 | USB-SERIAL CH340 |
| 串口端口 | `COM6`，以实际设备管理器为准 |
| 串口监视器 | VSCode 串口监视器 |

---

## 本周接线

### LED 运行指示

| 功能 | STM32 引脚 | 说明 |
|---|---|---|
| 运行指示 LED | `PA0` | 当前第 9 周运行灯 |
| LED GND | `GND` | 必须共地 |

### USART1 TX

Day 4 只做 STM32 向电脑发送数据，所以只需要 TX 和 GND。

| STM32 | USB-TTL / CH340 | 作用 |
|---|---|---|
| `PA9 / USART1_TX` | `RXD` | STM32 发，USB-TTL 收 |
| `GND` | `GND` | 共地 |

Day 4-6 暂时不需要：

| 暂不连接 | 原因 |
|---|---|
| `PA10 / USART1_RX -> TXD` | 第 13 周串口命令接收时再做 |
| USB-TTL `VCC` | STM32 已通过现有方式供电，避免供电线接错 |

关键提醒：

```text
PA9(TX) 接 USB-TTL RXD
GND 接 GND
不要接成 TX 对 TX
```

---

## 外设配置

| 外设 | 引脚 | 模式/参数 | 用途 |
|---|---|---|---|
| GPIOA | `PA0` | `GPIO_Mode_Out_PP`, `50MHz` | 运行指示灯 |
| USART1 TX | `PA9` | `GPIO_Mode_AF_PP`, `50MHz` | 串口发送 |
| USART1 | `PA9` | `115200, 8N1, no parity, no flow control, Tx only` | Debug 输出 |

串口监视器设置：

```text
监视模式：Serial
查看模式：文本
端口：COM6 - USB-SERIAL CH340
波特率：115200
行尾：无
发送格式：以文本形式发送
```

---

## 当前代码结构

本周重点文件：

```text
User/
  main.c

Hardware/
  Serial.h
  Serial.c

App/
  Debug.h
  Debug.c

BSP/
  bsp_gpio.h
  bsp_gpio.c

System/
  Delay.h
  Delay.c
```

当前工程中已有但第 9 周暂不展开的模块：

```text
Hardware/
  LED.c/.h
  PWM.c/.h
  Encoder.c/.h
  Motor.c/.h
  OLED.c/.h
  ADC.c/.h

System/
  Timer.c/.h
  MyDMA.c/.h

BSP/
  bsp_timer.c/.h
```

注意：当前 `Hardware/LED.c/.h` 默认控制的是 `PA1/PA2`，而第 9 周运行指示灯使用的是 `PA0`。

---

## 模块职责

### `main.c`

负责主流程：

- 初始化 `PA0` 运行指示灯。
- 初始化 Debug 输出。
- 输出启动信息。
- 在主循环中翻转 LED 并打印 `tick`。

当前主循环核心现象：

```text
PA0 LED 周期闪烁
串口输出 tick=0, tick=1, tick=2...
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
- 避免 `main.c` 里散落大量 `Serial_SendString()` 和 `Serial_SendNumber()`。

当前接口：

```c
void Debug_Init(void);
void Debug_Print(const char *message);
void Debug_PrintTick(uint32_t tick);
void Debug_PrintConfig(void);
```

当前 Debug 模块只负责第 9 周需要的启动信息和 `tick` 输出。  
后续 PID 的 `error=xx`、`pwm=xx`、`actual_speed=xx` 等输出，要等第 14 周有真实变量后再扩展。

---

## 当前串口输出样例

VSCode 串口监视器中已观察到：

```text
SpeedLoop Project Start!
tick=0
tick=1
tick=2
tick=3
tick=4
```

复位后会重新输出启动信息，并且 `tick` 从 0 开始：

```text
SpeedLoop Project Start!
tick=0
tick=1
tick=2
```

这说明：

- 程序确实重新启动。
- 主循环没有卡死。
- 串口输出稳定。
- 数字输出函数 `Serial_SendNumber()` 可用。

---

## 第 9 周完成记录

### Day 1：选定主工程

选择 `5-2_DMA+ADC_Multi-Channel` 复制为主项目。

选择理由：

- 已有 `BSP/`、`Hardware/`、`System/` 等模块。
- 后续第 10-16 周可以复用已有 GPIO、Timer、PWM、Encoder、Motor 等基础代码。

当前边界：

- 虽然工程已有 ADC、DMA、OLED、PWM、Encoder、Motor 等模块，但第 9 周不展开这些内容。

### Day 2：最小工程运行验证

完成内容：

- 打开主工程。
- 编译、下载并运行。
- 使用 `PA0` LED 闪烁证明程序正在运行。

结论：

```text
SpeedLoop_Project 可以作为第 9-16 周主工程继续使用。
```

### Day 3：GPIO / LED 边界整理

完成内容：

- 选择方案 A：继续使用 `BSP_GPIO_InitPin()` 控制 `PA0` 作为运行灯。
- 确认已有 `Hardware/LED.c/.h` 默认控制 `PA1/PA2`。

结论：

```text
PA0 是第 9 周运行指示灯。
Hardware/LED 模块暂不改动。
```

### Day 4：USART1 TX 输出

完成内容：

- 新增 `Hardware/Serial.h`。
- 新增 `Hardware/Serial.c`。
- 使用 `USART1 TX / PA9` 输出字符串。
- VSCode 串口监视器能看到启动信息和 `tick`。

关键配置：

```text
PA9 -> USB-TTL RXD
GND -> GND
baudrate = 115200
mode = Tx only
```

### Day 5：周期打印运行计数

完成内容：

- 新增 `Serial_SendNumber(uint32_t number)`。
- 主循环输出递增 `tick`。

已验证现象：

```text
tick=0
tick=1
tick=2
...
```

复位后 `tick` 从 0 重新开始。

### Day 6：最小 Debug 输出层

完成内容：

- 新增 `App/Debug.h`。
- 新增 `App/Debug.c`。
- 使用 `Debug_Init()` 初始化串口调试输出。
- 使用 `Debug_Print()` 输出启动信息。
- 使用 `Debug_PrintTick()` 输出运行计数。

当前分层：

```text
main.c
  -> Debug_PrintTick()
      -> Serial_SendString()
      -> Serial_SendNumber()
      -> USART1 TX
```

---

## 本周排障记录

| 问题 | 现象 | 处理 |
|---|---|---|
| 讲义初版没有贴合实际项目 | 原讲义假设新建 LED 模块，但工程已有 `BSP` 和 `Hardware/LED` | 修正讲义，改为复用 `BSP_GPIO_InitPin()` 和已有模块 |
| `Serial.c` 使用 `BSP_GPIO_InitPin()` 但未包含头文件 | 可能出现隐式声明或编译警告 | 在 `Serial.c` 中加入 `#include "bsp_gpio.h"` |
| `Serial.h` 声明 `uint32_t` 但未包含类型定义 | 依赖其他 `.c` 文件先包含 `stm32f10x.h`，头文件不够自洽 | 在 `Serial.h` 中加入 `#include "stm32f10x.h"` |
| `Debug.h` 声明 `uint32_t` 但未包含类型定义 | 头文件不够自洽 | 在 `Debug.h` 中加入 `#include "stm32f10x.h"` |
| `Debug_PrintConfig()` 多行字符串不够清楚 | `Debug_Print()` 本身会追加换行，多行字符串可读性一般 | 改成多次单行 `Debug_Print()` |

---

## 当前限制

当前项目还不能做这些事：

- 不能接收电脑发来的串口命令。
- 不能控制 PWM 输出。
- 不能读取编码器速度。
- 不能做 PID 速度闭环。
- 不能输出负数，例如 PID 误差 `error=-5`。

原因：

- `USART_Mode` 当前只开了 `Tx`，没有开 `Rx`。
- `Serial_SendNumber()` 当前只支持 `uint32_t`。
- 第 9 周只建立最小观察链路，不提前写闭环功能。

---

## 第 10 周入口

第 10 周将在当前工程基础上加入定时器固定周期任务。

建议保留：

- `PA0` 运行指示灯。
- `USART1 TX / PA9` 串口输出。
- `Serial` 模块。
- `Debug` 模块。

第 10 周目标：

```text
TIM 定时器固定周期
  -> 周期 flag
  -> 主循环处理周期任务
  -> Debug_PrintTick() 输出周期计数
  -> 为后续 PID 固定 dt 打基础
```

第 10 周不要急着做：

- PWM。
- 电机。
- 编码器。
- PID。

---

## 第 9 周结论

第 9 周已经达标。

当前工程已经具备后续扩展需要的最小地基：

```text
主工程固定
  -> PA0 运行灯
  -> USART1 TX 串口输出
  -> Serial 底层发送模块
  -> Debug 调试输出层
  -> tick 运行计数
```

下一步进入第 10 周：用定时器建立固定周期控制循环。
#   S T M 3 2 - S p e e d L o o p - P r o j e c t  
 