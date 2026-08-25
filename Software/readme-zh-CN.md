# ArmSync 软件文档

> 本文档描述了软件组成、文件夹目录结构、芯片适配、构建/导入选项等信息

**⚠警告⚠：**代码仅供参考！

[English Document](/Software/readme.md)

---

## 1. 软件组成

当前软件主要由三部分组成：

1. **机械臂端主控**：基于 Renesas RA8P1 双核 MCU，负责接收控制数据、姿态与关节数据处理、运动规划、逆运动学以及电机和末端执行器控制。
2. **用户端控制器**：基于 Renesas RA4M1，读取手柄上的模拟量、按键和外部 IMU 数据，并通过串口输出控制数据。
3. **用户端 IMU 模块**：基于 Renesas RA4M1，读取 ICM42688 和 QMC5883P，执行 ESKF 姿态融合，并输出姿态/空间向量数据。

此外，仓库中还保留了旧版工程、逆运动学测试脚本和 UI 资源。

## 2. 目录结构

```
Software/
├── Arm-Side/                         # 当前机械臂端 RA8P1 工程及相关工具
│   ├── Solution/
│   │   ├── ArmSync_Solution/         # e² studio Solution 工程
│   │   ├── ArmSync_Solution_CPU0/    # RA8P1 CPU0 应用工程
│   │   └── ArmSync_Solution_CPU1/    # RA8P1 CPU1 应用工程
│   ├── Armsync_IK_Model.md           # 机械臂坐标系和 IK 参数说明
│   ├── arm_ik_test.py                # IK 测试脚本
│   └── How_to_import.txt             # e² studio 工程导入提示
├── User-Side/
│   ├── Armsync_Ctrller/              # RA4M1 用户端控制器，CMake + VS Code
│   └── Armsync_ArmIMU/               # RA4M1 IMU 模块，CMake + VS Code
├── Deprecated Project Folders/       # 已废弃的旧工程
│   ├── Arm_vscode (DEPRECATED)/      # 旧 RA6M5 机械臂主控
│   ├── Ctrller_keil (DEPRECATED)/    # 旧 RA4M1 控制器 Keil 工程
│   └── IMU_keil (DEPRECATED)/        # 旧 RA4M1 IMU Keil 工程
└── UI/                               # UI/HMI 文件、布局和字体资源
```

## 3. 当前硬件平台和工具链

| 工程 | MCU | FSP | 构建/开发环境 |
|------|-----|-----|---------------|
| `ArmSync_Solution_CPU0` | RA8P1（`R7KA8P1KF`） | 6.5.0 | e² studio / GCC Arm Embedded |
| `ArmSync_Solution_CPU1` | RA8P1（`R7KA8P1KF`） | 6.5.0 | e² studio / GCC Arm Embedded |
| `Armsync_Ctrller` | RA4M1（`R7FA4M1AB`） | 6.4.0（和6.5.0可兼容） | CMake/Ninja + VS Code |
| `Armsync_ArmIMU`  | RA4M1（`R7FA4M1AB`） | 6.4.0（和6.5.0可兼容） | CMake/Ninja + VS Code |

RA8P1 工程当前使用 GCC Arm Embedded 13.2.1 工具链。RA4M1 工程的具体 GCC 安装路径需要在本地环境中配置，不能直接使用仓库配置文件中的固定路径。也请注意CMake需要配置`ARM_TOOLCHAIN_PATH`环境变量。

## 4. 机械臂端主控

### 4.1 CPU0：控制与运动处理

CPU0 工程位于：

`Arm-Side/Solution/ArmSync_Solution_CPU0/`

CPU0 使用 FreeRTOS 和 C++ 任务封装，当前应用模块包括：

- `UartRecvTask`：接收用户端控制器数据和其他串口输入
- `NormalizeTask`：对输入数据进行归一化、坐标转换和关节数据整理
- `IKTask`：根据末端目标执行逆运动学计算
- `FusionTask`：融合 IK 结果、手部关节数据和姿态输入；其中包含面向 J1～J5 的 NPU 平滑滤波逻辑
- `MotionPlanningTask`：将关节目标角度转换为电机运动规划数据
- `RecPlayTask`：记录和回放关节运动及夹爪数据
- `CPUCommTask`：通过 RA8P1 双核 IPC 将运动计划发送给 CPU1，并接收电机反馈
- `UITask`：处理串口 UI 数据、显示反馈和 UI 控制命令

CPU0 还包含：

- 六自由度机械臂的运动学和关节角处理
- FABRIK 等 IK 求解器代码
- Flash 参数存储（目前代码中禁用了FLASH，为图方便，直接使用了RAM）
- Ethos-U NPU 模型及相关推理代码，包含一个老的 MLP 模型和一个新的滤波器模型
- 调试输出、环形缓冲区和任务间队列

### 4.2 CPU1：执行器控制

CPU1 工程位于：

`Arm-Side/Solution/ArmSync_Solution_CPU1/`

CPU1 负责将 CPU0 的运动计划转换为执行器命令，并将执行器状态反馈给 CPU0。当前代码包括：

- `drivers/Emm_V5.c`：步进电机控制器通信驱动
- `ipc/`：CPU0 与 CPU1 之间的共享内存和 IPC 消息
- `gripper/`：ZP20S 总线舵机夹爪控制
- 电机角度、锁定状态、卡滞状态和夹爪角度反馈

CPU1 中的 IPC 数据结构已经按照 6 个关节设计：`J1`至`J6`，另有独立的夹爪角度和夹爪控制数据。夹爪使用 SCI5 半双工单线 UART，默认舵机 ID 为 0、波特率为 115200，180° 模式。

### 4.3 CPU0 与 CPU1 的数据流和软件架构总图

![Software Architecture](./Software%20Architecture.png)

图中 FLASH 部分和实际代码存在出入，当前代码中，NPU模型存储和动作录制都不使用 FLASH。

CPU0 的 IPC 控制包还包含急停状态和回零请求；CPU1 具有控制包超时监视逻辑，用于检测 CPU0 是否继续发送有效控制数据。

## 5. 用户端控制器

工程位置：

`User-Side/Armsync_Ctrller/`

这是一个 Renesas RA CMake 工程，主要功能包括：

- 读取用于夹爪控制的 ADC 模拟量
- 读取其他手柄模拟输入，例如腕部俯仰控制量
- 处理按键、LED、蜂鸣器等外设
- 接收 IMU 模块输出的数据
- 通过串口向下游设备转发整理后的姿态和空间向量数据
- 发送校准命令

IMU 传感器驱动和 ESKF 并不位于该工程，而是位于 `Armsync_ArmIMU` 工程中。

## 6. 用户端 IMU 模块

工程位置：

`User-Side/Armsync_ArmIMU/`

当前代码使用：

- ICM42688：加速度计和陀螺仪
- QMC5883P：磁力计
- ESKF：姿态融合
- SPI、I²C、外部中断和 UART

IMU 模块支持通过条件编译选择大臂或小臂模式。小臂模式下，模块会结合大臂 IMU 数据计算相对姿态，并发送四元数和空间向量数据。

数据帧使用 `0xAF` 帧头，`0x01` 和 `0xFA` 帧尾。

校准和偏置校正命令使用短帧：

| 功能 | 数据帧 |
|------|--------|
| 校准 | `AF 02 FA` |
| 加速度/陀螺仪偏置校正（此命令未被使用） | `AF 03 FA` |

## 7. 当前串口数据格式概览

代码中使用大端序存储浮点数，帧格式如下：

| 帧类型 | 长度 | 内容 |
|--------|------|------|
| IMU 小臂输出 | 43 字节 | 帧头、功能码、四元数 4 个 float、上臂向量 3 个 float、前臂向量 3 个 float、帧尾 |
| IMU 四元数输出 | 19 字节 | 帧头、功能码、四元数 4 个 float、帧尾 |
| 校准命令 | 3 字节 | `AF 02 FA` |
| 偏置校正命令 | 3 字节 | `AF 03 FA` |

具体 UART 通道、帧长度和数据解释应以对应工程中的 `cpp_main.cpp`、回调函数和 FSP 配置为准。修改数据帧时需要同步修改发送端和接收端。

## 8. 构建与导入

### 8.1 RA8P1 e² studio 工程

RA8P1 工程不是一个单独的 C/C++ 工程，而是由 Solution、CPU0 和 CPU1 三个工程共同组成（FSP Sulution Project）。

按照 `Arm-Side/How_to_import.txt` 的说明：

1. 导入 `ArmSync_Solution` Solution 工程。
2. 导入 `ArmSync_Solution_CPU0` C/C++ 工程。
3. 导入 `ArmSync_Solution_CPU1` C/C++ 工程。
4. 在 e² studio 中构建 Solution 工程；CPU0 和 CPU1 工程会随 Solution 一起构建。

**注意修改E2 Studio中每个工程的`属性 -> C/C++构建 -> 设置`的编译器包含路径、链接器链接文件路径和链接选项。**链接选项需要勾选 --specs=nano.specs、-u_printf_float、-u_scanf_float、--specs=nosys.specs 四项。

工程配置和生成文件位于各自的 `configuration.xml`、`ra_cfg/` 和 `ra_gen/` 目录中。`ra/` 目录包含 FSP、CMSIS、FreeRTOS、CMSIS-DSP/CMSIS-NN 以及 Ethos-U 相关依赖和生成代码。

### 8.2 RA4M1 CMake 工程

`Armsync_Ctrller` 和 `Armsync_ArmIMU` 都包含独立的 `CMakeLists.txt` 和 GCC 工具链配置。

可以在 VS Code 或系统环境变量中配置 `ARM_TOOLCHAIN_PATH` 环境变量，然后选择带有工具链文件的 ARM GCC Kit

## 9. 已废弃工程

`Deprecated Project Folders/` 中的工程仅用于历史参考

- `Arm_vscode (DEPRECATED)`：旧版 RA6M5 机械臂主控，已被 RA8P1 双核工程替代
- `Ctrller_keil (DEPRECATED)`：旧版 RA4M1 用户端控制器 Keil 工程
- `IMU_keil (DEPRECATED)`：旧版 RA4M1 IMU Keil 工程

## 10. UI 资源

`Software/UI/` 目前主要保存 HMI 文件、界面布局预览和字体资源，`.HMI`文件是“陶晶驰(TaoJingChi)”品牌的屏幕UI IDE工程文件。文件夹中具体包括：

- `ArmSync.HMI`，USART HMI 的 IDE 工程
- `ArmSync UI Layout.html`，UI背景设计
- `ArmSync UI Layout.png`
- `Fonts/`，USART HMI 所使用的字库文件

UI 资源与 RA8P1 CPU0 中的 `UITask` 配合使用。该目录本身不包含一个独立的通用 PC 应用工程。
