#!/usr/bin/env python3
"""
ArmSync 双链叠加可视化（关节严格单轴旋转版）
  1. [FUSION OUTPUT] J1..J5 角度 → 6 关节骨架(蓝)，每个角度只绕自己的轴旋转
  2. [INFO] Controller: E(...) W(...) 手柄参考线(红/绿)，E 长度350、W 从E延伸260

关节结构（Armsync_IK_Model.md）：
  零位：整臂自然下垂指向 Z-。X+=前 Y+=左(左臂) Z+=上
  J1 在原点(0,0,0)基座；每段长度见下。
  旋转：右手螺旋定则，拇指指向电机轴方向，逆时针为正。
    J1 轴 Y+  ±90      J2 轴 X+  -22.5~+125   J3 轴 Z-  ±90
    J4 轴 Y+  -122~0   J5 轴 Z-  ±90          J6 轴 Y+  ±90

连杆长度(mm)：J1→J2=121  J2→J3=160.985  J3→J4=103.2
              J4→J5=95   J5→J6=90.519   J6→EE=143.247

用法：
  python arm_angle_view.py -p COM19
  python arm_angle_view.py -f log.txt --slow 0.3
"""
import math, re, sys, argparse, time
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from mpl_toolkits.mplot3d import Axes3D  # noqa

D2R = math.pi / 180.0

# ===== 连杆长度 (mm, 顺序: J1→J2→J3→J4→J5→J6→EE) =====
SEG_LEN = [121.0, 160.985, 103.2, 95.0, 90.519, 143.247]

# 各关节的旋转轴（'X','Y','Z'，负号=反向绕）
# J1:Y+  J2:X+  J3:Z-  J4:Y+  J5:Z-  J6:Y+
# 修正：J1 正角度应往 X+ 走（原 rot_y 反了），用 Y-
JOINT_AXIS = ['Y-', 'X', 'Z-', 'Y', 'Z-', 'Y']

# ===== Controller E/W 长度 =====
L_E = 350.0
L_W = 260.0

# ===== 正则 =====
FUSION_RE = re.compile(
    r'J1\s*=\s*(-?\d+(?:\.\d+)?)\s+'
    r'J2\s*=\s*(-?\d+(?:\.\d+)?)\s+'
    r'J3\s*=\s*(-?\d+(?:\.\d+)?)\s+'
    r'J4\s*=\s*(-?\d+(?:\.\d+)?)\s+'
    r'J5\s*=\s*(-?\d+(?:\.\d+)?)'
)
# 带行标签区分 Controller / Jetson（都是 E(...) W(...)，靠前缀区分）
_EW = (
    r'[Ee]\s*\(\s*(-?\d+(?:\.\d+)?)\s*,\s*(-?\d+(?:\.\d+)?)\s*,\s*(-?\d+(?:\.\d+)?)\s*\)'
    r'\s*'
    r'[Ww]\s*\(\s*(-?\d+(?:\.\d+)?)\s*,\s*(-?\d+(?:\.\d+)?)\s*,\s*(-?\d+(?:\.\d+)?)\s*\)'
)
CTRL_RE = re.compile(r'Controller\s*:?\s*' + _EW)
JETSON_RE = re.compile(r'Jetson\s*:?\s*' + _EW)


def rot_x(a):
    c, s = math.cos(a), math.sin(a)
    return ((1, 0, 0), (0, c, -s), (0, s, c))


def rot_y(a):
    c, s = math.cos(a), math.sin(a)
    return ((c, 0, s), (0, 1, 0), (-s, 0, c))


def rot_z(a):
    c, s = math.cos(a), math.sin(a)
    return ((c, -s, 0), (s, c, 0), (0, 0, 1))


def mmul(A, B):
    return tuple(tuple(sum(A[i][k] * B[k][j] for k in range(3)) for j in range(3))
                 for i in range(3))


def mvec(M, v):
    return tuple(sum(M[i][j] * v[j] for j in range(3)) for i in range(3))


def fk(angles):
    """给定 J1~J6 角度(度) → 6 关节点 + EE (7点, mm)。
    每个关节绕自己的轴旋转，零位整臂下垂(-Z)，逐段累积方向并伸出长度。"""
    # 链旋转矩阵：第 i 关节的角度在"前 i 个关节已摆好"的基础上累积。
    # 实际旋转应作用于后续所有段，因此我们维护全局累积旋转 R，
    # 每段伸出方向 = R 作用到 (0,0,-1)。
    R = ((1, 0, 0), (0, 1, 0), (0, 0, 1))
    pts = [(0.0, 0.0, 0.0)]  # J1=原点(基座)

    def step(axis, ang_deg, length, base_dir='down', add_point=True):
        """base_dir: 'down'=沿累积旋转的 -Z(下垂,默认); 'upY'=沿 Y+(固定竖杆,不随旋转变)
        add_point=False 时只累积旋转不追加关节点点"""
        nonlocal R
        a = ang_deg * D2R
        if axis == 'X':
            R = mmul(R, rot_x(a))
        elif axis == 'Y':
            R = mmul(R, rot_y(a))
        elif axis == 'Y-':
            R = mmul(R, rot_y(-a))
        elif axis == 'Z-':
            R = mmul(R, rot_z(-a))
        if not add_point:
            return
        if base_dir == 'upY':
            d = (0.0, 1.0, 0.0)  # 竖杆固定沿全局 Y+，不随旋转变
        else:
            d = mvec(R, (0, 0, -1))
        p = pts[-1]
        pts.append((p[0] + d[0] * length, p[1] + d[1] * length, p[2] + d[2] * length))

    # J1→J2 是沿 Y+ 的固定竖杆(121mm)，红点固定在 (0,121,0)
    pts.append((0.0, 121.0, 0.0))  # J2 红点固定
    # J1 旋转只累积旋转、不加点（对竖杆本身无影响，但影响后续所有段）
    step(JOINT_AXIS[0], angles[0], 0.0, add_point=False)
    for i in range(1, 6):
        step(JOINT_AXIS[i], angles[i], SEG_LEN[i])
    return pts  # [J1,J2,J3,J4,J5,J6,EE]


def scale_vec(x, y, z, length):
    n = math.hypot(x, y, z)
    if n < 1e-9:
        return (0.0, 0.0, 0.0)
    s = length / n
    return (x * s, y * s, z * s)


class Source:
    def __init__(self, port=None, baud=115200, file=None):
        self.is_file = file is not None
        self.f = None
        self.buf = ''
        # 各数据独立保存最新值（缺的不更新）
        self.angles = None
        self.e_pt = None
        self.w_pt = None
        self.j_e = None
        self.j_w = None
        if self.is_file:
            self.f = open(file, 'r', encoding='utf-8', errors='ignore')
            print(f"[Source] Reading from file: {file}")
        else:
            import serial
            self.f = serial.Serial(port, baud, timeout=0.05)
            print(f"[Source] Opened {port} @ {baud}")

    def read_line(self):
        if self.is_file:
            return self.f.readline()
        while True:
            b = self.f.read(1)
            if not b:
                return None
            ch = b.decode('utf-8', errors='ignore')
            if ch == '\n':
                line = self.buf
                self.buf = ''
                return line
            self.buf += ch

    def next_frame(self):
        """读行并独立更新各数据源。遇到新的 [FUSION] 角度就立即返回（避免空等），
        途中随手吸收 Controller/Jetson 参考线。读不到新角度就用当前已缓存值。"""
        got_new_fusion = False
        for _ in range(400):
            line = self.read_line()
            if line is None:
                break
            m = FUSION_RE.search(line)
            if m:
                self.angles = [float(m.group(i)) for i in range(1, 6)] + [0.0]
                got_new_fusion = True
                break  # 有新手角度就够画一帧了
            m = CTRL_RE.search(line)
            if m:
                ex, ey, ez = (float(m.group(i)) for i in (1, 2, 3))
                wx, wy, wz = (float(m.group(i)) for i in (4, 5, 6))
                self.e_pt = scale_vec(ex, ey, ez, L_E)
                self.w_pt = scale_vec(wx, wy, wz, L_W)
                continue
            m = JETSON_RE.search(line)
            if m:
                ex, ey, ez = (float(m.group(i)) for i in (1, 2, 3))
                wx, wy, wz = (float(m.group(i)) for i in (4, 5, 6))
                self.j_e = scale_vec(ex, ey, ez, L_E)
                self.j_w = scale_vec(wx, wy, wz, L_W)
        if self.angles is None:
            return None
        return (self.angles, self.e_pt, self.w_pt, self.j_e, self.j_w)

    def close(self):
        if self.f:
            try:
                self.f.close()
            except Exception:
                pass


def main():
    ap = argparse.ArgumentParser(description='ArmSync 双链叠加 (单轴旋转骨架 + E/W参考)')
    ap.add_argument('-p', '--port', default=None)
    ap.add_argument('-b', '--baud', type=int, default=115200)
    ap.add_argument('-f', '--file', default=None)
    ap.add_argument('--slow', type=float, default=0.0, help='文件模式每帧间隔(秒)')
    args = ap.parse_args()

    if not args.file and not args.port:
        print("请指定 -p COMxx (串口) 或 -f log.txt (文件)")
        sys.exit(1)

    src = Source(port=args.port, baud=args.baud, file=args.file)

    fig = plt.figure(figsize=(11, 9))
    ax = fig.add_subplot(111, projection='3d')
    ax.set_xlabel('X mm (fwd)'); ax.set_ylabel('Y mm (left)'); ax.set_zlabel('Z mm (up)')
    ax.set_title(f'ArmSync joints + E/W ref - {(args.port or args.file)}')
    lim = 450
    ax.set_xlim(-lim, lim); ax.set_ylim(-lim, lim); ax.set_zlim(-lim, lim)
    ax.set_box_aspect((1, 1, 1))

    o = (0, 0, 0)
    for (dx, dy, dz, lab) in [(300, 0, 0, 'X+'), (0, 300, 0, 'Y+'), (0, 0, 300, 'Z+')]:
        ax.quiver(*o, dx, dy, dz, color='gray', arrow_length_ratio=0.1, lw=1.2)
        ax.text(dx * 1.05, dy * 1.05, dz * 1.05, lab, color='gray', fontsize=9)
    ax.scatter([0], [0], [0], c='k', s=130, marker='*')

    joint_labels = ['J1', 'J2', 'J3', 'J4', 'J5', 'J6', 'EE']
    joint_colors = ['black', 'red', 'darkorange', 'green', 'blue', 'purple', 'brown']

    title_text = ax.text2D(0.02, 0.97, '', transform=ax.transAxes,
                           fontfamily='monospace', fontsize=10, va='top',
                           bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.85))

    # ===== 预创建静态 artist，之后只更新数据（避免每帧删建，省性能） =====
    arm_line, = ax.plot([], [], [], color='dodgerblue', lw=3, label='arm joints')
    # 预创建：给足与颜色数一致的点(先全置0)，之后每帧 set_offsets 覆盖
    _placeholder = [(0.0, 0.0)] * len(joint_colors)
    _placeholder_z = [0.0] * len(joint_colors)
    joint_scatter = ax.scatter([p[0] for p in _placeholder],
                               [p[1] for p in _placeholder],
                               _placeholder_z, c=joint_colors, s=75, marker='o')

    # Controller (红绿)
    e_line, = ax.plot([], [], [], color='red', lw=2, ls='--', label='controller E')
    e_pt_scatter = ax.scatter([0], [0], [0], c='red', s=110, marker='^')
    w_line, = ax.plot([], [], [], color='limegreen', lw=2, ls='--', label='controller W')
    w_pt_scatter = ax.scatter([0], [0], [0], c='limegreen', s=110, marker='v')
    # Jetson (青)
    je_line, = ax.plot([], [], [], color='cyan', lw=2, ls='-.', label='Jetson E')
    je_pt_scatter = ax.scatter([0], [0], [0], c='cyan', s=110, marker='^')
    jw_line, = ax.plot([], [], [], color='cyan', lw=2, ls='-.', label='Jetson W')
    jw_pt_scatter = ax.scatter([0], [0], [0], c='cyan', s=110, marker='v')

    def set_line(ln, p1, p2):
        ln.set_data([p1[0], p2[0]], [p1[1], p2[1]])
        ln.set_3d_properties([p1[2], p2[2]], zdir='z')

    def set_scatter(sc, p):
        sc.set_offsets([(p[0], p[1])])
        sc.set_3d_properties([p[2]], zdir='z')

    def update(_):
        f = src.next_frame()
        if f is None:
            return
        angles, e_pt, w_pt, j_e, j_w = f

        # --- FUSION 骨架 ---
        pts = fk(angles)
        xs = [p[0] for p in pts]; ys = [p[1] for p in pts]; zs = [p[2] for p in pts]
        arm_line.set_data(xs, ys)
        arm_line.set_3d_properties(zs, zdir='z')
        joint_scatter.set_offsets(list(zip(xs, ys)))
        joint_scatter.set_3d_properties(zs, zdir='z')

        # --- Controller E/W (E 从 J2 红点 (0,121,0) 起) ---
        sx, sy, sz = 0.0, 121.0, 0.0
        if e_pt is not None:
            e_end = (sx + e_pt[0], sy + e_pt[1], sz + e_pt[2])
            set_line(e_line, (sx, sy, sz), e_end)
            set_scatter(e_pt_scatter, e_end)
            if w_pt is not None:
                w_end = (e_end[0] + w_pt[0], e_end[1] + w_pt[1], e_end[2] + w_pt[2])
                set_line(w_line, e_end, w_end)
                set_scatter(w_pt_scatter, w_end)
                w_final = w_end
            else:
                w_final = None
        else:
            w_final = None

        # --- Jetson E/W (青色) ---
        if j_e is not None:
            je_end = (sx + j_e[0], sy + j_e[1], sz + j_e[2])
            set_line(je_line, (sx, sy, sz), je_end)
            set_scatter(je_pt_scatter, je_end)
            if j_w is not None:
                jw_end = (je_end[0] + j_w[0], je_end[1] + j_w[1], je_end[2] + j_w[2])
                set_line(jw_line, je_end, jw_end)
                set_scatter(jw_pt_scatter, jw_end)

        js = '  '.join(f'{joint_labels[i]}={angles[i]:+.1f}°' for i in range(5)) \
             + f'  J6={angles[5]:+.1f}°'
        txt = js
        if e_pt is not None:
            txt += f"\nE=({e_pt[0]:.0f},{e_pt[1]:.0f},{e_pt[2]:.0f})"
        if w_final is not None:
            txt += f"  W=({w_final[0]:.0f},{w_final[1]:.0f},{w_final[2]:.0f})"
        title_text.set_text(txt)

    if args.slow:
        def _upd(_):
            update(_)
            time.sleep(args.slow)
        fn = _upd
    else:
        fn = update

    ani = animation.FuncAnimation(fig, fn, interval=100, cache_frame_data=False,
                                  blit=False)
    import matplotlib.patches as mpatches
    handles = [mpatches.Patch(color='dodgerblue', label='arm joints (J1~J5)'),
               mpatches.Patch(color='red', label='controller E (350mm)'),
               mpatches.Patch(color='limegreen', label='controller W (260mm, from E)'),
               mpatches.Patch(color='cyan', label='Jetson E/W (350/260)')]
    ax.legend(handles=handles, loc='upper right', fontsize=8)
    plt.tight_layout()
    plt.show()

    src.close()


if __name__ == '__main__':
    main()
