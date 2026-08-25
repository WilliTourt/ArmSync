# Arm-Side Python 可视化工具使用说明

本目录下有两个用于机械臂运动学调试和三维可视化的 Python 脚本：

- `arm_angle_view.py`：读取 `[FUSION]` 关节角度，并叠加显示 Controller/Jetson 的空间向量。
- `arm_ik_test.py`：读取 Jetson 输出的肘部、腕部坐标，实时计算 J1～J4 的逆运动学结果并显示机械臂模型。

这两个脚本主要用于调试和观察数据，不是机械臂固件，也不会直接控制电机。

它们读取的信息是 RA8P1 CPU0 工程中的 `NormalizeTask.cpp` 和 `FusionTask.cpp` 的调试输出，目前是关闭的。如果需要，请将代码中对应位置的调试代码解注释。

使用 Python 3.10 或更高版本，并安装以下依赖：

```
python -m pip install matplotlib pyserial
```

## `arm_angle_view.py`

该脚本使用当前机械臂坐标系和连杆长度进行正向显示：

- 根据 `J1`～`J5` 角度绘制机械臂骨架；当前输入中 `J6` 默认显示为 `0°`
- 显示 Controller 的 `E(...)`、`W(...)` 参考向量
- 显示 Jetson 的 `E(...)`、`W(...)` 参考向量
- 显示坐标轴：`X+` 向前、`Y+` 向左、`Z+` 向上
- 支持实时串口输入和文本日志输入

### 串口

默认波特率为 `115200`：

```powershell
python .\arm_angle_view.py --port COMxx
python .\arm_angle_view.py -p COMxx
```
如果波特率不同，可以使用 `--baud`：

```powershell
python .\arm_angle_view.py -p COMxx --baud 115200
```

串口号需要根据设备管理器或系统实际分配的端口修改。脚本会持续读取串口文本，并在收到新的 `[FUSION]` 关节角度后更新一帧画面。

### 日志文件模式

将串口输出保存为文本后，可以使用：

```powershell
python .\arm_angle_view.py --file .\log.txt
```

为了降低日志播放速度，可以增加每帧延时：

```powershell
python .\arm_angle_view.py -f .\log.txt --slow 0.3
```

`--slow` 的单位是秒。

## `arm_ik_test.py`

### 功能

从 Jetson 串口读取肘部和腕部坐标，然后：

- 根据坐标计算 J1、J2、J3、J4
- 将上臂和前臂长度投影到模型的固定长度
- 绘制 J1～J6 和末端执行器的三维连杆模型
- 显示当前计算出的关节角度、肘部坐标和腕部坐标

该脚本当前将 `J5`、`J6` 显示为 `0°`，因此它主要用于验证前四个关节的 IK 和输入坐标。

### 3.2 修改串口配置

脚本顶部目前使用：

```python
SERIAL_PORT = "COM18"
SERIAL_BAUD = 115200
```

使用前请根据 Jetson 实际连接的串口修改 `SERIAL_PORT`，直接运行

### 输入文本格式

脚本读取与 `NormalizeTask` 调试输出相匹配的文本行：

```text
[NORMALIZED INPUT] Elbow(-124,15,-78), Wrist(-129,-74,154)
```

坐标单位按当前工程约定为毫米。会忽略其他不匹配的串口输出，只处理同时包含 `Elbow(...)` 和 `Wrist(...)` 的行。

### 坐标系和模型参数

当前脚本使用以下坐标约定：

- `X+`：向前
- `Y+`：向左
- `Z+`：向上

机械臂的长度参数直接写在脚本中，并与 `Armsync_IK_Model.md` 中的参数对应。如果机械结构或坐标系发生变化，需要同步修改脚本中的连杆长度和 IK 计算逻辑。
