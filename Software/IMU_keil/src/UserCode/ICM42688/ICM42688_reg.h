#ifndef __ICM42688_REG_H__
#define __ICM42688_REG_H__

#define ICM42688_I2C_ADDR_LOW     0x68  // I2C地址（AD0接地）
#define ICM42688_I2C_ADDR_HIGH    0x69  // I2C地址（AD0接3.3V）

#define ICM42688_WHO_AM_I         0x47  // 设备ID固定值

// *******************************
// (User Bank 0) 寄存器
// *******************************
#define REG_DEVICE_CONFIG         0x11  // 设备配置（SPI模式、软件复位等）
#define REG_DRIVE_CONFIG          0x13  // 接口驱动配置（I²C/SPI slew rate）
#define REG_INT_CONFIG            0x14  // 中断引脚配置（模式、驱动、极性）
#define REG_FIFO_CONFIG           0x16  // FIFO工作模式选择
#define REG_TEMP_DATA1            0x1D  // 温度数据高字节
#define REG_TEMP_DATA0            0x1E  // 温度数据低字节
#define REG_ACCEL_DATA_X1         0x1F  // 加速度计X轴数据高字节
#define REG_ACCEL_DATA_X0         0x20  // 加速度计X轴数据低字节
#define REG_ACCEL_DATA_Y1         0x21  // 加速度计Y轴数据高字节
#define REG_ACCEL_DATA_Y0         0x22  // 加速度计Y轴数据低字节
#define REG_ACCEL_DATA_Z1         0x23  // 加速度计Z轴数据高字节
#define REG_ACCEL_DATA_Z0         0x24  // 加速度计Z轴数据低字节
#define REG_GYRO_DATA_X1          0x25  // 陀螺仪X轴数据高字节
#define REG_GYRO_DATA_X0          0x26  // 陀螺仪X轴数据低字节
#define REG_GYRO_DATA_Y1          0x27  // 陀螺仪Y轴数据高字节
#define REG_GYRO_DATA_Y0          0x28  // 陀螺仪Y轴数据低字节
#define REG_GYRO_DATA_Z1          0x29  // 陀螺仪Z轴数据高字节
#define REG_GYRO_DATA_Z0          0x2A  // 陀螺仪Z轴数据低字节
#define REG_TMST_FSYNCH           0x2B  // FSYNC时间戳高字节
#define REG_TMST_FSYNCL           0x2C  // FSYNC时间戳低字节
#define REG_INT_STATUS            0x2D  // 中断状态标志
#define REG_FIFO_COUNTH           0x2E  // FIFO计数器高字节
#define REG_FIFO_COUNTL           0x2F  // FIFO计数器低字节
#define REG_FIFO_DATA             0x30  // FIFO数据端口
#define REG_APEX_DATA0            0x31  // APEX数据0（步数低字节）
#define REG_APEX_DATA1            0x32  // APEX数据1（步数高字节）
#define REG_APEX_DATA2            0x33  // APEX数据2（步频）
#define REG_APEX_DATA3            0x34  // APEX数据3（活动分类、DMP状态）
#define REG_APEX_DATA4            0x35  // APEX数据4（点击检测信息）
#define REG_APEX_DATA5            0x36  // APEX数据5（双击时间间隔）
#define REG_INT_STATUS2           0x37  // 中断状态2（SMD、WoM）
#define REG_INT_STATUS3           0x38  // 中断状态3（步数、倾斜、唤醒等）
#define REG_SIGNAL_PATH_RESET     0x4B  // 信号路径复位（DMP、FIFO、时间戳）
#define REG_INTF_CONFIG0          0x4C  // 接口配置0（数据格式、端序）
#define REG_INTF_CONFIG1          0x4D  // 接口配置1（时钟选择、RTC模式）
#define REG_PWR_MGMT0             0x4E  // 电源管理0（传感器使能、模式）
#define REG_GYRO_CONFIG0          0x4F  // 陀螺仪配置0（量程、ODR）
#define REG_ACCEL_CONFIG0         0x50  // 加速度计配置0（量程、ODR）
#define REG_GYRO_CONFIG1          0x51  // 陀螺仪配置1（滤波器配置）
#define REG_GYRO_ACCEL_CONFIG0    0x52  // 陀螺仪/加速度计UI滤波器带宽
#define REG_ACCEL_CONFIG1         0x53  // 加速度计配置1（滤波器阶数）
#define REG_TMST_CONFIG           0x54  // 时间戳配置
#define REG_APEX_CONFIG0          0x56  // APEX配置0（功能使能、DMP ODR）
#define REG_SMD_CONFIG            0x57  // 显著运动检测配置
#define REG_FIFO_CONFIG1          0x5F  // FIFO配置1（使能、高分辨率等）
#define REG_FIFO_CONFIG2          0x60  // FIFO配置2（水位阈值低字节）
#define REG_FIFO_CONFIG3          0x61  // FIFO配置3（水位阈值高字节）
#define REG_FSYNC_CONFIG          0x62  // FSYNC配置（标签、极性）
#define REG_INT_CONFIG0           0x63  // 中断清除方式配置
#define REG_INT_CONFIG1           0x64  // 中断脉冲和复位配置
#define REG_INT_SOURCE0           0x65  // 中断源使能到INT1（基础中断）
#define REG_INT_SOURCE1           0x66  // 中断源使能到INT1（I3C错误、SMD、WoM）
#define REG_INT_SOURCE3           0x68  // 中断源使能到INT2（基础中断）
#define REG_INT_SOURCE4           0x69  // 中断源使能到INT2（I3C错误、SMD、WoM）
#define REG_FIFO_LOST_PKT0        0x6C  // FIFO丢失包计数低字节
#define REG_FIFO_LOST_PKT1        0x6D  // FIFO丢失包计数高字节
#define REG_SELF_TEST_CONFIG      0x70  // 自测配置（陀螺仪、加速度计）
#define REG_WHO_AM_I              0x75  // 设备ID寄存器（固定为0x47）
#define REG_REG_BANK_SEL          0x76  // 寄存器银行选择（跨银行访问）

// *******************************
// (User Bank 1) 寄存器
// *******************************
#define REG_SENSOR_CONFIG0        0x03  // 传感器轴使能/禁用配置
#define REG_GYRO_CONFIG_STATIC2   0x0B  // 陀螺仪抗混叠滤波器/陷波滤波器使能
#define REG_GYRO_CONFIG_STATIC3   0x0C  // 陀螺仪抗混叠滤波器参数（DELT）
#define REG_GYRO_CONFIG_STATIC4   0x0D  // 陀螺仪抗混叠滤波器参数（DELTSQR低字节）
#define REG_GYRO_CONFIG_STATIC5   0x0E  // 陀螺仪抗混叠滤波器参数（BITSHIFT、DELTSQR高字节）
#define REG_GYRO_CONFIG_STATIC6   0x0F  // 陀螺仪X轴陷波滤波器频率（低字节，出厂校准）
#define REG_GYRO_CONFIG_STATIC7   0x10  // 陀螺仪Y轴陷波滤波器频率（低字节，出厂校准）
#define REG_GYRO_CONFIG_STATIC8   0x11  // 陀螺仪Z轴陷波滤波器频率（低字节，出厂校准）
#define REG_GYRO_CONFIG_STATIC9   0x12  // 陀螺仪各轴陷波滤波器频率选择位（高位）
#define REG_GYRO_CONFIG_STATIC10  0x13  // 陀螺仪陷波滤波器带宽选择
#define REG_XG_ST_DATA            0x5F  // 陀螺仪X轴自测数据（出厂校准值）
#define REG_YG_ST_DATA            0x60  // 陀螺仪Y轴自测数据（出厂校准值）
#define REG_ZG_ST_DATA            0x61  // 陀螺仪Z轴自测数据（出厂校准值）
#define REG_TMSTVAL0              0x62  // 时间戳值低字节（需触发锁存）
#define REG_TMSTVAL1              0x63  // 时间戳值中字节
#define REG_TMSTVAL2              0x64  // 时间戳值高字节（4位）
#define REG_INTF_CONFIG4          0x7A  // 接口配置4（I3C总线模式、SPI线数）
#define REG_INTF_CONFIG5          0x7B  // 接口配置5（引脚9功能选择）
#define REG_INTF_CONFIG6          0x7C  // 接口配置6（I3C功能使能）

// *******************************
// (User Bank 2) 寄存器
// *******************************
#define REG_ACCEL_CONFIG_STATIC2  0x03  // 加速度计抗混叠滤波器参数（DELT）和使能
#define REG_ACCEL_CONFIG_STATIC3  0x04  // 加速度计抗混叠滤波器参数（DELTSQR低字节）
#define REG_ACCEL_CONFIG_STATIC4  0x05  // 加速度计抗混叠滤波器参数（BITSHIFT、DELTSQR高字节）
#define REG_XA_ST_DATA            0x3B  // 加速度计X轴自测数据（出厂校准值）
#define REG_YA_ST_DATA            0x3C  // 加速度计Y轴自测数据（出厂校准值）
#define REG_ZA_ST_DATA            0x3D  // 加速度计Z轴自测数据（出厂校准值）

// *******************************
// (User Bank 4) 寄存器
// *******************************
#define REG_APEX_CONFIG1          0x40  // APEX配置1（计步器低能量阈值、DMP休眠时间）
#define REG_APEX_CONFIG2          0x41  // APEX配置2（计步器幅度阈值、步数窗口）
#define REG_APEX_CONFIG3          0x42  // APEX配置3（计步器检测阈值、缓冲区时间、高能量阈值）
#define REG_APEX_CONFIG4          0x43  // APEX配置4（倾斜等待时间、休眠超时）
#define REG_APEX_CONFIG5          0x44  // APEX配置5（安装矩阵定义）
#define REG_APEX_CONFIG6          0x45  // APEX配置6（休眠手势检测窗口）
#define REG_APEX_CONFIG7          0x46  // APEX配置7（点击检测最小冲击阈值、最大峰值容差）
#define REG_APEX_CONFIG8          0x47  // APEX配置8（点击检测时间窗口参数）
#define REG_APEX_CONFIG9          0x48  // APEX配置9（灵敏度模式选择）
#define REG_ACCEL_WOM_X_THR       0x4A  // X轴WoM阈值（单位：mg，分辨率3.9mg）
#define REG_ACCEL_WOM_Y_THR       0x4B  // Y轴WoM阈值
#define REG_ACCEL_WOM_Z_THR       0x4C  // Z轴WoM阈值
#define REG_INT_SOURCE6           0x4D  // APEX中断使能到INT1（步数、倾斜、唤醒、休眠、点击）
#define REG_INT_SOURCE7           0x4E  // APEX中断使能到INT2（步数、倾斜、唤醒、休眠、点击）
#define REG_INT_SOURCE8           0x4F  // 基础中断使能到I3C IBI
#define REG_INT_SOURCE9           0x50  // I3C错误、SMD、WoM中断使能到IBI
#define REG_INT_SOURCE10          0x51  // APEX中断使能到IBI（步数、倾斜、唤醒、休眠、点击）
#define REG_OFFSET_USER0          0x77  // 用户陀螺仪X轴偏移低字节
#define REG_OFFSET_USER1          0x78  // 用户陀螺仪Y轴偏移高4位 + X轴偏移高4位
#define REG_OFFSET_USER2          0x79  // 用户陀螺仪Y轴偏移低字节
#define REG_OFFSET_USER3          0x7A  // 用户陀螺仪Z轴偏移低字节
#define REG_OFFSET_USER4          0x7B  // 用户加速度计X轴偏移高4位 + 陀螺仪Z轴偏移高4位
#define REG_OFFSET_USER5          0x7C  // 用户加速度计X轴偏移低字节
#define REG_OFFSET_USER6          0x7D  // 用户加速度计Y轴偏移低字节
#define REG_OFFSET_USER7          0x7E  // 用户加速度计Z轴偏移高4位 + Y轴偏移高4位
#define REG_OFFSET_USER8          0x7F  // 用户加速度计Z轴偏移低字节

#endif
