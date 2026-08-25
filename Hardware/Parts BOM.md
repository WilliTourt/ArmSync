# Bill of Materials - ArmSync 6-DOF Robotic Arm

[中文文档](/Hardware/Parts%20BOM-zh-CN.md)

## 1. Motors (J1-J6)

| Joint | Model/Specifications | Type | Price (RMB) | Notes |
|------|----------|------|------------|------|
| J1 | 57×76mm | 57 stepper motor + Zhangdatou closed-loop servo controller (CAN version) | 142 | Base rotation |
| J2 | 42×60mm | 42 stepper motor + Zhangdatou closed-loop servo controller (CAN version) | 97 | Shoulder pitch |
| J3 | 42×40mm | 42 stepper motor + Zhangdatou closed-loop servo controller (CAN version) | 81 | Shoulder yaw |
| J4 | 42×48mm | 42 stepper motor + Zhangdatou closed-loop servo controller (CAN version) | 89 | Elbow |
| J5 | 42×40mm | 42 stepper motor + Zhangdatou closed-loop servo controller (CAN version) | 81 | Wrist pitch |
| J6 | 42×34mm | 42 stepper motor + Zhangdatou closed-loop servo controller (CAN version) | 76 | Wrist rotation |

**Subtotal: RMB 566**

---

## 2. Gearboxes (J1-J6)

PFDE planetary gearboxes are used.

| Joint | Model | Specifications | Gear Ratio | Flange Type | Price (RMB) |
|------|------|------|--------|----------|------------|
| J1 | PRF57-L2-20-P2 | 57, 2-stage | 20:1 | Round flange | 121 |
| J2 | PRF42-L2-30-P2 | 42, 2-stage | 30:1 | Square flange | 104 |
| J3 | PRF42-L1-10-P2 | 42, 1-stage | 10:1 | Round flange | 74 |
| J4 | PRF42-L1-10-P2 | 42, 1-stage | 10:1 | Round flange | 74 |
| J5 | - | - | - | - | - |
| J6 | PRF42-L1-10-P2 or PRF42-L1-5-P2 | 42, 1-stage | 10:1 or 5:1 | Round flange | 74 |
| EE | - | - | - | - | - |

**Subtotal: RMB 447**

---

## 3. Timing Belt and Pulley Drive System

| Name | Specifications | Quantity | Unit Price (RMB) | Subtotal (RMB) | Notes |
|------|------|------|------------|------------|------|
| Timing pulley | 2GT, 20 teeth, 5mm bore, 7mm groove width | 2 | 4 | 8 | - |
| Timing pulley | H3M, 25 teeth, 5mm bore, 11mm groove width | 2 | 6.4 | 12.8 | - |
| Timing pulley | H3M, 25 teeth, 14mm bore, 16mm groove width | 2 | 7.1 | 14.2 | - |
| Timing belt | 128-2GT-6 (6mm wide) | 1 | 1 | 1 | - |
| Timing belt | 162-H3M-10 (10mm wide) | 1 | 3.5 | 3.5 | - |
| Timing belt | 162-H3M-15 (15mm wide) | 1 | 4 | 4 | - |

**Subtotal: RMB 43.5**

---

## 4. Fasteners and Drive Components

### 4.1 Locking Bushings (Not recommended for J1/J2/J3/J4)

| Specifications | Quantity | Unit Price (RMB) | Subtotal (RMB) | Usage |
|------|------|------------|------------|------|
| Z21 8×18×11 | 4 | 14 | 56 | J2/J3/J4/J5 |
| Z21 14×26×17 | 1 | 13 | 13 | J1 |

> **Alternative:** J2/J5 can use a combination of two locking bushings and two single-diaphragm couplings. Honestly, the coupling solution is less prone to shaft slippage.

### 4.2 Couplings

| Specifications | Quantity | Unit Price (RMB) | Subtotal (RMB) | Usage |
|------|------|------------|------------|------|
| Single-diaphragm coupling D32d14 L28 | 1 | 19 | 38 | J1 |
| Single-diaphragm coupling D32d8 L28 | 3 | 19 | 38 | J2/J3/J4 |
| Single-diaphragm coupling D19d4 L20 | 1 | 18 | 18 | J6 |

### 4.3 Shafts

| Name | Specifications | Quantity | Notes |
|------|------|------|------|
| D-shaped shaft | 5mm diameter × 45mm long | 3 | - |

**Subtotal: approximately RMB 120**

---

## 5. End Effector (EE)

The end effector can be configured freely. If the gripper provided in the 3D files is used, an MG90S servo is required (RMB 11).

---

## 6. Screw List (May Be Incomplete)

> A “+” after a screw length means that the actual screw may be longer than the listed value.

### 6.1 Base Aluminum Extrusion

| Specifications | Quantity | Usage |
|------|------|------|
| M6×16 | 10 | Securing the aluminum extrusion |
| M6 T-nut | 12 | Base installation |

| Material | Specifications | Quantity | Notes |
|------|------|------|------|
| Aluminum extrusion | 6060 × 650mm | 1 | Base support |

### 6.2 J1 Base Rotation

| Specifications | Quantity | Usage |
|------|------|------|
| M3×31 | 4 | Securing the locking bushing |
| M5×25 | 8 | - |

### 6.3 J2 Shoulder Pitch

| Specifications | Quantity | Usage |
|------|------|------|
| M2.5×24 | 3 | Securing the locking bushing |
| M3×20+ | 8 | - |
| M3×75+ | 4 | - |

### 6.4 J3 Shoulder Yaw

| Specifications | Quantity | Usage |
|------|------|------|
| M2.5×16 | 3 | Securing the locking bushing |
| M3×59+ | 4 | - |

### 6.5 J4 Elbow

| Specifications | Quantity | Usage |
|------|------|------|
| M2.5×19 | 3 | Securing the locking bushing |
| M3×15+ | 4 | - |
| M3×48+ | 4 | - |

### 6.6 J5 Wrist Pitch

| Specifications | Quantity | Usage |
|------|------|------|
| M2.5×13 | 3 | Securing the locking bushing |
| M3×53+ | 4 | - |

### 6.7 J6 Wrist Rotation

| Specifications | Quantity | Usage |
|------|------|------|
| M2.5×17 | 3 | Securing the locking bushing |
| M3×13+ | 4 | - |
| M3×48+ | 4 | - |

### 6.8 EE End Gripper

| Specifications | Quantity | Usage |
|------|------|------|
| M3×38 | 4 | - |
| M2.5×18 | 5 | - |

---

## 7. Development and Debugging Tools (Optional)

| Name | Unit Price (RMB) | Quantity | Subtotal (RMB) | Notes |
|------|------------|------|------------|------|
| CAN transceiver module | 3 | Several | - | - |
| CAN tester (ZDT) | 50 | 1 | 50 | Bus analysis and debugging |

**Subtotal: RMB 53**

---

## Total

| Category | Estimated Total (RMB) |
|------|----------------|
| Motors | 577 |
| Gearboxes | 447 |
| Timing belts and pulleys | 43.5 |
| Fasteners/drive components | 120 |
| Development tools | 53 |
| **Total** | **1,240.5 RMB** |

> **Note:** Screws, aluminum extrusion and other standard parts, 3D-printing costs for structural parts, PCB components, and shipping are not included. The actual estimated cost is RMB 1,500.
