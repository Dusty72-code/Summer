# Summer — 暑期大作业

---

## 任务背景

- 舵机 -> 云台yaw轴
- 电机 -> 底盘其中一个轮子
- 摇杆 -> Y轴控制轮子前进后退的速度，X轴控制yaw轴旋转方向和速度

## 项目简介

- **主控芯片**：STM32F103C8T6
- **实时操作系统**：FreeRTOS V10.3.1 + CMSIS-RTOS V2
- **工程生成**：STM32CubeMX（`summer.ioc`）
- **算法**：C语言

系统由两块电路板组成：

| 板卡 | 角色 | 承担功能 |
|------|------|----------|
| 云台板（GIMBAL） | 上位机/控制端 | 摇杆采样、舵机 PWM 驱动、SW 键触发自检、LED 指示 |
| 底盘板（CHASSIS） | 下位机/执行端 | 电机速度环闭环、编码器反馈、OLED 仪表盘、LED 指示 |

控制链路：**摇杆 → 云台板 → CAN（0x200 控制帧）→ 底盘板 → PID 速度环 → 电机 → 编码器 → 底盘板 → CAN（0x201 反馈帧）→ 云台板 → OLED/LED**。

---

## 硬件平台与引脚分配

| 外设 | 引脚/资源 | 说明 |
|------|-----------|------|
| 系统指示灯 LED | PB0（高电平点亮） | 状态指示：启动闪烁 / 常亮 / 呼吸灯 |
| 摇杆 SW 键 | PB1 | 自检触发（带防抖） |
| 舵机 | TIM2_CH3 | 20ms 周期 PWM，脉宽 500~2500µs 对应 ±满速 |
| 电机驱动 TB6612 | TIM1_CH1（PA8 PWM）+ PB12/AIN1、PB13/AIN2、PB14/STBY | PWM 频率约 2.2kHz |
| 编码器 | TIM3 编码器模式（PA6/PA7） | 11 线 × 减速比 30 × 4 倍频 = 1320 计数/转 |
| OLED（SSD1306） | I2C1（重映射 PB8/PB9），地址 0x78 | 128×64，全页刷新，6×8 点阵字体 |
| CAN | CAN1（USB_LP_CAN1_RX0 / CAN1_SCE 中断） | 标准帧 8 字节 |

---

## 软件架构（代码分层）

工程采用「**硬件层 → 中间层 → 应用层**」的分层架构

```
┌─────────────────────────────────────────────────────────────┐
│ application/  （应用层）                                      │         
│   ├─ C_Summer       板级编译开关（GIMBAL/CHASSIS）、全局常量、外设初始化分发 │
│   ├─ Communication  CAN 应用状态机（收发缓存、心跳、在线判定）      │
│   ├─ Gimbal         摇杆映射、舵机控制                          │
│   └─ Chassis        OLED 仪表盘                               │
├─────────────────────────────────────────────────────────────┤
│ component/    （中间层）                                      │
│   ├─ communication  can_protocol    CAN 报文编解码            │
│   ├─ controller     pid             通用 PID 库              │
│   └─ motor          motor_control   电机控制 + PID 实例 + 自检 │
├─────────────────────────────────────────────────────────────┤
│ bsp/          （硬件层）                                      │
│   bsp_can / bsp_oled / bsp_motor / bsp_servo / bsp_joystick / bsp_led  │
├─────────────────────────────────────────────────────────────┤
│ Core/         （CubeMX 生成：main、freertos 任务、外设初始化）    │
│ Middlewares/  （FreeRTOS 内核）                               │
│ Drivers/      （STM32 HAL 库 + CMSIS）                        │
└─────────────────────────────────────────────────────────────┘
```

### 目录结构

```
summer1/
├── Core/                  # CubeMX 生成：main.c、freertos.c、外设驱动、中断
├── Drivers/               # STM32F1xx HAL + CMSIS
├── Middlewares/           # FreeRTOS V10.3.1 + CMSIS-RTOS V2
├── application/           # 应用层
│   ├── C_Summer/          #   板级开关 Summer.h、外设初始化分发
│   ├── Communication/     #   CAN 应用层 can_app
│   ├── Gimbal/            #   摇杆 joystick_app、舵机 servo_app
│   └── Chassis/           #   OLED 仪表盘 OLED_app
├── bsp/                   # 硬件层
├── component/             # 组件层
│   ├── communication/     #   CAN 协议编解码 can_protocol
│   ├── controller/        #   通用 PID 库
│   └── motor/             #   电机控制 motor_control
├── CMakeLists.txt / summer.ioc / 链接脚本 / OpenOCD 配置
└── README.md
```

---

## FreeRTOS 任务设计

### 公共任务（两板均创建）

| 任务 | 栈大小 | 优先级 | 周期 | 功能 |
|------|--------|--------|------|------|
| `CAN_SendTask` | 128×4 B | AboveNormal (32) | 10ms（通信异常时降为 100ms） | 周期性编码并发送本板报文，发送计数 `can_tx_cnt++` |
| `CAN_RecvTask` | 256×4 B | AboveNormal (32) | 轮询（≈1ms） | 从环形缓冲取帧解码，依据心跳变化判定对端在线 |
| `CAN_HBTask` | 128×4 B | Low (8) | 50ms | 心跳超时检测（100ms），翻转 `can_comm_ok` 与 `STATUS_CAN_ERROR` |
| `LEDTask` | 128×4 B | Normal (24) | 状态相关（200ms 决策） | 启动闪烁、常亮 / 呼吸灯状态机 |

### 云台板（GIMBAL）专属任务

| 任务 | 栈大小 | 优先级 | 周期 | 功能 |
|------|--------|--------|------|------|
| `JoystickTask` | 128×4 B | Normal (24) | 10ms | 上电摇杆自动校准；采样 X/Y 轴 → 舵机/电机目标转速；SW 键触发自检 |
| `ServoTask` | 128×4 B | AboveNormal (32) | 10ms | 将 `g_servo.target_speed` 写入舵机 PWM |
 
### 底盘板（CHASSIS）专属任务

| 任务 | 栈大小 | 优先级 | 周期 | 功能 |
|------|--------|--------|------|------|
| `MotorTask` | 256×4 B | AboveNormal (32) | 10ms | 上电电机自检；读编码器 → 计算实际转速 → PID 速度环 → 输出 PWM |
| `OLEDTask` | 256×4 B | Normal (24) | 400ms | 刷新 OLED 仪表盘 |

### 任务调度关系

```
                        ┌──────────┐
  JoystickTask(10ms) ──▶│ 摇杆/按键 │──▶ SW 按下 → SelfTest_Execute
                        └──────────┘──▶ X轴 → g_servo.target_speed ──▶ ServoTask ──▶ 舵机PWM
                                        Y轴 → wheel_target_speed ──▶ CAN_SendTask(0x200)
                                                                           │
                    ┌──────────────────────────────────────────────────────┘
                    ▼
              ┌──────────┐   ┌──────────┐    ┌──────────┐    ┌──────────┐
  底盘板       │ CAN_Recv │──▶│ MotorTask │──▶│ PID 速度环 │──▶│ 电机 PWM  │
              └──────────┘   └──────────┘    └──────────┘    └──────────┘
                    ▲                                              │
                    │◀──── CAN_SendTask(0x201) ◀── 实际转速/编码器 ◀──┘
                    │
              ┌──────────┐   ┌──────────┐   ┌──────────┐
              │ CAN_HB   │   │ LEDTask  │   │ OLEDTask │
              │ 心跳监测   │   │ 状态指示  │   │ 仪表盘    │
              └──────────┘   └──────────┘   └──────────┘
```

---

## 板间通信方案 - CAN通信

### 报文定义

**0x200 云台 → 底盘（`GimbalCtrlMsg_t`）**

| 字节 | 字段 | 说明 |
|------|------|------|
| [0:1] | `servo_target_speed` (int16) | 舵机目标转速（rpm，范围 ±60） |
| [2:3] | `wheel_target_speed` (int16) | 电机目标转速（rpm，范围 ±200） |
| [4] | `servo_online` | 舵机在线标志 |
| [5] | `gimbal_heartbeat` | 心跳计数（每帧 +1，溢出回绕） |
| [6] | `status_flags` | 状态位：`0x00 SYSTEM_OK / 0x01 CAN_ERROR / 0x08 SELF_TEST` |
| [7] | `reserved` | 保留 |

**0x201 底盘 → 云台（`ChassisFeedbackMsg_t`）**

| 字节 | 字段 | 说明 |
|------|------|------|
| [0:1] | `motor_actual_speed` (int16) | 电机实际转速（rpm） |
| [2:3] | `motor_encoder_raw` (int16) | 原始编码器增量 |
| [4] | `motor_online` | 电机在线标志 |
| [5] | `chassis_heartbeat` | 心跳计数 |
| [6] | `status_flags` | 状态位（同上） |
| [7] | `reserved` | 保留 |

### 在线检测（心跳机制）

- 发送侧：每帧携带 8 位自增心跳计数；
- 接收侧：`CAN_RecvTask` 检测到心跳值变化即置对端 `online = 1`；
- 超时判定：`CAN_HBTask` 每 50ms 检查 `HAL_GetTick() - last_rx_time > 100ms`，判定对端掉线，翻转 `can_comm_ok` 并在状态帧中置 `STATUS_CAN_ERROR`；
- 掉线降速：通信异常时 `CAN_SendTask` 发送周期由 10ms 降为 100ms，减小总线负载。

### 通信保障设计

- **无锁化接收环形缓冲**：CAN 接收中断（`HAL_CAN_RxFifo0MsgPendingCallback`）将帧推入 8 帧环形缓冲，仅对 `count` 使用临界区保护，任务侧取帧解码，避免中断与任务竞争丢帧；
- **发送错误自恢复**：`HAL_CAN_AddTxMessage` 发送失败时调用 `BSP_CAN_Recover()`（停 CAN → 启 CAN → 重挂中断），自动恢复总线；
- **状态帧跨板传播**：`SYSTEM_OK / CAN_ERROR / SELF_TEST` 三种系统状态通过 `status_flags` 实时同步到对端，保证两板故障语义统一

---

## 舵机控制（云台板）

控制信号链：

```
摇杆 X 轴 → JoystickTask(10ms) → ServoControl_SetSpeed → g_servo.target_speed
          → ServoTask(10ms) → BSP_Servo_SetSpeed → TIM2_CH3 PWM
          → CAN_App_SetGimbalCtrl → 0x200 帧（底盘 OLED 显示）
```

- **PWM 生成**：TIM2 CH3，预分频 15、自动重载 19999 → **20ms 周期**；
- **转速映射**：`pulse = 1500 − speed×10`（speed 为 -100~100 百分比）；
- **速度换算**：`target_rpm = speed% / 100 × 60`，满速对应 **±60 rpm**；
- **摇杆映射**：X 轴百分比 × 60 → 目标转速，最大 ±60 rpm。

---

## 电机控制（底盘板）

### 速度环 PID 闭环

| 参数 | 值 | 说明 |
|------|-----|----|
| Kp | 250.0 | 比例增益 |
| Ki | 100.0 | 积分增益 |
| Kd | 0.0 | 微分 |
| 输出限幅 | ±7199 | 对应 TIM1 PWM 满量程 |
| 积分限幅 | ±7199 | 抗积分饱和 |

### 电机在线自检

上电即执行（`Motor_SelfTest`）：停转 → 清编码器 → 以 1500 PWM 正转 300ms → 停 → 读取编码器增量，**增量 ≥ 100** 判定电机/驱动器在线（`motor_online=1, motor_error=0`），否则判定故障（`motor_error=1`），底盘 LED 进入呼吸灯状态

---

## OLED 显示（底盘板）

| 行 y | 内容 | 含义 |
|------|------|------|
| 0 | `= SELF TEST =` / `Chassis C Board` | 自检状态 / 板名 |
| 10 | `Srv :  60rpm[OK]` | 舵机目标转速（云台经 CAN 下发）+ 在线状态 |
| 19 | `MtrT: 100rpm` | 电机目标转速 |
| 28 | `MtrR: 100rpm[OK]` | 电机实际转速 + 在线状态 |
| 37 | `CAN:[OK]G-C` | CAN 通信状态 |
| 46 | `TX: 123 RX: 123` | CAN 收发帧计数（双向增长证明总线在通信） |
| 55 | `Up:  123s` | 系统运行时长 |

---

## LED 状态指示

上电先执行 `LED_StartBlink()`：**闪烁 3 次（80ms 亮 / 80ms 灭）**，作为系统正常启动检测
随后由 `LEDTask` 状态机（每 200ms 决策）控制：

| 场景 | 云台板（GIMBAL） | 底盘板（CHASSIS） |
|------|------------------|-------------------|
| 自检进行中 | 常亮 | 常亮 |
| 正常 | 底盘在线 **且** 电机在线 → **常亮** | 无电机错误 → **常亮** |
| 异常 | **CAN 通信失败 → 呼吸灯** | **电机错误 → 呼吸灯** |

---

## 额外功能

### 整机自检（SW 键触发）

云台板摇杆 SW 键按下触发 `SelfTest_Execute()`，完整流程：

```
SW 按下
  │
  ├─ 1. 置 STATUS_SELF_TEST 状态位 → 底盘 OLED 第一行显示 "= SELF TEST ="
  ├─ 2. 云台 LED 闪烁提示
  ├─ 3. 舵机正转 60 rpm，持续 2s（SELF_TEST_SERVO_DURATION_MS）
  ├─ 4. 舵机逆转 60 rpm，持续 2s
  ├─ 5. 舵机回中，停 0.5s
  ├─ 6. 电机正转 100 rpm，持续 4s（SELF_TEST_PHASE_DURATION_MS）
  ├─ 7. 电机逆转 100 rpm，持续 4s
  └─ 8. 清 STATUS_SELF_TEST，舵机回中，LED 常亮
         → OLED 恢复 "Chassis C Board"，系统重新进入正常工作状态
```
