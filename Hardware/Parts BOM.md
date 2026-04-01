# Parts BOM - ArmSync 7DOF Robotic Arm Bill of Materials

## 1. Motors (J1-J7)

| Joint | Model/Specs | Type | Price (RMB) | Notes |
|------|----------|------|------------|------|
| J1 | 57×76mm | 57 Stepper Motor + Zhangdatou Closed-Loop Servo Controller (CAN Version) | 142 | Base rotation |
| J2 | 42×60mm | 42 Stepper Motor + Zhangdatou Closed-Loop Servo Controller (CAN Version) | 97 | Shoulder pitch |
| J3 | 42×40mm | 42 Stepper Motor + Zhangdatou Closed-Loop Servo Controller (CAN Version) | 81 | Shoulder yaw |
| J4 | 42×48mm | 42 Stepper Motor + Zhangdatou Closed-Loop Servo Controller (CAN Version) | 89 | Elbow |
| J5 | 42×40mm | 42 Stepper Motor + Zhangdatou Closed-Loop Servo Controller (CAN Version) | 81 | Wrist pitch |
| J6 | 42×34mm | 42 Stepper Motor + Zhangdatou Closed-Loop Servo Controller (CAN Version) | 76 | Wrist roll |
| J7 | MG90S | Servo | 11 | Gripper |

**Subtotal: 577 RMB**

---

## 2. Gearboxes (J1-J6)

Using Pufeide Electric Planetary Gearboxes

| Joint | Model | Specs | Gear Ratio | Flange Type | Price (RMB) |
|------|------|------|--------|----------|------------|
| J1 | PRF57-L2-20-P2 | 57 Frame, 2-Stage | 20:1 | Square Flange | 121 |
| J2 | PRF42-L2-30-P2 | 42 Frame, 2-Stage | 30:1 | Square Flange | 104 |
| J3 | PRF42-L1-10-P2 | 42 Frame, 1-Stage | 10:1 | Round Flange | 74 |
| J4 | PRF42-L1-10-P2 | 42 Frame, 1-Stage | 10:1 | Round Flange | 74 |
| J5 | PRF42-L1-10-P2 or PRF42-L1-5-P2 | 42 Frame, 1-Stage | 10:1 or 5:1 | Round Flange | 74 |
| J6 | - | - | - | - | - |

> **Note:** J6 wrist roll joint does not use a gearbox (direct drive or alternative transmission)

**Subtotal: 447 RMB**

---

## 3. Timing Belt/Pulley Drive System

| Name | Specs | Qty | Unit Price (RMB) | Subtotal (RMB) | Notes |
|------|------|------|------------|------------|------|
| Timing Pulley | 2GT 20T 5mm Bore 7mm Belt Width | 4 | 4 | 16 | - |
| Timing Pulley | 2GT 20T 5mm Bore 11mm Belt Width | 2 | 5 | 10 | - |
| Timing Belt | 128-2GT-6 (6mm width) | 2 | 1 | 2 | - |
| Timing Belt | 128-2GT-10 (10mm width) | 1 | 2 | 2 | - |

> **Alternative:** 126 length belts can be used for tighter tension but are harder to install

**Subtotal: 30 RMB**

---

## 4. Fasteners and Drive Components

### 4.1 Locking Bushings

| Specs | Qty | Unit Price (RMB) | Subtotal (RMB) | Usage |
|------|------|------------|------------|------|
| Z21 8×18×11 | 4 | 14 | 56 | J2/J3/J4/J5 |
| Z21 14×26×17 | 1 | 13 | 13 | J1 |

> **Alternative:** J2-J5 can use 2×bushings + 2×diaphragm couplings combination

### 4.2 Couplings

| Specs | Qty | Unit Price (RMB) | Subtotal (RMB) | Usage |
|------|------|------------|------------|------|
| Diaphragm Coupling D32d8 L28 | 2 | 19 | 38 | Alternative option |
| Diaphragm Coupling D19d4 L20 | 1 | 18 | 18 | J6 |

### 4.3 Shafts

| Name | Specs | Qty | Notes |
|------|------|------|------|
| D-Shaft | 5mm diameter × 45mm length | 3 | - |

**Subtotal: 125 RMB** (107 RMB with alternative option)

---

## 5. Screw List

### 5.1 J7 End Gripper

| Specs | Qty | Usage |
|------|------|------|
| M3×38 | 4 | - |
| M2.5×18 | 5 | - |

### 5.2 J6 Wrist Roll

| Specs | Qty | Usage |
|------|------|------|
| M2.5×17 | 3 | Locking bushing |
| M3×13+ | 4 | - |
| M3×48+ | 4 | - |

### 5.3 J5 Wrist Pitch

| Specs | Qty | Usage |
|------|------|------|
| M2.5×13 | 3 | Locking bushing |
| M3×53+ | 4 | - |

### 5.4 J4 Elbow

| Specs | Qty | Usage |
|------|------|------|
| M2.5×19 | 3 | Locking bushing |
| M3×15+ | 4 | - |
| M3×48+ | 4 | - |

### 5.5 J3 Shoulder Yaw

| Specs | Qty | Usage |
|------|------|------|
| M2.5×16 | 3 | Locking bushing |
| M3×59+ | 4 | - |

### 5.6 J2 Shoulder Pitch

| Specs | Qty | Usage |
|------|------|------|
| M2.5×24 | 3 | Locking bushing |
| M3×20+ | 8 | - |
| M3×75+ | 4 | - |

### 5.7 J1 Base Rotation

| Specs | Qty | Usage |
|------|------|------|
| M3×31 | 4 | Locking bushing |
| M5×25 | 8 | - |

### 5.8 Base Aluminum Extrusion

| Specs | Qty | Usage |
|------|------|------|
| M6×16 | 10 | Extrusion mounting |
| M6 T-Nut | 12 | Floor mounting |

| Material | Specs | Qty | Notes |
|------|------|------|------|
| Aluminum Extrusion | 6060 × 600mm | 1 | Base support |

---

## 6. Development Tools

| Name | Unit Price (RMB) | Qty | Subtotal (RMB) | Notes |
|------|------------|------|------------|------|
| CAN Transceiver Module | 3 | Several | - | For debugging |
| CAN Analyzer (ZDT) | 50 | 1 | 50 | Bus analysis |

**Subtotal: 53 RMB**

---

## Total Cost

| Category | Estimated Total (RMB) |
|------|----------------|
| Motors | 577 |
| Gearboxes | 447 |
| Timing Belts/Pulleys | 30 |
| Fasteners/Drive Components | 125 |
| Development Tools | 53 |
| **Total** | **~1,232 RMB** |

> **Note:** Screws, aluminum extrusion, and other standard parts pricing not included. Actual cost may vary.

---

## Purchasing Recommendations

1. **Motors:** Zhangdatou closed-loop stepper motors, CAN version for multi-axis control
2. **Gearboxes:** Pufeide Electric planetary gearboxes, P2 precision grade
3. **Locking Bushings:** Z21 series are standard parts, available on multiple platforms
4. **Screws:** Recommended to purchase 1.5× the listed quantity to account for loss/damage
