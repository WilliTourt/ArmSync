# ArmSync

ArmSync is a six-degree-of-freedom robotic arm project using a dual-IMU master-slave wireless teleoperation scheme. It was originally developed for the Renesas Electronics Competition.

This repository contains mechanical design files, PCBs, embedded firmware, kinematics tools, and UI resources. You are welcome to reference the designs and code, but please note that the project is still being organized and iterated.

**⚠ WARNING: The software resources in this repository are provided for reference only.**

[中文文档](README-zh-CN.md)

---

## Overview

ArmSync is a six-degree-of-freedom robotic arm primarily made from PLA 3D-printed parts. It contains six mechanical joints, J1–J6, and supports configurable end effectors.

![ARM](./IMG_20260712_131320.jpg)

![ARM](./IMG_20260726_022927.jpg)

Main features:

- **Six degrees of freedom**: J1 base rotation, J2 shoulder pitch, J3 shoulder yaw, J4 elbow, J5 wrist pitch, and J6 wrist rotation
- **Dual-IMU master-slave control**: Uses user-side IMU modules to obtain upper-arm and forearm attitude for wireless/serial master-slave control
- **Stepper-motor actuators**: J1–J6 use stepper motors, gearboxes, and motor controllers
- **Configurable end effector**: A gripper is provided in the 3D resources, and other custom end effectors can also be installed

## Specifications

| Parameter | Value |
|------|------|
| Arm degrees of freedom | 6 (J1–J6) |
| Reach (including gripper) | Approximately 610mm |
| Target end-effector payload | 350g |
| Control interface | CAN bus / servo signal |
| Power supply | 24VDC (stepper motors) / 5VDC (servo) |
| Structural material | PLA 3D-printed parts |

## Repository Structure

```
ArmSync/
├── Hardware/                         # Mechanical, electrical, and assembly resources
│   ├── 3D Files/                     # Robot arm, gripper, and controller 3D models
│   │   ├── 3MF for printing/         # 3MF files ready for slicing
│   │   ├── SolidWorks 2026/          # SolidWorks source files
│   │   ├── STEP and STL/             # STEP/STL universal formats
│   │   └── URDF/                     # ROS/Gazebo model
│   ├── PCBs/                         # PCB projects and images
│   ├── example_imgs/                 # J1–J6 and EE assembly illustrations
│   ├── Parts BOM-zh-CN.md            # Chinese bill of materials
│   ├── Assembly Guide-zh-CN.md       # Chinese assembly guide
│   └── readme-zh-CN.md               # Hardware documentation
├── Software/                         # Firmware, kinematics tools, and UI resources
│   ├── Arm-Side/                     # RA8P1 robot-arm controller project
│   │   ├── Solution/                 # e² studio Solution, CPU0, and CPU1 projects
│   │   ├── arm_ik_test.py            # IK visualization tool for Jetson input
│   │   ├── arm_angle_view.py         # Joint-angle and spatial-vector visualization tool
│   │   └── Scripts README-zh-CN.md   # Python tools usage guide
│   ├── User-Side/                    # RA4M1 user-side controller and IMU projects
│   ├── Deprecated Project Folders/   # Deprecated projects
│   ├── UI/                           # HMI, UI layout, and font resources
│   └── readme-zh-CN.md               # Software documentation
├── Helpful Docs/                     # Reference material for MCU, NPU, HMI, and dual-core development
├── LICENSE
├── README-zh-CN.md
└── README.md
```

## Quick Start

### Hardware

- [Hardware documentation](Hardware/readme-zh-CN.md): Hardware specifications, directory structure, and resource descriptions
- [Bill of materials](Hardware/Parts%20BOM-zh-CN.md): Motors, gearboxes, fasteners, and development tools
- [Assembly guide](Hardware/Assembly%20Guide-zh-CN.md): Arm assembly, gripper, timing belts, and wiring
- [3D files](Hardware/3D%20Files/): SolidWorks, STEP/STL, 3MF, and URDF files
- [PCB resources](Hardware/PCBs/): Controller, IMU, and RA8P1 extension-board resources

For printing the mechanical parts, use:

`Hardware/3D Files/3MF for printing/Renesas ArmSync.3mf`

You can also use the universal models in `STEP and STL/` or modify the source files in `SolidWorks 2026/`.

### Software

Read the [software documentation](Software/readme-zh-CN.md) to understand the current project structure and toolchains:

- The RA8P1 controller uses e² studio and consists of the Solution, CPU0, and CPU1 projects
- The RA4M1 controller and IMU use CMake, Ninja, and VS Code
- The robot-arm Python tools are described in [Scripts README-zh-CN.md](Software/Arm-Side/Scripts%20README-zh-CN.md)
- The robot-arm coordinate system and kinematics parameters are described in [Armsync_IK_Model.md](Software/Arm-Side/Armsync_IK_Model.md)

Make sure that the FSP, Arm GCC toolchain, serial-port configuration, and actual hardware version are compatible.
