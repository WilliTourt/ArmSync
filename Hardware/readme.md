# Hardware - ArmSync 7DOF Robotic Arm

This directory contains all hardware-related files for the ArmSync 7DOF robotic arm project.

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

Check the [3D Files](./3D%20Files/) directory for the complete mechanical design:
- **SolidWorks 2026/**: Original editable source files
- **STEP and STL/**: Universal formats for viewing and printing
- **3MF for printing/**: Pre-configured Bambu Studio project files

### 2. Gather Parts

See [Parts BOM.md](./Parts%20BOM.md) for the complete parts list with:
- Motor specifications and pricing
- Gearbox selection guide
- Fastener quantities
- Estimated total cost (~1,232 RMB)

### 3. Print the Parts

**Recommended Settings:**
- Material: PLA Basic (structural parts), PETG (high stress parts)
- Layer Height: 0.2mm
- Infill: 30-40% (structural), 20% (cosmetic)
- Supports: As needed (see individual part notes)

**Print Checklist:**
- [ ] J1 Base
- [ ] Shoulder assembly parts
- [ ] Upper arm (upper and lower)
- [ ] Forearm (upper and lower)
- [ ] Wrist assembly
- [ ] Gripper components
- [ ] RC controller (optional)

### 4. Assemble

Follow the [Assembly Guide.md](./Assembly%20Guide.md) for detailed step-by-step instructions:
- Assembly order: J1 → J2 → J3 → J4 → J5 → J6 → J7
- Torque specifications for fasteners
- Wiring and CAN bus setup
- Initial testing procedures

## Design Highlights

### Motor Selection
- **CAN-enabled closed-loop stepper motors** for precise position control
- **Gear reduction** via planetary gearboxes for high torque output
- **Cost-effective** compared to servo motors or harmonic drives

### Structural Design
- **Modular joint design** for easy maintenance and upgrades
- **Aluminum extrusion base** (6060) for stability
- **3D printed construction** for rapid prototyping and low cost

### Transmission
- **Planetary gearboxes** on J1-J5 for high reduction ratios
- **Timing belt option** available for J5/J6 as alternative
- **Direct drive or coupling** on J6 for compact wrist design

## Bill of Materials Summary

| Category | Estimated Cost (RMB) |
|----------|---------------------|
| Motors (J1-J7) | 577 |
| Gearboxes (J1-J6) | 447 |
| Timing belts/pulleys | 30 |
| Fasteners/couplings | 125 |
| Development tools | 53 |
| **Total** | **~1,232** |

*Note: Aluminum extrusion, screws, and shipping costs not included*

## Tools Required

- Hex key set (M2.5/M3/M5/M6)
- Screwdriver set
- Caliper (optional, for verification)
- Grease (for gearboxes)
- Multimeter (for wiring checks)

## Safety Notes

⚠️ **Important:**
- Always power off before making mechanical adjustments
- Keep clear during initial testing - unexpected movements can occur
- Ensure proper grounding of the power supply
- Use appropriate safety gear when operating

## Contributing

If you build this arm and find improvements:
1. Fork the repository
2. Make your changes
3. Submit a pull request with photos of your build

## License

Hardware designs are released under the MIT License. See the root [LICENSE](../LICENSE) file for details.

## Acknowledgments

- Motors: Zhangdatou (张大头) closed-loop stepper motors
- Gearboxes: Pufeide Electric (普菲德电气) planetary gearboxes
- Printed on: Bambu Lab P1S

## Support

For questions about hardware assembly:
1. Check the [Assembly Guide](./Assembly%20Guide.md) troubleshooting section
2. Open an issue on GitHub
3. Contact the project maintainer

---

**Next Steps:** After completing hardware assembly, proceed to the [Software](../Software/) directory for firmware and control software setup.
