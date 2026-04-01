# Assembly Guide - ArmSync 7DOF Robotic Arm

## Preparation

### Tool List
- Hex key set (M2.5/M3/M5/M6)
- Screwdriver set
- Caliper (optional, for verification)
- Grease (for gearboxes)
- Multimeter (for continuity checks)

### Parts Check
Before assembly, verify all parts against [Parts BOM](./Parts%20BOM.md).

---

## Assembly Steps

### Step 1: J1 Base Rotation Joint

**Parts Required:**
- J1 Base printed part × 1
- 6060 Aluminum Extrusion 600mm × 1
- 57×76mm Stepper Motor × 1
- PRF57-L2-20-P2 Gearbox × 1
- Locking Bushing Z21 14×26×17 × 1
- M6×16 Screws × 10
- M6 T-Nuts × 12
- M5×25 Screws × 8
- M3×31 Screws × 4 (for locking bushing)

**Assembly Procedure:**

1. **Install Aluminum Extrusion Base**
   - Slide the 6060 aluminum extrusion through the bottom slot of J1 Base
   - Secure with M6 T-nuts + M6×16 screws
   - Ensure extrusion is perpendicular to base, tighten screws evenly

2. **Install Gearbox**
   - Apply small amount of grease to gearbox input shaft
   - Place PRF57-L2-20-P2 gearbox (square flange up) into J1 Base circular recess
   - Secure gearbox to base with M5×25 screws

3. **Install Motor**
   - Align 57 stepper motor shaft with gearbox input shaft
   - Install locking bushing Z21 14×26×17 to connect motor shaft and gearbox input shaft
   - Tighten locking bushing with M3×31 screws (diagonal pattern, gradual tightening)

4. **Cable Routing**
   - Route motor cables through side exit hole of base
   - Leave sufficient slack to avoid strain

> **Check Point:** Manually rotate motor shaft to confirm gearbox output shaft rotates synchronously without binding.

---

### Step 2: J2 Shoulder Pitch Joint

**Parts Required:**
- Shoulder printed part × 1
- Upperarm up/down printed parts × 1
- 42×60mm Stepper Motor × 1
- PRF42-L2-30-P2 Gearbox × 1
- Locking Bushing Z21 8×18×11 × 1
- Diaphragm Coupling D32d8 L28 × 1 (or second locking bushing)
- M2.5×24 Screws × 3 (locking bushing)
- M3×20+ Screws × 8
- M3×75+ Screws × 4

**Assembly Procedure:**

1. **Install Gearbox to Shoulder**
   - Place PRF42-L2-30-P2 gearbox (square flange up) into Shoulder part recess
   - Secure with M3 screws (select length based on actual hole positions)

2. **Connect J1 and J2**
   - Align Shoulder part with J1 Base gearbox output shaft
   - Connect using locking bushing or coupling
   - Note: This is the load-bearing joint of the entire arm, tighten securely

3. **Install Motor**
   - Mount 42×60mm motor to Shoulder side motor mount
   - Connect motor shaft and gearbox input shaft with locking bushing Z21 8×18×11
   - Tighten with M2.5×24 screws

4. **Pre-install Upper Arm Upper Section**
   - Connect Upperarm up part to Shoulder
   - Use M3×75+ long screws for through-mounting (do not fully tighten yet, adjustment needed later)

> **Check Point:** Manually turn motor to confirm shoulder pitch motion is smooth without interference.

---

### Step 3: J3 Shoulder Yaw Joint

**Parts Required:**
- Upperarm up/down printed parts (pre-installed in J2)
- 42×40mm Stepper Motor × 1
- PRF42-L1-10-P2 Gearbox (round flange) × 1
- Locking Bushing Z21 8×18×11 × 1
- M2.5×16 Screws × 3
- M3×59+ Screws × 4

**Assembly Procedure:**

1. **Install Gearbox**
   - Place PRF42-L1-10-P2 gearbox (round flange up) into top recess of Upperarm up
   - Secure with M3 screws

2. **Install Motor**
   - Mount 42×40mm motor to Upperarm up side motor mount
   - Connect motor shaft and gearbox input shaft with locking bushing
   - Tighten with M2.5×16 screws

3. **Connect Upper Arm Lower Section**
   - Align Upperarm dn part with gearbox output shaft
   - Connect using locking bushing or coupling
   - Secure with M3×59+ screws

> **Check Point:** Confirm yaw rotation range is normal without mechanical limit conflicts.

---

### Step 4: J4 Elbow Joint

**Parts Required:**
- Forearm up printed part × 1
- 42×48mm Stepper Motor × 1
- PRF42-L1-10-P2 Gearbox (round flange) × 1
- Locking Bushing Z21 8×18×11 × 1
- M2.5×19 Screws × 3
- M3×15+ Screws × 4
- M3×48+ Screws × 4

**Assembly Procedure:**

1. **Install Gearbox to Elbow**
   - Install gearbox to bottom connection of Upperarm dn
   - Secure with M3 screws

2. **Install Motor**
   - Mount 42×48mm motor to elbow motor mount
   - Connect with locking bushing, tighten with M2.5×19 screws

3. **Connect Forearm Upper Section**
   - Align Forearm up with gearbox output shaft
   - Connect using locking bushing
   - Secure with M3×48+ screws

---

### Step 5: J5 Wrist Pitch Joint

**Parts Required:**
- Wrist printed part × 1
- 42×40mm Stepper Motor × 1
- PRF42-L1-10-P2 or PRF42-L1-5-P2 Gearbox × 1
- Locking Bushing Z21 8×18×11 × 1
- M2.5×13 Screws × 3
- M3×53+ Screws × 4

**Assembly Procedure:**

1. **Install Gearbox to Wrist**
   - Install gearbox to end of Forearm up
   - Secure with M3 screws

2. **Install Motor**
   - Mount 42×40mm motor
   - Connect with locking bushing, tighten with M2.5×13 screws

3. **Connect Wrist Structure**
   - Align Wrist part with gearbox output shaft
   - Connect with locking bushing, secure with M3×53+ screws

---

### Step 6: J6 Wrist Roll Joint

**Parts Required:**
- Wrist roll connector × 1
- 42×34mm Stepper Motor × 1
- Diaphragm Coupling D19d4 L20 × 1
- M2.5×17 Screws × 3
- M3×13+ Screws × 4
- M3×48+ Screws × 4

**Assembly Procedure:**

1. **Install Motor**
   - Mount 42×34mm motor to side of Wrist
   - Note: J6 does not use a gearbox, direct drive or timing belt transmission

2. **Connect Roll Shaft**
   - Use Diaphragm Coupling D19d4 L20 to connect motor shaft and roll shaft
   - Ensure both ends of coupling are tight

---

### Step 7: J7 Gripper Assembly

**Parts Required:**
- Gripper components (see 3D Files/Gripper)
- MG90S Servo × 1
- M3×38 Screws × 4
- M2.5×18 Screws × 5

**Assembly Procedure:**

1. **Install Servo**
   - Place MG90S servo into gripper base servo slot
   - Secure with M2.5×18 screws

2. **Install Gear Drive**
   - Connect servo arm to gripper gear mechanism
   - Ensure fingers open and close smoothly

3. **Mount to Wrist**
   - Use M3×38 screws to mount gripper assembly to J6 roll output end

---

### Step 8: Timing Pulley Installation (if used)

**Parts Required:**
- 2GT 20T Timing Pulleys × 6
- 2GT Timing Belts (128-2GT-6 / 128-2GT-10) × 3

**Installation Locations:**
Depending on design, timing pulleys may be used for J5/J6 transmission alternative. Install according to actual design drawings.

---

## Wiring

### CAN Bus Wiring

All motors are connected in series via CAN bus:

```
Controller CAN_H → J1 CAN_H → J2 CAN_H → ... → J7 CAN_H
Controller CAN_L → J1 CAN_L → J2 CAN_L → ... → J7 CAN_L
```

**Notes:**
- 120Ω termination resistors required at both ends of CAN bus
- Recommend twisted pair wire to reduce interference
- Each node address must be configured individually (J1=1, J2=2, ...)

### Power Wiring

- Main Power: 24V DC (based on motor rated voltage)
- Servo Power: 5-6V DC (MG90S)
- Ensure power supply has sufficient capacity to drive all motors simultaneously

---

## Initial Testing

### 1. Pre-Power Check
- [ ] All screws tightened
- [ ] Wiring correct, no shorts
- [ ] All joints move smoothly, no binding
- [ ] Power voltage correct

### 2. Single Joint Test
- [ ] Use CAN analyzer or host software to test joints individually
- [ ] Confirm each motor responds correctly to commands
- [ ] Check rotation direction is correct

### 3. Zero Point Calibration
- [ ] Manually move all joints to mechanical zero position
- [ ] Set zero offset in software
- [ ] Record zero positions for all joints

---

## Notes

1. **Safety First:** Maintain safe distance during testing, avoid sudden movements causing injury
2. **Gradual Power-Up:** First power-up recommend connecting single motor for testing, confirm correct before connecting all
3. **Lubrication Maintenance:** Regularly add grease to gearboxes to maintain smooth transmission
4. **Screw Check:** Check all screws for looseness after running for some time

---

## Troubleshooting

| Problem | Possible Cause | Solution |
|------|----------|----------|
| Motor not rotating | Power not connected, wrong CAN address | Check power and communication settings |
| Joint binding | Screws too tight, part interference | Check assembly clearance, readjust |
| Abnormal noise | Insufficient lubrication, loose screws | Add grease, tighten screws |
| Position inaccurate | Zero offset, mechanical clearance | Recalibrate zero, check locking bushing |

---

## Next Steps

After completing hardware assembly, please refer to [Software](../Software/) directory for firmware flashing and control software development.
