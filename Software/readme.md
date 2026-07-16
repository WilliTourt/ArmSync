# ArmSync Software Documentation

[中文文档](/Software/readme-zh-CN.md)

---

## Directory Structure

```
Software/
├── Armsync_RoboticArm_RA8P1/   # Robot arm main controller (RA8P1, e2 Studio)
├── Armsync_ArmIMU/             # IMU module (RA4M1, CMake + VSCode)
├── Armsync_Ctrller/            # Hand controller (RA4M1, CMake + VSCode)
├── Arm_vscode (DEPRECATED)/    # [Deprecated] Old robot arm main (RA6M5)
├── Ctrller_keil (DEPRECATED)/  # [Deprecated] Old hand controller (RA4M1, Keil)
└── IMU_keil (DEPRECATED)/      # [Deprecated] Old IMU module (RA4M1, Keil)
```

## Hardware Platform

- **MCU**: Renesas RA Family
  - RA8P1 (Armsync_RoboticArm_RA8P1, current)
  - RA4M1 (Armsync_ArmIMU, Armsync_Ctrller, current)
  - RA6M5 (Arm_vscode, deprecated)
  - RA4M1 legacy projects (Ctrller_keil, IMU_keil, deprecated)
- **Framework**: Renesas FSP
  - RA8P1: FSP 6.4.0 (e2 Studio)
  - RA4M1: FSP 6.4.0 (CMake + VSCode)

## Overview

### Armsync_RoboticArm_RA8P1 - Robot Arm Main Controller

- FreeRTOS multi-task architecture
- Receives and parses hand attitude data
- Gripper control
- (Under development)

### Armsync_Ctrller - Hand Controller

- IMU sensor fusion (ICM42688P 6-axis + QMC5883P 3-axis magnetometer)
- Quaternion fusion using ESKF
- Sends attitude data via Bluetooth serial
- ADC reads pressure sensor for gripper control

### Armsync_ArmIMU - IMU Module

- Same ICM42688P + QMC5883P sensor setup
- Supports upper-arm / forearm mode configuration (conditional compilation)
- Sends quaternion data via serial

### Deprecated Projects

- **Arm_vscode**: Old RA6M5 main controller, replaced by RA8P1
- **Ctrller_keil / IMU_keil**: Old Keil projects, migrated to CMake + VSCode

## Communication Protocol

Output packets use a custom protocol with the following format:

| Field | Description |
|-------|-------------|
| Header | 0xAF |
| Function Code | 0x01~0x04 |
| Data | Quaternion / Euler angles / ADC values etc. |
| Tail | 0xFA |
