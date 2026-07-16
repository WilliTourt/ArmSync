# ArmSync 软件说明

[English Document](/Software/readme.md)

---

## 目录结构

```
Software/
├── Armsync_RoboticArm_RA8P1/   # 机械臂主控（RA8P1，e2 Studio）
├── Armsync_ArmIMU/             # IMU 模块端（RA4M1，CMake + VSCode）
├── Armsync_Ctrller/            # 手部控制器（RA4M1，CMake + VSCode）
├── Arm_vscode (DEPRECATED)/    # [已废弃] 旧版机械臂主控（RA6M5）
├── Ctrller_keil (DEPRECATED)/  # [已废弃] 旧版手部控制器（RA4M1，Keil）
└── IMU_keil (DEPRECATED)/      # [已废弃] 旧版 IMU 模块（RA4M1，Keil）
```

## 硬件平台

- **主控芯片**: Renesas RA 系列 MCU
  - RA8P1 (Armsync_RoboticArm_RA8P1，当前主力)
  - RA4M1 (Armsync_ArmIMU, Armsync_Ctrller，当前主力)
  - RA6M5 (Arm_vscode，已废弃)
  - RA4M1 旧工程 (Ctrller_keil, IMU_keil，均已废弃)
- **开发框架**: Renesas FSP
  - RA8P1: FSP 6.4.0 (e2 Studio)
  - RA4M1: FSP 6.4.0 (CMake + VSCode)

## 功能简介

### Armsync_RoboticArm_RA8P1 - 机械臂主控

- FreeRTOS 多任务架构
- 接收手部姿态数据并解析
- 夹爪控制
- （仍在开发）

### Armsync_Ctrller - 手部控制器

- IMU 传感器融合（ICM42688P 六轴 + QMC5883P 三轴磁力计）
- 使用 ESKF 融合四元数
- 通过蓝牙串口发送姿态数据
- ADC 读取压力传感器控制夹爪

### Armsync_ArmIMU - IMU 模块端

- 同样使用 ICM42688P + QMC5883P
- 支持大臂/小臂两种模式配置（条件编译）
- 通过串口发送四元数数据

### 已废弃工程

- **Arm_vscode**: 旧版 RA6M5 主控，已被 RA8P1 替代
- **Ctrller_keil / IMU_keil**: 旧版 Keil 工程，已迁移到 CMake + VSCode

## 通信协议

数据包使用自定义协议，格式如下：

| 字段 | 说明 |
|------|------|
| 帧头 | 0xAF |
| 功能码 | 0x01~0x04 |
| 数据 | 四元数/欧拉角/ADC值等 |
| 帧尾 | 0xFA |
