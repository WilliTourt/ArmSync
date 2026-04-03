# Hardware - ArmSync 7DOF Robotic Arm

This directory contains all hardware-related files for the ArmSync 7DOF robotic arm project.

[中文文档](/Hardware/readme-zh-CN.md)

## Overview

ArmSync is a 7-degree-of-freedom robotic arm featuring dual-IMU master-slave wireless teleoperation control. The arm is designed for educational and research purposes, with a focus on cost-effectiveness and ease of replication.

### Key Specifications

| Parameter | Value |
|-----------|-------|
| Degrees of Freedom | 7 (J1-J7) |
| Reach (with gripper) | ~60cm |
| Payload Capacity | 500g (target) |
| Control Interface | CAN Bus |
| Power Supply | 24V DC |

### Joint Configuration

| Joint | Name | Motor | Gear Ratio | Function |
|-------|------|-------|------------|----------|
| J1 | Base Rotation | 57×76mm Stepper | 20:1 | Base rotation |
| J2 | Shoulder Pitch | 42×60mm Stepper | 30:1 | Shoulder up/down |
| J3 | Shoulder Yaw | 42×40mm Stepper | 10:1 | Shoulder left/right |
| J4 | Elbow | 42×48mm Stepper | 10:1 | Elbow bend |
| J5 | Wrist Pitch | 42×40mm Stepper | 10:1 or 5:1 | Wrist up/down |
| J6 | Wrist Roll | 42×34mm Stepper | - | Wrist rotation |
| J7 | Gripper | MG90S Servo | - | End effector |

## Directory Structure

```
Hardware/
├── 3D Files/              # All 3D model files
│   ├── 3MF for printing/  # Ready-to-print 3MF files
│   ├── SolidWorks 2026/   # Original SolidWorks source files
│   └── STEP and STL/      # Exported neutral format files
├── Parts BOM.md           # Bill of materials with pricing
├── Assembly Guide.md      # Step-by-step assembly instructions
└── readme.md              # This file
```

## Quick Start

### 1. Review the Design

Check the [3D Files](./3D%20Files/) directory for complete mechanical design files:
- **SolidWorks 2026/**: Original editable source files
- **STEP and STL/**: Universal formats for viewing and printing
- **3MF for printing/**: Pre-configured Bambu Studio project files

### 2. Gather Parts

See [Parts BOM.md](./Parts%20BOM.md) for complete parts list including:
- Motor specifications and pricing
- Gearbox selection guide
- Fastener quantities
- Estimated total cost (approximately 1,400 RMB)

### 3. Print the Parts

**Recommended Settings:**
- Material: PLA Basic or better
- Layer Height: 0.18mm
- Infill: 35-60% (structural), 20% (cosmetic)
- Supports: As needed

### 4. Assemble

Follow [Assembly Guide.md](./Assembly%20Guide.md) for step-by-step assembly:
- Assembly order: From top to bottom. J1 → J2 → J3 → J4 → J5 → J6 → J7
- Fastener torque specifications
- Wiring and CAN bus setup

## Support

For questions about hardware assembly:
1. Check [Assembly Guide](./Assembly%20Guide.md)
2. Or submit an Issue on GitHub
3. Or contact me via email or other social media!
