# ArmSync

A 7-DOF wireless teleoperation humanoid robotic arm with dual-IMU master-slave control, designed for the Renesas Electronics Competition.

If you are also working on a robotic arm project, feel free to reference our design and code, but please note their limitations. All files are provided as-is.

[中文文档](README-zh-CN.md)

---

## Overview

ArmSync is a 3D-printed robotic arm featuring:

- **7 Degrees of Freedom**: Shoulder (3DOF) + Elbow (1DOF) + Wrist (2DOF) + Gripper (1DOF)
- **Dual-IMU Control**: Wireless master-slave operation using IMU-based motion tracking
- **CAN Bus Stepper Motors**: Precise control with high-torque reduction gearboxes
- **Design Payload**: ~500g, suitable for lightweight grasping tasks

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
├── Hardware/                   # Mechanical design files
│   ├── 3D Files/               # CAD files
│   │   ├── 3MF for printing/   # Ready-to-print 3MF files
│   │   ├── SolidWorks 2026/    # Source CAD files
│   │   └── STEP and STL/       # Exported formats
│   ├── example_imgs/           # Assembly example images (J1-J7)
│   ├── PCBs for Reference/     # Reference PCB files
│   │   ├── EasyEDA_LCEDA/      # LCEDA project files
│   │   └── Imgs/               # PCB images and schematics
│   ├── Assembly Guide-zh-CN.md # Chinese assembly guide
│   ├── Assembly Guide.md       # English assembly guide
│   ├── Parts BOM-zh-CN.md      # Chinese parts list
│   ├── Parts BOM.md            # English parts list
│   ├── readme-zh-CN.md         # Hardware docs (Chinese)
│   └── readme.md               # Hardware docs (English)
├── Software/                   # Firmware and control software
│   ├── Arm_vscode/             # Robotic arm main controller (RA6M5)
│   ├── Ctrller_keil/           # Hand controller (RA4M1)
│   ├── IMU_keil/               # IMU module (RA4M1)
│   ├── readme-zh-CN.md         # Software docs (Chinese)
│   └── readme.md               # Software docs (English)
├── LICENSE                     # MIT License
├── README-zh-CN.md             # This document (Chinese)
└── README.md                   # This document (English)
```

## Quick Start

### Hardware Documentation

- [Parts BOM](Hardware/Parts%20BOM.md) - All required parts and some supplier information
- [Assembly Guide](Hardware/Assembly%20Guide.md) - Brief description of assembly steps
- [Hardware readme](Hardware/readme.md) - Mechanical structure documentation

### Print Parts

1. Download `Hardware/3D Files/3MF for printing/Renesas ArmSync.3mf`
2. Import into Bambu Studio or other 3MF-compatible slicer
3. Review print settings and adjust for your printer
4. Print all parts

For other CAD software, use files in `STEP and STL/` folder.

### PCBs

Refer to LCEDA project files and images in `Hardware/PCBs for Reference/`. For reference only.

### Software

Refer to `Software/readme.md` for information on the three project components.

⚠️ **Note**: The software code is provided for documentation purposes only. Due to time constraints during the competition, the code was written hastily and has limited reference value.

## License

MIT License - See [LICENSE](LICENSE) file for details

## Acknowledgments

Thanks to Renesas Electronics for the technical support and competition platform.
