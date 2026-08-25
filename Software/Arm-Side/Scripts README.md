# Arm-Side Python Visualization Tools

This directory contains two Python scripts for robot-arm kinematics debugging and 3D visualization:

- `arm_angle_view.py`: Reads `[FUSION]` joint angles and overlays the Controller/Jetson spatial vectors.
- `arm_ik_test.py`: Reads elbow and wrist coordinates from Jetson, calculates the J1–J4 inverse-kinematics result in real time, and displays the arm model.

These scripts are intended for debugging and data observation. They are not robot-arm firmware and do not directly control the motors.

The input data comes from the debug output in `NormalizeTask.cpp` and `FusionTask.cpp` in the RA8P1 CPU0 project. These debug statements are currently disabled. Uncomment the corresponding code in the source if needed.

Use Python 3.10 or later and install the following dependencies:

```
python -m pip install matplotlib pyserial
```

## `arm_angle_view.py`

This script performs forward visualization using the current robot-arm coordinate system and link lengths:

- Draws the arm skeleton from the `J1`–`J5` angles; `J6` is displayed as `0°` in the current input
- Displays the Controller `E(...)` and `W(...)` reference vectors
- Displays the Jetson `E(...)` and `W(...)` reference vectors
- Displays the coordinate axes: `X+` forward, `Y+` left, and `Z+` up
- Supports both live serial input and text log files

### Serial Input

The default baud rate is `115200`:

```powershell
python .\arm_angle_view.py --port COMxx
python .\arm_angle_view.py -p COMxx
```

If the baud rate is different, use `--baud`:

```powershell
python .\arm_angle_view.py -p COMxx --baud 115200
```

Replace the serial port with the port assigned by the operating system or Device Manager. The script continuously reads serial text and updates the visualization whenever it receives a new `[FUSION]` joint-angle line.

### Log File Mode

After saving the serial output to a text file, run:

```powershell
python .\arm_angle_view.py --file .\log.txt
```

To slow down log playback, add a per-frame delay:

```powershell
python .\arm_angle_view.py -f .\log.txt --slow 0.3
```

The value of `--slow` is in seconds.

## `arm_ik_test.py`

### Function

This script reads elbow and wrist coordinates from the Jetson serial port and then:

- Calculates J1, J2, J3, and J4 from the coordinates
- Projects the upper-arm and forearm lengths onto the model's fixed lengths
- Draws a 3D link model of J1–J6 and the end effector
- Displays the calculated joint angles, elbow coordinates, and wrist coordinates

J5 and J6 are currently displayed as `0°`, so this script is primarily for validating the first four joints' IK and input coordinates.

### Serial Configuration

The script currently uses the following values at the top of the file:

```python
SERIAL_PORT = "COM18"
SERIAL_BAUD = 115200
```

Before use, change `SERIAL_PORT` to the serial port connected to the Jetson, then run the script directly:

```powershell
python .\arm_ik_test.py
```

### Input Text Format

The script reads lines matching the debug output format of `NormalizeTask`:

```text
[NORMALIZED INPUT] Elbow(-124,15,-78), Wrist(-129,-74,154)
```

Coordinates are interpreted in millimeters according to the current project convention. Other unmatched serial output is ignored; only lines containing both `Elbow(...)` and `Wrist(...)` are processed.

### Coordinate System and Model Parameters

The script uses the following coordinate convention:

- `X+`: forward
- `Y+`: left
- `Z+`: up

The arm-length parameters are defined directly in the script and correspond to the parameters in `Armsync_IK_Model.md`. If the mechanical structure or coordinate system changes, update the link lengths and IK calculations in the script accordingly.
