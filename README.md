# ArmSync

A 7-DOF wireless teleoperation robotic arm with dual-IMU master-slave control, designed for the Renesas Electronics Competition.

[中文文档](./README-zh-CN.md)

## ⚠️ Work in Progress

This repository is currently being organized. **Only 3D model files are uploaded at this stage.**

Software, documentation, and BOM are coming soon. Check back later or watch this repo for updates.

## Overview

ArmSync is a 3D-printed robotic arm featuring:
- **7 Degrees of Freedom**: Shoulder (3DOF) + Elbow (1DOF) + Wrist (2DOF) + Gripper (1DOF)
- **Dual-IMU Control**: Wireless master-slave operation using IMU-based motion tracking
- **CAN Bus Stepper Motors**: Precise control with high-torque reduction gearboxes
- **500g Payload**: Capable of lifting a standard water bottle
- **Fully Open Source**: Hardware, firmware, and software

## Specifications

| Parameter | Value |
|-----------|-------|
| Total DOF | 7 |
| Arm Reach | ~50cm (forearm 30cm + upper arm 20cm) |
| Payload | 500g |
| Control Method | Wireless IMU-based teleoperation |
| Actuators | CAN bus stepper motors with gearboxes |
| Material | PLA (3D printed) |

## Repository Structure

```
ArmSync/
├── Hardware/           # Mechanical design files ✅
│   ├── 3D Files/       # CAD files (SolidWorks, STEP, STL, 3MF)
│   │   ├── 3MF for printing/      # Ready-to-print 3MF files
│   │   ├── SolidWorks 2026/       # Source CAD files
│   │   └── STEP and STL/          # Exported formats
│   ├── Parts BOM.md    # 🚧 Coming soon
│   └── readme.md       # Hardware documentation 🚧
├── Software/           # Firmware and control software 🚧
│   └── readme.md       # 🚧 Coming soon
├── LICENSE             # MIT License ✅
└── README.md           # This file ✅
```

Legend: ✅ Available | 🚧 Work in progress

## What's Available Now

### 3D Model Files (`Hardware/3D Files/`)

| Folder | Contents |
|--------|----------|
| `3MF for printing/` | Pre-sliced 3MF file for Bambu Lab printers |
| `SolidWorks 2026/` | Original CAD files (.SLDPRT, .SLDASM) |
| `STEP and STL/` | Universal export formats |

**Included assemblies:**
- Main arm structure (J1 Base, Shoulder, Upper arm, Forearm, Wrist)
- Gripper assembly
- RC controller (remote control)

## What's Coming

- [ ] Complete Bill of Materials (BOM)
- [ ] Assembly instructions
- [ ] Firmware source code
- [ ] Control software
- [ ] Wiring diagrams
- [ ] User manual

## Quick Start (for 3D Printing)

1. Download `Hardware/3D Files/3MF for printing/Renesas ArmSync.3mf`
2. Import into Bambu Studio (or your preferred slicer)
3. Review the print settings and adjust for your printer
4. Print all parts

For other CAD software, use files in `STEP and STL/` folder.

## License

This project is licensed under the MIT License - see the [LICENSE](./LICENSE) file for details.

## Acknowledgments

- Designed for the Renesas Electronics Competition
- Gripper design adapted from open source projects

## MakerWorld

3D printable files will also be available on [MakerWorld](https://makerworld.com) for easier slicing and printing.

---

**Note**: This is a student competition project. Use at your own risk.
