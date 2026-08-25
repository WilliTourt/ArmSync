# Hardware Documentation - ArmSync 6-DOF Robotic Arm

This directory contains all hardware-related files for the ArmSync six-degree-of-freedom robotic arm project.

[中文文档](/Hardware/readme-zh-CN.md)

## Project Overview

ArmSync is a six-degree-of-freedom robotic arm with dual-IMU master-slave wireless teleoperation. The project focuses on cost-effectiveness and ease of replication.

### Main Specifications

| Parameter | Value |
|-----------|-------|
| Degrees of freedom | 6 (J1-J6) |
| Reach (including gripper) | ~610mm |
| End-effector payload | 350g (target) |
| Control interface | CAN bus / servo signal |
| Power supply | 24VDC (stepper motors) / 5VDC (servo) |

### Joint Configuration

| Joint | Name | Motor | Gear Ratio | Function |
|-------|------|-------|------------|----------|
| J1 | Base rotation | 57×76mm stepper motor | 20:1 | Base rotation |
| J2 | Shoulder pitch | 42×60mm stepper motor | 30:1 | Shoulder up/down |
| J3 | Shoulder yaw | 42×40mm stepper motor | 10:1 | Shoulder left/right |
| J4 | Elbow | 42×48mm stepper motor | 10:1 | Elbow bending |
| J5 | Wrist pitch | 42×40mm stepper motor | 10:1 or 5:1 | Wrist up/down |
| J6 | Wrist rotation | 42×34mm stepper motor | - | Wrist rotation |
| EE | Gripper | MG90S servo or any other end effector | - | End effector |

## Directory Structure

```
Hardware/
├── 3D Files/              # All 3D model files
│   ├── 3MF for printing/  # Ready-to-print 3MF files
│   ├── SolidWorks 2026/   # Original SolidWorks source files
│   ├── STEP and STL/      # Exported universal-format files
│   └── URDF/              # URDF files
├── Parts BOM.md           # Bill of materials and prices
├── Assembly Guide.md      # Step-by-step assembly instructions
└── readme.md              # This file
```

## Quick Start

### 1. Review the Design

See the [3D Files](./3D%20Files/) directory for the complete mechanical design files:
- **SolidWorks 2026/**: Original editable source files
- **STEP and STL/**: Universal formats for viewing and printing
- **3MF for printing/**: Pre-configured Bambu Studio project files
- **URDF/**: Robotic-arm model for ROS and other simulation tools

See the [PCB schematics and layouts](./PCBs/) directory for the required board files:
- **EasyEDA_LCEDA/**: Controller PCB, wearable IMU sensor PCB, and extension-board files for the Renesas CPKHMI-RA8P1 board
- **Imgs/**: Screenshots of schematics and PCBs
- **KiCad 10/**: Boards for the stepper-motor CAN bus / servo-signal connectors. These boards are designed to plug into each ZDT stepper-motor controller and relay the wiring.

### 2. Purchase Parts

See [Parts BOM.md](./Parts%20BOM.md) for the complete parts list, including:
- Motor specifications and prices
- Gearbox selection guide
- Fastener quantities
- Estimated total cost (approximately RMB 1,500)

### 3. 3D Printing

**Recommended print settings:**
- Material: PLA Basic or better
- Layer height: 0.18mm
- Infill: 35–60% (structural parts), 20% (cosmetic parts)
- Supports: Add as needed

**Note: Some M2.5 holes used to connect couplings may be undersized. Check the bore of the coupling you purchase and consider enlarging the holes to M3 if necessary.**

### 4. Assembly

Follow the [Assembly Guide.md](./Assembly%20Guide.md) for step-by-step assembly:
- Assembly order: from top to bottom: J1 → J2 → J3 → J4 → J5 → J6 → EE
- Fastener specifications
- Wiring

## Support

For questions about hardware assembly:
1. Check the [Assembly Guide](./Assembly%20Guide.md)
2. Submit an issue on GitHub
3. Contact me by email or through other social media.
