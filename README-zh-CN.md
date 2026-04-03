# ArmSync

一款 7 自由度无线遥操作人形机械臂，采用双 IMU 主从控制方案，为瑞萨电子设计竞赛而设计。
如果你同样要做一个机械臂项目，欢迎参考我们的设计和代码，但请注意它们的局限性。所有文件都按原样提供。

[English Document](README.md)

---

## 项目简介

ArmSync 是一款 3D 打印机械臂，具有以下特点：

- **7 自由度**：肩部（3DOF）+ 肘部（1DOF）+ 腕部（2DOF）+ 夹爪（1DOF）
- **双 IMU 控制**：使用基于 IMU 的运动追踪进行无线主从操作
- **CAN 总线步进电机**：配备高扭矩减速箱的精确控制
- **设计负载**：约500g，适合轻量级抓取任务

## 技术参数

| 参数 | 数值 |
|------|------|
| 总自由度 | 7 |
| 臂展 | ~50cm（前臂 30cm + 上臂 20cm）|
| 负载能力 | 500g |
| 控制方式 | 无线 IMU 遥操作 |
| 执行器 | CAN 总线步进电机 + 减速箱 |
| 材料 | PLA（3D 打印）|

## 仓库结构

```
ArmSync/
├── Hardware/                   # 机械设计文件
│   ├── 3D Files/               # CAD 文件
│   │   ├── 3MF for printing/   # 可直接打印的 3MF 文件
│   │   ├── SolidWorks 2026/    # 原始 CAD 文件
│   │   └── STEP and STL/       # 导出格式
│   ├── example_imgs/           # 组装示例图片 (J1-J7)
│   ├── PCBs for Reference/     # 参考 PCB 文件
│   │   ├── EasyEDA_LCEDA/      # 立创 EDA 工程文件
│   │   └── Imgs/               # PCB 图片和原理图
│   ├── Assembly Guide-zh-CN.md # 中文组装指南
│   ├── Assembly Guide.md       # 英文组装指南
│   ├── Parts BOM-zh-CN.md      # 中文零件清单
│   ├── Parts BOM.md            # 英文零件清单
│   ├── readme-zh-CN.md         # 硬件说明（中文）
│   └── readme.md               # 硬件说明（英文）
├── Software/                   # 固件和控制软件
│   ├── Arm_vscode/             # 机械臂主控程序（RA6M5）
│   ├── Ctrller_keil/           # 手部控制器程序（RA4M1）
│   ├── IMU_keil/               # IMU 模块程序（RA4M1）
│   ├── readme-zh-CN.md         # 软件说明（中文）
│   └── readme.md               # 软件说明（英文）
├── LICENSE                     # MIT 许可证
├── README-zh-CN.md             # 本文档（中文）
└── README.md                   # 本文档（英文）
```

## 快速开始

### 硬件文档

- [零件清单](Hardware/Parts%20BOM-zh-CN.md) - 所有需要的零件和一些供应商信息
- [组装指南](Hardware/Assembly%20Guide-zh-CN.md) - 对组装步骤的简要说明
- [硬件说明](Hardware/readme-zh-CN.md) - 机械结构设计说明

### 打印零件

1. 下载 `Hardware/3D Files/3MF for printing/Renesas ArmSync.3mf`
2. 导入 Bambu Studio 或其他支持 3MF 的切片软件
3. 检查打印设置并根据你的打印机进行调整
4. 打印所有零件

对于其他 CAD 软件，请使用 `STEP and STL/` 文件夹中的文件。

### PCB

参考 `Hardware/PCBs for Reference/` 中的立创 EDA 工程文件和图片。仅供参考。

### 软件

参考 `Software/readme-zh-CN.md` 了解三个部分的工程。

⚠️ **注意**：软件代码仅为记录目的，由于比赛期间时间紧迫，代码写得较为仓促，参考价值有限。

## 许可证

MIT License - 详见 [LICENSE](LICENSE) 文件

## 致谢

感谢瑞萨电子提供的技术支持和竞赛平台。
