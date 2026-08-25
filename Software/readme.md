# ArmSync Software Documentation

> This document describes the software components, directory structure, supported devices, and build/import options.

**⚠ WARNING ⚠: The code is provided for reference only!**

[中文文档](/Software/readme-zh-CN.md)

---

## 1. Software Components

The current software mainly consists of three parts:

1. **Robot-arm controller**: Based on a Renesas RA8P1 dual-core MCU. It receives control data, processes attitude and joint data, performs motion planning and inverse kinematics, and controls the motors and end effector.
2. **User-side controller**: Based on a Renesas RA4M1. It reads analog inputs and buttons on the handheld controller, receives data from the external IMU, and outputs control data over serial.
3. **User-side IMU module**: Based on a Renesas RA4M1. It reads the ICM42688 and QMC5883P, runs ESKF attitude fusion, and outputs attitude/spatial-vector data.

The repository also retains legacy projects, inverse-kinematics test scripts, and UI resources.

## 2. Directory Structure

```
Software/
├── Arm-Side/                         # Current RA8P1 robot-arm projects and tools
│   ├── Solution/
│   │   ├── ArmSync_Solution/         # e² studio Solution project
│   │   ├── ArmSync_Solution_CPU0/    # RA8P1 CPU0 application project
│   │   └── ArmSync_Solution_CPU1/    # RA8P1 CPU1 application project
│   ├── Armsync_IK_Model.md           # Robot-arm coordinate system and IK parameters
│   ├── arm_ik_test.py                # IK test script
│   └── How_to_import.txt             # e² studio project import notes
├── User-Side/
│   ├── Armsync_Ctrller/              # RA4M1 user-side controller, CMake + VS Code
│   └── Armsync_ArmIMU/               # RA4M1 IMU module, CMake + VS Code
├── Deprecated Project Folders/       # Deprecated projects
│   ├── Arm_vscode (DEPRECATED)/      # Legacy RA6M5 robot-arm controller
│   ├── Ctrller_keil (DEPRECATED)/    # Legacy RA4M1 controller Keil project
│   └── IMU_keil (DEPRECATED)/        # Legacy RA4M1 IMU Keil project
└── UI/                               # UI/HMI files, layouts, and font resources
```

## 3. Current Hardware Platforms and Toolchains

| Project | MCU | FSP | Build/development environment |
|------|-----|-----|---------------|
| `ArmSync_Solution_CPU0` | RA8P1 (`R7KA8P1KF`) | 6.5.0 | e² studio / GCC Arm Embedded |
| `ArmSync_Solution_CPU1` | RA8P1 (`R7KA8P1KF`) | 6.5.0 | e² studio / GCC Arm Embedded |
| `Armsync_Ctrller` | RA4M1 (`R7FA4M1AB`) | 6.4.0 (compatible with 6.5.0) | CMake/Ninja + VS Code |
| `Armsync_ArmIMU` | RA4M1 (`R7FA4M1AB`) | 6.4.0 (compatible with 6.5.0) | CMake/Ninja + VS Code |

The RA8P1 projects currently use the GCC Arm Embedded 13.2.1 toolchain. The exact GCC installation path for the RA4M1 projects must be configured locally; the fixed path in the repository configuration cannot be used directly. Also note that CMake requires the `ARM_TOOLCHAIN_PATH` environment variable to be configured.

## 4. Robot-Arm Controller

### 4.1 CPU0: Control and Motion Processing

The CPU0 project is located at:

`Arm-Side/Solution/ArmSync_Solution_CPU0/`

CPU0 uses FreeRTOS with C++ task wrappers. Its current application modules include:

- `UartRecvTask`: Receives user-side controller data and other serial inputs
- `NormalizeTask`: Normalizes input data, transforms coordinates, and organizes joint data
- `IKTask`: Performs inverse-kinematics calculations from the end-effector target
- `FusionTask`: Fuses IK results, hand-joint data, and attitude input; it includes NPU smoothing logic for J1–J5
- `MotionPlanningTask`: Converts target joint angles into motor motion plans
- `RecPlayTask`: Records and plays back joint motion and gripper data
- `CPUCommTask`: Sends motion plans to CPU1 through RA8P1 dual-core IPC and receives motor feedback
- `UITask`: Handles serial UI data, displayed feedback, and UI commands

CPU0 also contains:

- Kinematics and joint-angle processing for the six-degree-of-freedom arm
- IK solver code including FABRIK
- Flash parameter storage (Flash is currently disabled in the code; RAM is used for convenience)
- Ethos-U NPU models and inference code, including an older MLP model and a newer filter model
- Debug output, ring buffers, and inter-task queues

### 4.2 CPU1: Actuator Control

The CPU1 project is located at:

`Arm-Side/Solution/ArmSync_Solution_CPU1/`

CPU1 converts CPU0 motion plans into actuator commands and sends actuator status back to CPU0. Its current code includes:

- `drivers/Emm_V5.c`: Communication driver for the stepper-motor controllers
- `ipc/`: Shared memory and IPC messages between CPU0 and CPU1
- `gripper/`: ZP20S bus-servo gripper control
- Feedback for motor angles, locked status, stalled status, and gripper angle

The IPC data structures in CPU1 are designed for six joints, `J1` through `J6`, with separate gripper-angle and gripper-control data. The gripper uses SCI5 half-duplex single-wire UART, with servo ID 0, a baud rate of 115200, and 180° mode by default.

### 4.3 CPU0/CPU1 Data Flow and Software Architecture

![Software Architecture](./Software%20Architecture.png)

The FLASH portions shown in the diagram differ from the actual code. The current code does not use FLASH for NPU-model storage or motion recording.

The CPU0 IPC control packet also contains emergency-stop and homing requests. CPU1 monitors control-packet timeouts to detect whether CPU0 is still sending valid control data.

## 5. User-Side Controller

Project location:

`User-Side/Armsync_Ctrller/`

This is a Renesas RA CMake project. Its main functions include:

- Reading the ADC analog input used for gripper control
- Reading other handheld analog inputs, such as wrist-pitch control
- Handling buttons, LEDs, the buzzer, and other peripherals
- Receiving data from the IMU module
- Forwarding processed attitude and spatial-vector data downstream over serial
- Sending calibration commands

The IMU sensor drivers and ESKF are not located in this project; they are located in `Armsync_ArmIMU`.

## 6. User-Side IMU Module

Project location:

`User-Side/Armsync_ArmIMU/`

The current code uses:

- ICM42688: Accelerometer and gyroscope
- QMC5883P: Magnetometer
- ESKF: Attitude fusion
- SPI, I²C, external interrupt, and UART

The IMU module supports selecting upper-arm or forearm mode through conditional compilation. In forearm mode, it combines upper-arm IMU data to calculate relative attitude and sends quaternion and spatial-vector data.

Data frames use the `0xAF` header, function code `0x01`, and `0xFA` tail.

Calibration and bias-correction commands use short frames:

| Function | Data frame |
|------|--------|
| Calibration | `AF 02 FA` |
| Accelerometer/gyroscope bias correction (not currently used) | `AF 03 FA` |

## 7. Current Serial Data Formats

The code stores floating-point values in big-endian order. The frame formats are:

| Frame type | Length | Contents |
|--------|------|------|
| Forearm IMU output | 43 bytes | Header, function code, four quaternion floats, three upper-arm vector floats, three forearm vector floats, tail |
| IMU quaternion output | 19 bytes | Header, function code, four quaternion floats, tail |
| Calibration command | 3 bytes | `AF 02 FA` |
| Bias-correction command | 3 bytes | `AF 03 FA` |

The UART channel, frame length, and data interpretation must be verified against `cpp_main.cpp`, the callback functions, and the FSP configuration in the relevant project. When changing a data frame, update both the sender and receiver.

## 8. Building and Importing

### 8.1 RA8P1 e² studio Projects

The RA8P1 project is not a standalone C/C++ project. It consists of three projects: the Solution, CPU0, and CPU1 projects (an FSP Solution Project).

According to `Arm-Side/How_to_import.txt`:

1. Import the `ArmSync_Solution` Solution project.
2. Import the `ArmSync_Solution_CPU0` C/C++ project.
3. Import the `ArmSync_Solution_CPU1` C/C++ project.
4. Build the Solution project in e² studio; the CPU0 and CPU1 projects will be built along with it.

**In e² studio, update each project's `Properties → C/C++ Build → Settings`: compiler include paths, linker script paths, and linker options.** The linker options must include all four of the following: `--specs=nano.specs`, `-u_printf_float`, `-u_scanf_float`, and `--specs=nosys.specs`.

Project configuration and generated files are located in each project's `configuration.xml`, `ra_cfg/`, and `ra_gen/` directories. The `ra/` directory contains FSP, CMSIS, FreeRTOS, CMSIS-DSP/CMSIS-NN, Ethos-U dependencies, and generated code.

### 8.2 RA4M1 CMake Projects

`Armsync_Ctrller` and `Armsync_ArmIMU` each contain an independent `CMakeLists.txt` and GCC toolchain configuration.

Configure the `ARM_TOOLCHAIN_PATH` environment variable in VS Code or in the system environment, then select the ARM GCC Kit that uses the toolchain file.

## 9. Deprecated Projects

The projects in `Deprecated Project Folders/` are for historical reference only:

- `Arm_vscode (DEPRECATED)`: Legacy RA6M5 robot-arm controller, replaced by the RA8P1 dual-core project
- `Ctrller_keil (DEPRECATED)`: Legacy RA4M1 user-side controller Keil project
- `IMU_keil (DEPRECATED)`: Legacy RA4M1 IMU Keil project

## 10. UI Resources

`Software/UI/` mainly contains HMI files, UI layout previews, and font resources. `.HMI` files are project files for the TaoJingChi (TJC) screen UI IDE. The directory includes:

- `ArmSync.HMI`: USART HMI IDE project
- `ArmSync UI Layout.html`: UI background design
- `ArmSync UI Layout.png`
- `Fonts/`: Font files used by USART HMI

The UI resources work with `UITask` in RA8P1 CPU0. This directory does not contain a standalone general-purpose PC application.
