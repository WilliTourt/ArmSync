#!/usr/bin/env python3
"""
ArmSync 6-Axis Robot Arm — Real-time 3D Visualization from Jetson Serial
Coords: X+=fwd Y+=left Z+=up (left arm)

Usage: python arm_ik_test.py
  - Connects to Jetson via serial, reads keypoint frames, animates 3D arm in real time.
"""
import math
import struct
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# ===== Serial config =====
SERIAL_PORT = "COM19"
SERIAL_BAUD = 115200

# ===== Jetson frame format (matches UartRecvTask.h) =====
FRAME_HEADER = 0xAA
FRAME_FOOTER = 0x0D
NUM_KEYPOINTS = 4           # elbow / wrist / index / thumb
PAYLOAD_LEN  = NUM_KEYPOINTS * 3 * 2   # 24 bytes (int16 × 3 coords × 4 kp)
FRAME_LEN    = 1 + PAYLOAD_LEN + 1     # 26 bytes total

# ===== Robot arm segment lengths (mm) =====
L_J1_J2 = 121.0
L_J2_J3 = 160.985
L_J3_J4 = 103.2
L_J4_J5 = 95.0
L_J5_J6 = 90.519
L_J6_EE = 143.247

L_UPPER = L_J2_J3 + L_J3_J4   # J2->J4: 264.185
L_FORE  = L_J4_J5 + L_J5_J6   # J4->J6: 185.519
R2D = 180.0 / math.pi

# ===== Serial reader =====
class JetsonReader:
    def __init__(self, port, baud):
        self.ser = None
        try:
            import serial
            self.ser = serial.Serial(port, baud, timeout=0.05)
            print(f"[Serial] Opened {port} @ {baud}")
        except ImportError:
            print("[Serial] pyserial not installed. Run: pip install pyserial")
        except Exception as e:
            print(f"[Serial] Could not open {port}: {e}")

    def read_frame(self):
        """Try to read and parse one valid Jetson frame. Returns (ex,ey,ez, wx,wy,wz) or None."""
        if self.ser is None:
            return None

        try:
            # Sync to header
            while True:
                b = self.ser.read(1)
                if not b:
                    return None
                if b[0] == FRAME_HEADER:
                    break

            # Read remaining 25 bytes
            buf = bytearray([FRAME_HEADER])
            buf += self.ser.read(FRAME_LEN - 1)
            if len(buf) < FRAME_LEN:
                return None

            # Validate footer
            if buf[-1] != FRAME_FOOTER:
                return None

            # Parse 4 keypoints (int16 LE ×3 coords each)
            payload = buf[1:1+PAYLOAD_LEN]
            kp = []
            for i in range(NUM_KEYPOINTS):
                x = int.from_bytes(payload[i*6  :i*6+2], 'little', signed=True)
                y = int.from_bytes(payload[i*6+2:i*6+4], 'little', signed=True)
                z = int.from_bytes(payload[i*6+4:i*6+6], 'little', signed=True)
                kp.append((x, y, z))

            # Return elbow (kp[0]) and wrist (kp[1]) for IK
            return kp[0], kp[1]

        except Exception as e:
            return None

    def close(self):
        if self.ser:
            self.ser.close()

# ===== IK solver (same logic as original) =====
def solve_ik(ex, ey, ez, wx, wy, wz):
    """Return (j1, j2, j3, j4) angles in radians, plus all joint positions."""

    # Shift to J1 frame (J1→J2 is 121mm along Y+)
    ey += L_J1_J2
    wy += L_J1_J2

    j2x, j2y, j2z = 0.0, L_J1_J2, 0.0

    # ---- Spherical projection (preserve direction, scale to segment length) ----
    uax, uay, uaz = ex - j2x, ey - j2y, ez - j2z
    ua_len = math.hypot(uax, uay, uaz)
    if ua_len > 0.01:
        s = L_UPPER / ua_len
        ex_p = j2x + uax * s
        ey_p = j2y + uay * s
        ez_p = j2z + uaz * s
    else:
        ex_p, ey_p, ez_p = ex, ey, ez

    # Forearm direction from RAW data
    fax, fay, faz = wx - (j2x + uax), wy - (j2y + uay), wz - (j2z + uaz)
    fa_len = math.hypot(fax, fay, faz)
    if fa_len > 0.01:
        s = L_FORE / fa_len
        wx_p = ex_p + fax * s
        wy_p = ey_p + fay * s
        wz_p = ez_p + faz * s
    else:
        wx_p, wy_p, wz_p = wx, wy, wz

    # ---- J1 ----
    j1 = math.atan2(ex_p - j2x, -(ez_p - j2z))

    # ---- J2 ----
    d_j1j4 = math.hypot(ex_p, ey_p, ez_p)
    cosJ2 = (L_J1_J2**2 + L_UPPER**2 - d_j1j4**2) / (2 * L_J1_J2 * L_UPPER)
    cosJ2 = max(-1, min(1, cosJ2))
    j2 = math.acos(cosJ2) - math.pi / 2

    # ---- J4 ----
    d_j2j6 = math.hypot(wx_p - j2x, wy_p - j2y, wz_p - j2z)
    cosJ4 = (L_UPPER**2 + L_FORE**2 - d_j2j6**2) / (2 * L_UPPER * L_FORE)
    cosJ4 = max(-1, min(1, cosJ4))
    j4 = math.acos(cosJ4) - math.pi

    # ---- J3 ----
    udx = (ex_p - j2x) / L_UPPER
    udy = (ey_p - j2y) / L_UPPER
    udz = (ez_p - j2z) / L_UPPER
    j4x_x, j4x_y, j4x_z = udz, 0.0, -udx
    j4x_len = math.hypot(j4x_x, j4x_y, j4x_z)
    if j4x_len > 0.001:
        j4x_x /= j4x_len; j4x_y /= j4x_len; j4x_z /= j4x_len
    else:
        j4x_x, j4x_y, j4x_z = 1, 0, 0

    fdx = (wx_p - ex_p) / L_FORE
    fdy = (wy_p - ey_p) / L_FORE
    fdz = (wz_p - ez_p) / L_FORE
    f_x = fdx * j4x_x + fdy * j4x_y + fdz * j4x_z
    f_y = fdx * 0 + fdy * 1 + fdz * 0
    j3 = math.atan2(-f_y, -f_x)

    # ---- Joint positions along the chain ----
    j3x = j2x + (ex_p - j2x) * L_J2_J3 / L_UPPER
    j3y = j2y + (ey_p - j2y) * L_J2_J3 / L_UPPER
    j3z = j2z + (ez_p - j2z) * L_J2_J3 / L_UPPER
    j5x = ex_p + (wx_p - ex_p) * L_J4_J5 / L_FORE
    j5y = ey_p + (wy_p - ey_p) * L_J4_J5 / L_FORE
    j5z = ez_p + (wz_p - ez_p) * L_J4_J5 / L_FORE

    # EE
    eex = wx_p + (wx_p - ex_p) * L_J6_EE / L_FORE
    eey = wy_p + (wy_p - ey_p) * L_J6_EE / L_FORE
    eez = wz_p + (wz_p - ez_p) * L_J6_EE / L_FORE

    return j1, j2, j3, j4, j2x, j2y, j2z, ex_p, ey_p, ez_p, wx_p, wy_p, wz_p, j3x, j3y, j3z, j5x, j5y, j5z, eex, eey, eez


# ===== Main: real-time plot =====
reader = JetsonReader(SERIAL_PORT, SERIAL_BAUD)

fig = plt.figure(figsize=(10, 8))
ax = fig.add_subplot(111, projection='3d')

# Static elements
ax.set_xlabel('X mm'); ax.set_ylabel('Y mm'); ax.set_zlabel('Z mm')
ax.set_title(f'ArmSync IK — Live from Jetson ({SERIAL_PORT})')
ax.set_xlim(-600, 600)
ax.set_ylim(-300, 900)
ax.set_zlim(-600, 300)

origin = (0, 0, 0)
axis_len = 200
for (dx, dy, dz, label) in [
    (axis_len, 0, 0, 'X+'),
    (0, axis_len, 0, 'Y+'),
    (0, 0, axis_len, 'Z+'),
]:
    ax.quiver(*origin, dx, dy, dz, color='gray', arrow_length_ratio=0.1, lw=1.2)
    ax.text(dx * 1.05, dy * 1.05, dz * 1.05, label, color='gray', fontsize=9)

# J1 origin
ax.scatter([0], [0], [0], c='black', s=100, marker='*')

# Title text for joint angles
title_text = ax.text2D(0.02, 0.98, '', transform=ax.transAxes,
                       fontfamily='monospace', fontsize=9, va='top',
                       bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))

# Dynamic scatter/line collections — redrawn each frame
points_artists = []
lines_artists = []

def update(_):
    global points_artists, lines_artists

    # Clear previous frame's dynamic elements
    for a in points_artists:
        a.remove()
    for a in lines_artists:
        a.remove()
    points_artists.clear()
    lines_artists.clear()

    # Drain all available frames, keep last
    result = None
    for _ in range(20):
        r = reader.read_frame()
        if r is not None:
            result = r

    if result is None:
        title_text.set_text('Waiting for Jetson data...')
        return

    (ex, ey, ez), (wx, wy, wz) = result

    j1, j2, j3, j4, \
        j2x, j2y, j2z, \
        ex_p, ey_p, ez_p, \
        wx_p, wy_p, wz_p, \
        j3x, j3y, j3z, \
        j5x, j5y, j5z, \
        eex, eey, eez = solve_ik(ex, ey, ez, wx, wy, wz)

    c = 'blue'

    # Joint scatter points
    pts = [
        (j2x, j2y, j2z, 'green', 'o', 60, 'Shoulder/J2'),
        (ex_p, ey_p, ez_p, c, 's', 60, 'Elbow/J4'),
        (wx_p, wy_p, wz_p, c, '^', 60, 'Wrist/J6'),
        (j3x, j3y, j3z, c, 'D', 30, ''),
        (j5x, j5y, j5z, c, 'D', 30, ''),
    ]
    for (px, py, pz, col, mk, sz, lb) in pts:
        s = ax.scatter([px], [py], [pz], c=col, s=sz, marker=mk, label=lb)
        points_artists.append(s)

    # Chain lines
    chain = [
        ([0, j2x], [0, j2y], [0, j2z], 'k-', 1.5),
        ([j2x, j3x], [j2y, j3y], [j2z, j3z], c, 1.5),
        ([j3x, ex_p], [j3y, ey_p], [j3z, ez_p], c, 1.5),
        ([ex_p, j5x], [ey_p, j5y], [ez_p, j5z], c, 1.5),
        ([j5x, wx_p], [j5y, wy_p], [j5z, wz_p], c, 1.5),
        ([wx_p, eex], [wy_p, eey], [wz_p, eez], c, 1.5),
    ]
    for (xs, ys, zs, col, lw) in chain:
        ln = ax.plot(xs, ys, zs, color=col, lw=lw)[0]
        lines_artists.append(ln)

    # EE claw
    gx = (eex - wx_p) * 0.15; gy = (eey - wy_p) * 0.15; gz = (eez - wz_p) * 0.15
    px = -(eey - wy_p) * 0.12; py = (eex - wx_p) * 0.12
    for (dx, dy) in [(px+gx, py+gy), (-px+gx, -py+gy)]:
        ln = ax.plot([eex, eex+dx], [eey, eey+dy], [eez, eez+gz], color=c, lw=2)[0]
        lines_artists.append(ln)

    # Joint angle text
    title_text.set_text(
        f"J1={j1*R2D:+.1f}°  J2={j2*R2D:+.1f}°  "
        f"J3={j3*R2D:+.1f}°  J4={j4*R2D:+.1f}°  "
        f"J5=0  J6=0\n"
        f"Elbow=({ex_p:.0f},{ey_p:.0f},{ez_p:.0f})  "
        f"Wrist=({wx_p:.0f},{wy_p:.0f},{wz_p:.0f})"
    )

ani = animation.FuncAnimation(fig, update, interval=50, cache_frame_data=False)
plt.show()

reader.close()
