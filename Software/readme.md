# ArmSync Software Documentation

⚠️ **The software code in this repository is provided for documentation purposes only. Due to time constraints during the competition, the code was written hastily and lacks proper architectural design and documentation. Its reference value is limited.** If needed, it is recommended to use it only as a reference for understanding the project working principles, rather than direct reuse.

---

## Project Structure

```
Software/
├── Arm_vscode/      # Robotic arm main controller (RA6M5)
├── Ctrller_keil/    # Hand controller (RA4M1)
└── IMU_keil/       # IMU module (RA4M1)
```

## Hardware Platform

- **MCU**: Renesas RA Series
  - RA6M5 (Arm_vscode)
  - RA4M1 (Ctrller_keil, IMU_keil)
- **Development Framework**: Renesas FSP (Flexible Software Package)

## Function Overview

### Arm_vscode - Robotic Arm Main Controller
- CAN bus communication for controlling multiple motors
- Receiving and parsing hand pose data
- Gripper control
- Key interrupt handling

### Ctrller_keil - Hand Controller
- IMU sensor fusion (ICM42688P 6-axis + QMC5883P 3-axis magnetometer)
- Quaternion fusion using ESKF (Extended Kalman Filter)
- Sending pose data via Bluetooth serial
- ADC reading of pressure sensor for gripper control

### IMU_keil - IMU Module
- Also uses ICM42688P + QMC5883P
- Supports both upper arm and forearm modes
- Sending quaternion data via serial port

## Communication Protocol

The data packet uses a custom protocol with the following format:

| Field | Description |
|-------|-------------|
| Header | 0xAF |
| Function Code | 0x01~0x04 |
| Data | Quaternion/Euler angles/ADC values, etc. |
| Footer | 0xFA |

## Notes

- The code structure is messy; direct reuse should be done with caution
- Recommended only for understanding the project principles
- If reuse is needed, please refactor the code structure yourself

## License

MIT License
