#!/usr/bin/env python3
"""
ArmSync 6-Axis Robot Arm — 3D Visualization
Coords: X+=fwd Y+=left Z+=up (left arm)
"""
import math
import matplotlib.pyplot as plt

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

# ===== Joint axis directions (zero pose: arm hanging down) =====
# J1: Y+   J2: X+   J3: Z-
# J4: Y+   J5: Z-   J6: Y+

# ===== Test cases (shoulder, elbow, wrist in Jetson coords) =====
tests = [
    # Raw Jetson data (sx,sy,sz, ex,ey,ez, wx,wy,wz) in Jetson frame
    (0,0,0,  65,0,-65,  65,-250,-65),          # broken-arm (hand)
    (0,0,0,  -75,-9,-290,  98,-125,-445),      # raw Jetson: extending
    (0,0,0,  -85,4,-288,  80,-156,-419),       # raw Jetson: to side
    (0,0,0,  -80,-6,-289,  89,-148,-426),      # raw Jetson: bending
]

colors = ['red','blue','green','orange']

# ===== 3D plot =====
fig = plt.figure(figsize=(10, 8))
ax = fig.add_subplot(111, projection='3d')

for idx, (sx,sy,sz, ex,ey,ez, wx,wy,wz) in enumerate(tests):
    # Shift to J1 frame (J1->J2 = 121mm along Y+)
    sy += L_J1_J2; ey += L_J1_J2; wy += L_J1_J2

    # J2 position in J1 frame (J1->J2 along Y+ only)
    j2x, j2y, j2z = 0.0, L_J1_J2, 0.0

    c = colors[idx]

    # ---- Spherical projection (preserve DIRECTION, scale LENGTH) ----
    # Upper arm direction from raw data
    uax, uay, uaz = ex - j2x, ey - j2y, ez - j2z
    ua_len = math.hypot(uax, uay, uaz)
    if ua_len > 0.01:
        s = L_UPPER / ua_len
        ex = j2x + uax * s
        ey = j2y + uay * s
        ez = j2z + uaz * s

    # Forearm direction from RAW data (not projected elbow!)
    fax, fay, faz = wx - (j2x+uax), wy - (j2y+uay), wz - (j2z+uaz)  # raw elbow→wrist
    fa_len = math.hypot(fax, fay, faz)
    if fa_len > 0.01:
        s = L_FORE / fa_len
        wx = ex + fax * s
        wy = ey + fay * s
        wz = ez + faz * s

    print(f"Proj: E=({ex:.1f},{ey:.1f},{ez:.1f}) |{math.hypot(ex-j2x,ey-j2y,ez-j2z):.0f}mm|  W=({wx:.1f},{wy:.1f},{wz:.1f}) |{math.hypot(wx-ex,wy-ey,wz-ez):.0f}mm|")

    # ---- J1: horizontal angle of upper arm in XZ plane ----
    j1 = math.atan2(ex - j2x, -(ez - j2z))

    # ---- J2: cosine law on triangle J1-J2-J4, then -90 deg ----
    # J1->J2 is along Y+, arm down at J2=0 is Z-, these are 90 deg apart
    d_j1j4 = math.hypot(ex, ey, ez)
    cosJ2 = (L_J1_J2**2 + L_UPPER**2 - d_j1j4**2) / (2*L_J1_J2*L_UPPER)
    cosJ2 = max(-1, min(1, cosJ2))
    j2_pos = math.acos(cosJ2) - math.pi/2

    # ---- J4: law of cosines on triangle J2-J4-J6 ----
    d_j2j6 = math.hypot(wx-j2x, wy-j2y, wz-j2z)
    cosJ4 = (L_UPPER**2 + L_FORE**2 - d_j2j6**2) / (2*L_UPPER*L_FORE)
    cosJ4 = max(-1, min(1, cosJ4))
    j4 = math.acos(cosJ4) - math.pi   # complementary: always negative (elbow bends back)

    # ---- J3: upper arm twist, from forearm direction vs. bend plane ----
    # Build J4's local frame: Z=upper arm direction, Y=global Y⊥, X=Y×Z
    udx, udy, udz = (ex-j2x)/L_UPPER, (ey-j2y)/L_UPPER, (ez-j2z)/L_UPPER
    # Actually: cross((0,1,0), (udx,udy,udz)) = (udz, 0, -udx)
    j4x_x, j4x_y, j4x_z = udz, 0.0, -udx
    j4x_len = math.hypot(j4x_x, j4x_y, j4x_z)
    if j4x_len > 0.001:
        j4x_x/=j4x_len; j4x_y/=j4x_len; j4x_z/=j4x_len
    else:
        j4x_x,j4x_y,j4x_z = 1,0,0
    # Forearm unit vector (from projected elbow to projected wrist)
    fdx = (wx-ex)/L_FORE; fdy = (wy-ey)/L_FORE; fdz = (wz-ez)/L_FORE
    f_x = fdx*j4x_x + fdy*j4x_y + fdz*j4x_z  # in J4 X
    f_y = fdx*0 + fdy*1 + fdz*0               # in J4 Y (global Y)
    j3 = math.atan2(-f_y, -f_x)

    # ---- Compute J3/J5 positions along arm ----
    j3x = j2x + (ex-j2x) * L_J2_J3 / L_UPPER
    j3y = j2y + (ey-j2y) * L_J2_J3 / L_UPPER
    j3z = j2z + (ez-j2z) * L_J2_J3 / L_UPPER
    j5x = ex + (wx-ex) * L_J4_J5 / L_FORE
    j5y = ey + (wy-ey) * L_J4_J5 / L_FORE
    j5z = ez + (wz-ez) * L_J4_J5 / L_FORE

    # ---- Compute EE (gripper) position along same direction ----
    eex = wx + (wx-ex) * L_J6_EE / L_FORE
    eey = wy + (wy-ey) * L_J6_EE / L_FORE
    eez = wz + (wz-ez) * L_J6_EE / L_FORE

    print(f"J1={j1*R2D:.1f}  J2={j2_pos*R2D:.1f}  J3={j3*R2D:.1f}  J4={j4*R2D:.1f}")

    c = colors[idx]
    ax.scatter([j2x], [j2y], [j2z], c='green', s=60, marker='o', label='Shoulder/J2')
    ax.scatter([ex], [ey], [ez], c=[c], s=60, marker='s', label='Elbow/J4')
    ax.scatter([wx], [wy], [wz], c=[c], s=60, marker='^', label='Wrist/J6')

    # Lines: J1->J2->J3->J4 (upper arm chain)
    ax.plot([0,j2x], [0,j2y], [0,j2z], 'k-', lw=1.5)
    ax.plot([j2x,j3x], [j2y,j3y], [j2z,j3z], color=c, lw=1.5)
    ax.plot([j3x,ex], [j3y,ey], [j3z,ez], color=c, lw=1.5, linestyle=':')
    # Lines: J4->J5->J6 (forearm chain)
    ax.plot([ex,j5x], [ey,j5y], [ez,j5z], color=c, lw=1.5)
    ax.plot([j5x,wx], [j5y,wy], [j5z,wz], color=c, lw=1.5, linestyle=':')

    # J3, J5 points
    ax.scatter([j3x],[j3y],[j3z], c=[c], s=30, marker='D')
    ax.scatter([j5x],[j5y],[j5z], c=[c], s=30, marker='D')
    # EE claw (V-shaped gripper)
    ax.plot([wx,eex], [wy,eey], [wz,eez], color=c, lw=1.5, linestyle='-.')
    # Two fingers
    gx = (eex-wx)*0.15; gy = (eey-wy)*0.15; gz = (eez-wz)*0.15
    px = -(eey-wy)*0.12; py = (eex-wx)*0.12
    ax.plot([eex, eex+px+gx], [eey, eey+py+gy], [eez, eez+gz], color=c, lw=2)
    ax.plot([eex, eex-px+gx], [eey, eey-py+gy], [eez, eez+gz], color=c, lw=2)

# ---- Origin (J1) ----
ax.scatter([0], [0], [0], c='black', s=100, marker='*', label='J1 (origin)')

# ---- Axes with direction arrows ----
origin = (0, 0, 0)
axis_len = 200
arrow_style = dict(arrowstyle='-|>', color='gray', lw=1.5, mutation_scale=15)

for (dx,dy,dz, label) in [
    (axis_len,0,0,'X+ (fwd)'),
    (0,axis_len,0,'Y+ (left)'),
    (0,0,axis_len,'Z+ (up)'),
]:
    ax.quiver(*origin, dx, dy, dz, color='gray', arrow_length_ratio=0.1, linewidth=1.5)
    ax.text(dx*1.05, dy*1.05, dz*1.05, label, color='gray', fontsize=9)

# ---- Aesthetics ----
ax.set_xlabel('X mm'); ax.set_ylabel('Y mm'); ax.set_zlabel('Z mm')
ax.set_title('ArmSync Jetson KeyPoints (in J1 frame)')

# Fix aspect ratio for better perception
max_range = max(
    max(abs(sx) for (sx,_,_,_,_,_,_,_,_) in tests),
    max(abs(sy+121) for (_,sy,_,_,ey,_,_,wy,_) in tests),
    max(abs(sz) for (_,_,sz,_,_,ez,_,_,wz) in tests),
    300
)
ax.set_xlim(-max_range, max_range)
ax.set_ylim(-max_range, max_range)
ax.set_zlim(-max_range*0.5, max_range)

# Remove duplicate legend entries
handles, lbls = ax.get_legend_handles_labels()
by_label = dict(zip(lbls, handles))
ax.legend(by_label.values(), by_label.keys(), loc='upper left')

plt.show()
