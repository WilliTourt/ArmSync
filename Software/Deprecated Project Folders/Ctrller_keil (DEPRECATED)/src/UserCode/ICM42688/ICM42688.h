#ifndef __ICM42688_H__
#define __ICM42688_H__

#include "src\UserCode\cpp_main\cpp_main.h"
#include "src\UserCode\ICM42688\ICM42688_reg.h"

#include "hal_data.h"

#include <stdint.h>

// #define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#include "debug.h"
#endif
#ifdef HAL_I2C_MODULE_ENABLED
#include "i2c.h"
#endif
#ifdef HAL_SPI_MODULE_ENABLED
#include "spi.h"
#endif

#ifdef R_SPI_API_H

#endif

#define PI 3.14159265
#define g  9.80655

class ICM42688
{
    public:
        enum class ICM42688_StatusTypeDef : uint8_t
        {
            OK = 0,
            ERROR,
        };

        enum class ICM42688_SWITCH : uint8_t
        {
            CLOSE = 0,
            OPEN = 1,
        };
        
        enum class ICM42688_MountingOrientation : uint8_t
        {
            MOUNT_Z_DOWN = 0,      // Z轴朝下（默认）
            MOUNT_Z_UP= 1,        // Z轴朝上
            MOUNT_X_DOWN= 2,      // X轴朝下
            MOUNT_X_UP= 3,        // X轴朝上
            MOUNT_Y_DOWN= 4,      // Y轴朝下
            MOUNT_Y_UP= 5,         // Y轴朝上
        };

        #ifdef HAL_I2C_MODULE_ENABLED
        ICM42688(I2C_HandleTypeDef* hi2c,
             uint8_t address = ICM42688_I2C_ADDR_LOW,
             ICM42688_MountingOrientation mounting = ICM42688_MountingOrientation::MOUNT_Z_DOWN);
        #endif

        #ifdef HAL_SPI_MODULE_ENABLED
        ICM42688(SPI_HandleTypeDef* hspi,
             GPIO_TypeDef* cs_port,
             uint16_t cs_pin,
             ICM42688_MountingOrientation mounting = ICM42688_MountingOrientation::MOUNT_Z_DOWN);
        #endif

        #ifdef R_SPI_API_H
        ICM42688(ICM42688_MountingOrientation mounting = ICM42688_MountingOrientation::MOUNT_Z_DOWN);
        #endif

        ICM42688_StatusTypeDef begin(void);

        #ifdef HAL_I2C_MODULE_ENABLED
        inline const char* get_GPIO_PortName(GPIO_TypeDef* GPIOx)
        {
            if (GPIOx == GPIOA) return "PA";
            else if (GPIOx == GPIOB) return "PB";
            else if (GPIOx == GPIOC) return "PC";
            else if (GPIOx == GPIOD) return "PD";
            else if (GPIOx == GPIOE) return "PE";
            else if (GPIOx == GPIOF) return "PF";
            else if (GPIOx == GPIOG) return "PG";
            else return "Unknown GPIO Port";
        }
        inline const char* get_GPIO_PinName(uint16_t GPIO_Pin)
        {
            switch(GPIO_Pin)
            {
                case GPIO_PIN_0:   return "0";
                case GPIO_PIN_1:   return "1";
                case GPIO_PIN_2:   return "2";
                case GPIO_PIN_3:   return "3";
                case GPIO_PIN_4:   return "4";
                case GPIO_PIN_5:   return "5";
                case GPIO_PIN_6:   return "6";
                case GPIO_PIN_7:   return "7";
                case GPIO_PIN_8:   return "8";
                case GPIO_PIN_9:   return "9";
                case GPIO_PIN_10:  return "10";
                case GPIO_PIN_11:  return "11";
                case GPIO_PIN_12:  return "12";
                case GPIO_PIN_13:  return "13";
                case GPIO_PIN_14:  return "14";
                case GPIO_PIN_15:  return "15";
                default:           return "Unknown GPIO Pin";
            }
        }
        inline const uint16_t get_GPIO_PinNum(uint16_t GPIO_Pin)
        {
            switch(GPIO_Pin)
            {
                case GPIO_PIN_0:   return 0;
                case GPIO_PIN_1:   return 1;
                case GPIO_PIN_2:   return 2;
                case GPIO_PIN_3:   return 3;
                case GPIO_PIN_4:   return 4;
                case GPIO_PIN_5:   return 5;
                case GPIO_PIN_6:   return 6;
                case GPIO_PIN_7:   return 7;
                case GPIO_PIN_8:   return 8;
                case GPIO_PIN_9:   return 9;
                case GPIO_PIN_10:  return 10;
                case GPIO_PIN_11:  return 11;
                case GPIO_PIN_12:  return 12;
                case GPIO_PIN_13:  return 13;
                case GPIO_PIN_14:  return 14;
                case GPIO_PIN_15:  return 15;
                default:           return 16;
            }
        }
        #endif

        class General
        {
            public:
                enum class StatusTypeDef : uint8_t
                {
                    OK = 0,
                    ERROR,
                };

                enum class ACCEL_ODR : uint8_t
                {
                    ACCEL_ODR_32000HZ = 1,      // LN mode
                    ACCEL_ODR_16000HZ = 2,      // LN mode
                    ACCEL_ODR_8000HZ = 3,       // LN mode
                    ACCEL_ODR_4000HZ = 4,       // LN mode
                    ACCEL_ODR_2000HZ = 5,       // LN mode
                    ACCEL_ODR_1000HZ = 6,       // LN mode
                    ACCEL_ODR_200HZ = 7,        // LP & LN mode
                    ACCEL_ODR_100HZ = 8,        // LP & LN mode
                    ACCEL_ODR_50HZ = 9,         // LP & LN mode
                    ACCEL_ODR_25HZ = 10,        // LP & LN mode
                    ACCEL_ODR_12_5HZ = 11,      // LP & LN mode
                    ACCEL_ODR_6_25HZ = 12,      // LP mode
                    ACCEL_ODR_3_125HZ = 13,     // LP mode
                    ACCEL_ODR_1_5625HZ = 14,    // LP mode
                    ACCEL_ODR_500HZ = 15,       // LP & LN mode
                };

                enum class ACCEL_FS : uint8_t
                {
                    ACCEL_FS_16G = 0,
                    ACCEL_FS_8G = 1,
                    ACCEL_FS_4G = 2,
                    ACCEL_FS_2G = 3,
                };

                enum class ACCEL_ORDER_UIF : uint8_t
                {
                    ACCEL_ORDER_UIF_1 = 0,
                    ACCEL_ORDER_UIF_2 = 1,
                    ACCEL_ORDER_UIF_3 = 2,
                };

                enum class ACCEL_MODE : uint8_t
                {
                    ACCEL_MODE_LP = 0,
                    ACCEL_MODE_LN = 1,
                };

                enum class ACCEL_UIF_LN : uint8_t
                {
                    ACCEL_UIF_LN_ODR_2 = 0,
                    ACCEL_UIF_LN_400InODR_4 = 1,
                    ACCEL_UIF_LN_400InODR_5 = 2,
                    ACCEL_UIF_LN_400InODR_8 = 3,
                    ACCEL_UIF_LN_400InODR_10 = 4,
                    ACCEL_UIF_LN_400InODR_16 = 5,
                    ACCEL_UIF_LN_400InODR_20 = 6,
                    ACCEL_UIF_LN_400InODR_40 = 7,
                    ACCEL_UIF_LN_400InODR = 14,
                    ACCEL_UIF_LN_200In8ODR = 15,
                };

                enum class ACCEL_UIF_LP : uint8_t
                {
                    ICM42688_ACCEL_UIF_LP_1AVG = 1,
                    ICM42688_ACCEL_UIF_LP_16AVG = 6,
                };

                enum class GYRO_ODR : uint8_t
                {
                    GYRO_ODR_32000HZ = 1,       // LN mode
                    GYRO_ODR_16000HZ = 2,       // LN mode
                    GYRO_ODR_8000HZ = 3,        // LN mode
                    GYRO_ODR_4000HZ = 4,        // LN mode
                    GYRO_ODR_2000HZ = 5,        // LN mode
                    GYRO_ODR_1000HZ = 6,        // LN mode
                    GYRO_ODR_200HZ = 7,         // LN mode
                    GYRO_ODR_100HZ = 8,         // LN mode
                    GYRO_ODR_50HZ = 9,          // LN mode
                    GYRO_ODR_25HZ = 10,         // LN mode
                    GYRO_ODR_12_5HZ = 11,       // LN mode
                    GYRO_ODR_500HZ = 15,        // LN mode
                };

                enum class GYRO_FS : uint8_t
                {
                    GYRO_FS_2000DPS = 0,
                    GYRO_FS_1000DPS = 1,
                    GYRO_FS_500DPS = 2,
                    GYRO_FS_250DPS = 3,
                    GYRO_FS_125DPS = 4,
                    GYRO_FS_62_5DPS = 5,
                    GYRO_FS_31_25DPS = 6,
                    GYRO_FS_15_625DPS = 7,
                };

                enum class GYRO_ORDER_UIF : uint8_t
                {
                    GYRO_ORDER_UIF_1 = 0,
                    GYRO_ORDER_UIF_2 = 1,
                    GYRO_ORDER_UIF_3 = 2,
                };

                enum class NF_BW : uint8_t
                {
                    NF_BW_1449HZ = 0,
                    NF_BW_680HZ = 1,
                    NF_BW_329HZ = 2,
                    NF_BW_162HZ = 3,
                    NF_BW_80HZ = 4,
                    NF_BW_40HZ = 5,
                    NF_BW_20HZ = 6,
                    NF_BW_10HZ = 7,
                };

                enum class GYRO_UIF_LN : uint8_t
                {
                    GYRO_UIF_LN_ODR_2 = 0,
                    GYRO_UIF_LN_400InODR_4 = 1,
                    GYRO_UIF_LN_400InODR_5 = 2,
                    GYRO_UIF_LN_400InODR_8 = 3,
                    GYRO_UIF_LN_400InODR_10 = 4,
                    GYRO_UIF_LN_400InODR_16 = 5,
                    GYRO_UIF_LN_400InODR_20 = 6,
                    GYRO_UIF_LN_400InODR_40 = 7,
                    GYRO_UIF_LN_400InODR = 14,
                    GYRO_UIF_LN_200In8ODR = 15,
                };

                enum class TEMP_DLPF_BW : uint8_t
                {
                    TEMP_DLPF_BW_4000Hz = 0,
                    TEMP_DLPF_BW_170Hz = 1,
                    TEMP_DLPF_BW_82Hz = 2,
                    TEMP_DLPF_BW_40Hz = 3,
                    TEMP_DLPF_BW_20Hz = 4,
                    TEMP_DLPF_BW_10Hz = 5,
                    TEMP_DLPF_BW_5Hz = 6,
                };

                enum class FILTER_LEVEL : uint8_t
                {
                    VERY_HIGH = 0,
                    HIGH = 1,
                    MEDIUM = 2,
                    LOW = 3,
                    VERY_LOW = 4,
                    VERT_VERY_LOW = 5,
                };

                enum class TMST_RESOL : uint8_t
                {
                    TMST_RESOL_1us = 0,
                    TMST_RESOL_16us = 1,
                };
               

                General(ICM42688* parent);

                ICM42688::ICM42688_StatusTypeDef begin(void);

                /**
                 * @brief 开启加速度计
                 * @param odr 加速度计ODR配置
                 * @param fs 加速度计量程配置
                 * @param mode 加速度计模式,分为LN和LP,根据需要以及手册限制写入
                 * @param UIF_bw_LN LN模式下的BW,如果是LP模式,则该参数无效
                 * @param UIF_bw_LP LP模式下的BW,如果是LN模式,则该参数无效
                 * @param UIF_order UI滤波器的阶数,阶数越高滤波约好,延迟更高
                 * @param M2F_enable M2滤波器开启,再在UI滤波器基础上再加一层滤波
                 * @param AAF_enable 抗混叠滤波器开启
                 * @param AAF_DELT AFF相关参数
                 * @param AAF_DELTSQR AFF相关参数
                 * @param AAF_BITSHIFT AFF相关参数
                 * @return General类状态
                 */
                StatusTypeDef enable_acc(ACCEL_ODR odr = ACCEL_ODR::ACCEL_ODR_200HZ,
                                        ACCEL_FS fs = ACCEL_FS::ACCEL_FS_2G,
                                        ACCEL_MODE mode = ACCEL_MODE::ACCEL_MODE_LN,
                                        ACCEL_UIF_LN UIF_bw_LN = ACCEL_UIF_LN::ACCEL_UIF_LN_400InODR_4,
                                        ACCEL_UIF_LP UIF_bw_LP = ACCEL_UIF_LP::ICM42688_ACCEL_UIF_LP_1AVG,
                                        ACCEL_ORDER_UIF UIF_order = ACCEL_ORDER_UIF::ACCEL_ORDER_UIF_1,
                                        ICM42688_SWITCH M2F_enable = ICM42688_SWITCH::CLOSE,
                                        ICM42688_SWITCH AAF_enable = ICM42688_SWITCH::CLOSE,
                                        uint8_t AAF_DELT = 2,
                                        uint16_t AAF_DELTSQR = 4,
                                        uint8_t AAF_BITSHIFT = 13);
                /**
                 * @brief 开启加速度计
                 * @param odr 加速度计ODR配置
                 * @param fs 加速度计量程配置
                 * @param mode 加速度计模式,分为LN和LP,根据需要以及手册限制写入
                 * @param UIF_bw_LN LN模式下的BW,如果是LP模式,则该参数无效
                 * @param UIF_bw_LP LP模式下的BW,如果是LN模式,则该参数无效
                 * @param UIF_order UI滤波器的阶数,阶数越高滤波约好,延迟更高
                 * @param M2F_enable M2滤波器开启,再在UI滤波器基础上再加一层滤波
                 * @param AAF_enable 抗混叠滤波器开启
                 * @return General类状态
                 */
                StatusTypeDef enable_acc(ACCEL_ODR odr = ACCEL_ODR::ACCEL_ODR_200HZ,
                                        ACCEL_FS fs = ACCEL_FS::ACCEL_FS_2G,
                                        ACCEL_MODE mode = ACCEL_MODE::ACCEL_MODE_LN,
                                        ACCEL_UIF_LN UIF_bw_LN = ACCEL_UIF_LN::ACCEL_UIF_LN_400InODR_4,
                                        ACCEL_UIF_LP UIF_bw_LP = ACCEL_UIF_LP::ICM42688_ACCEL_UIF_LP_1AVG,
                                        ACCEL_ORDER_UIF UIF_order = ACCEL_ORDER_UIF::ACCEL_ORDER_UIF_1,
                                        ICM42688_SWITCH M2F_enable = ICM42688_SWITCH::CLOSE,
                                        ICM42688_SWITCH AAF_enable = ICM42688_SWITCH::CLOSE);
                /**
                 * @brief 开启加速度计
                 * @param odr 加速度计ODR配置
                 * @param fs 加速度计量程配置
                 * @param filter_level 滤波等级
                 * @return General类状态
                 */
                StatusTypeDef enable_acc(ACCEL_ODR odr = ACCEL_ODR::ACCEL_ODR_200HZ,
                                        ACCEL_FS fs = ACCEL_FS::ACCEL_FS_2G,
                                        FILTER_LEVEL filter_level = FILTER_LEVEL::MEDIUM);
                /**
                 * @brief 关闭加速度计
                 * @return General类状态
                 */
                StatusTypeDef disable_acc(void);
                /**
                 * @brief 设置陀螺仪相关滤波器
                 * @param odr 加速度计ODR配置
                 * @param fs 加速度计量程配置
                 * @param UIF_bw UI滤波器的BW
                 * @param UIF_order UI滤波器的阶数,阶数越高滤波约好,延迟更高
                 * @param M2F_enable M2滤波器开启,再在UI滤波器基础上再加一层滤波
                 * @param NF_enable 缺陷滤波器开启,该滤波器可以滤除指定中心频率以及带宽的信号,如可以滤除噪声
                 * @param NF_certenfreq_x x轴缺陷滤波器中心频率
                 * @param NF_certenfreq_y y轴缺陷滤波器中心频率
                 * @param NF_certenfreq_z z轴缺陷滤波器中心频率
                 * @param NF_bw 缺陷滤波器带宽
                 * @param AAF_enable 抗混叠滤波器开启
                 * @param AAF_DELT AFF相关参数
                 * @param AAF_DELTSQR AFF相关参数
                 * @param AAF_BITSHIFT AFF相关参数
                 * @return General类状态
                 */    
                StatusTypeDef enable_gyro(GYRO_ODR odr = GYRO_ODR::GYRO_ODR_200HZ,
                                         GYRO_FS fs = GYRO_FS::GYRO_FS_500DPS,
                                         GYRO_UIF_LN UIF_bw = GYRO_UIF_LN::GYRO_UIF_LN_400InODR_4,
                                         GYRO_ORDER_UIF UIF_order = GYRO_ORDER_UIF::GYRO_ORDER_UIF_1,
                                         ICM42688_SWITCH M2F_enable = ICM42688_SWITCH::CLOSE,
                                         ICM42688_SWITCH NF_enable = ICM42688_SWITCH::CLOSE,
                                         uint16_t NF_certenfreq_x = 2000,
                                         uint16_t NF_certenfreq_y = 2000,
                                         uint16_t NF_certenfreq_z = 2000,
                                         NF_BW NF_bw = NF_BW::NF_BW_80HZ,
                                         ICM42688_SWITCH AAF_enable = ICM42688_SWITCH::CLOSE,
                                         uint8_t AAF_DELT = 2,
                                         uint16_t AAF_DELTSQR = 4,
                                         uint8_t AAF_BITSHIFT = 13);
                /**
                 * @brief 设置陀螺仪相关滤波器
                 * @param odr 加速度计ODR配置
                 * @param fs 加速度计量程配置
                 * @param UIF_bw UI滤波器的BW
                 * @param UIF_order UI滤波器的阶数,阶数越高滤波约好,延迟更高
                 * @param M2F_enable M2滤波器开启,再在UI滤波器基础上再加一层滤波
                 * @param NF_enable 缺陷滤波器开启,该滤波器可以滤除指定中心频率以及带宽的信号,如可以滤除噪声
                 * @param AAF_enable 抗混叠滤波器开启
                 * @return General类状态
                 */  
                StatusTypeDef enable_gyro(GYRO_ODR odr = GYRO_ODR::GYRO_ODR_200HZ,
                                         GYRO_FS fs = GYRO_FS::GYRO_FS_500DPS,
                                         GYRO_UIF_LN UIF_bw = GYRO_UIF_LN::GYRO_UIF_LN_400InODR_4,
                                         GYRO_ORDER_UIF UIF_order = GYRO_ORDER_UIF::GYRO_ORDER_UIF_1,
                                         ICM42688_SWITCH M2F_enable = ICM42688_SWITCH::CLOSE,
                                         ICM42688_SWITCH NF_enable = ICM42688_SWITCH::CLOSE,
                                         ICM42688_SWITCH AAF_enable = ICM42688_SWITCH::CLOSE);
                /**
                 * @brief 设置陀螺仪相关滤波器
                 * @param odr 加速度计ODR配置
                 * @param fs 加速度计量程配置
                 * @param filter_level 滤波等级
                 * @return General类状态
                 */ 
                StatusTypeDef enable_gyro(GYRO_ODR odr = GYRO_ODR::GYRO_ODR_200HZ,
                                         GYRO_FS fs = GYRO_FS::GYRO_FS_500DPS,
                                         FILTER_LEVEL filter_level = FILTER_LEVEL::MEDIUM);
                /**
                 * @brief 关闭陀螺仪
                 * @return General类状态
                 */
                StatusTypeDef disable_gyro(void);
                /**
                 * @brief 开启温度计
                 * @param DLPF_bw 温度计滤波系数
                 * @return General类状态
                 */
                StatusTypeDef enable_temp(TEMP_DLPF_BW DLPF_bw);
                /**
                 * @brief 开启温度计
                 * @return General类状态
                 */
                StatusTypeDef enable_temp(void);
                /**
                 * @brief 关闭温度计
                 * @return General类状态
                 */
                StatusTypeDef disable_temp(void);
                /**
                 * @brief 开启时间戳
                 * @param resol 时间戳分辨率
                 * @return General类状态
                 */
                StatusTypeDef enable_tmst(TMST_RESOL resol = TMST_RESOL::TMST_RESOL_16us);
                /**
                 * @brief 开启时间戳
                 * @return General类状态
                 */
                StatusTypeDef enable_tmst(void);
                /**
                 * @brief 关闭时间戳
                 * @return General类状态
                 */
                StatusTypeDef disable_tmst(void);

                /**
                 * @brief 设置加速度和角速度的偏移值
                 * @param samples 采样次数
                 * @return General类状态
                 */
                StatusTypeDef set_acc_gyro_offset(uint16_t samples);

                /**
                 * @brief 读取原始的加速度,角速度,温度
                 * @return General类状态
                 * @note 该函数可以提前判断哪些轴开启了,所以运行较慢
                 */ 
                StatusTypeDef read_data(void);

                /**
                 * @brief 获取加速度x分量
                 * @return 加速度x分量
                 */
                float get_ax(void);
                /**
                 * @brief 获取加速度y分量
                 * @return 加速度y分量
                 */
                float get_ay(void);
                /**
                 * @brief 获取加速度z分量
                 * @return 加速度z分量
                 */
                float get_az(void);
                /**
                 * @brief 获取角速度x分量
                 * @return 角速度计x分量
                 */
                float get_gx(void);
                /**
                 * @brief 获取角速度y分量
                 * @return 角速度计y分量
                 */
                float get_gy(void);
                /**
                 * @brief 获取角速度z分量
                 * @return 角速度计z分量
                 */
                float get_gz(void);
                /**
                 * @brief 获取温度
                 * @return 温度
                 */
                float get_temp(void);

                float get_tmst_fsync(void);

                float get_tmst(void);
                
            private:

                #ifdef ENABLE_DEBUG
                    // ACCEL_ODR
                    const char* ACCEL_ODR_str[16] = {
                        "UNKNOW",           // 0
                        "32000HZ",          // 1
                        "16000HZ",          // 2
                        "8000HZ",           // 3
                        "4000HZ",           // 4
                        "2000HZ",           // 5
                        "1000HZ",           // 6
                        "200HZ",            // 7
                        "100HZ",            // 8
                        "50HZ",             // 9
                        "25HZ",             // 10
                        "12.5HZ",           // 11
                        "6.25HZ",           // 12
                        "3.125HZ",          // 13
                        "1.5625HZ",         // 14
                        "500HZ"             // 15
                    };

                    // ACCEL_FS
                    const char* ACCEL_FS_str[4] = {
                        "16G",
                        "8G",
                        "4G",
                        "2G",
                    };

                    // ACCEL_ORDER_UIF
                    const char* ACCEL_ORDER_UIF_str[3] = {
                        "ORDER_UIF_1",    // 0
                        "ORDER_UIF_2",    // 1
                        "ORDER_UIF_3",    // 2
                    };

                    // ACCEL_MODE
                    const char* ACCEL_MODE_str[2] = {
                        "MODE_LP",        // 0
                        "MODE_LN",        // 1
                    };

                    // ACCEL_UIF_LN (注意：枚举值不连续，需要填充空位)
                    const char* ACCEL_UIF_LN_str[16] = {
                        "UIF_LN_ODR_2",           // 0
                        "UIF_LN_400InODR_4",      // 1
                        "UIF_LN_400InODR_5",      // 2
                        "UIF_LN_400InODR_8",      // 3
                        "UIF_LN_400InODR_10",     // 4
                        "UIF_LN_400InODR_16",     // 5
                        "UIF_LN_400InODR_20",     // 6
                        "UIF_LN_400InODR_40",     // 7
                        "UNKNOWN",                // 8  (空位)
                        "UNKNOWN",                // 9  (空位)
                        "UNKNOWN",                // 10 (空位)
                        "UNKNOWN",                // 11 (空位)
                        "UNKNOWN",                // 12 (空位)
                        "UNKNOWN",                // 13 (空位)
                        "UIF_LN_400InODR",        // 14
                        "UIF_LN_200In8ODR",       // 15
                    };

                    // ACCEL_UIF_LP (注意：枚举值不连续，只有1和6)
                    const char* ACCEL_UIF_LP_str[7] = {
                        "UNKNOWN",                // 0
                        "UIF_LP_1AVG",            // 1
                        "UNKNOWN",                // 2
                        "UNKNOWN",                // 3
                        "UNKNOWN",                // 4
                        "UNKNOWN",                // 5
                        "UIF_LP_16AVG",           // 6
                    };

                    // GYRO_ODR (从1开始，需要偏移)
                    const char* GYRO_ODR_str[16] = {
                        "UNKNOWN",                // 0
                        "32000HZ",                // 1
                        "16000HZ",                // 2
                        "8000HZ",                 // 3
                        "4000HZ",                 // 4
                        "2000HZ",                 // 5
                        "1000HZ",                 // 6
                        "200HZ",                  // 7
                        "100HZ",                  // 8
                        "50HZ",                   // 9
                        "25HZ",                   // 10
                        "12.5HZ",                 // 11
                        "UNKNOWN",                // 12
                        "UNKNOWN",                // 13
                        "UNKNOWN",                // 14
                        "500HZ",                  // 15
                    };

                    // GYRO_FS
                    const char* GYRO_FS_str[8] = {
                        "2000DPS",                // 0
                        "1000DPS",                // 1
                        "500DPS",                 // 2
                        "250DPS",                 // 3
                        "125DPS",                 // 4
                        "62.5DPS",                // 5
                        "31.25DPS",               // 6
                        "15.625DPS",              // 7
                    };

                    // GYRO_ORDER_UIF
                    const char* GYRO_ORDER_UIF_str[3] = {
                        "ORDER_UIF_1",            // 0
                        "ORDER_UIF_2",            // 1
                        "ORDER_UIF_3",            // 2
                    };

                    // NF_BW
                    const char* NF_BW_str[8] = {
                        "NF_BW_1449HZ",           // 0
                        "NF_BW_680HZ",            // 1
                        "NF_BW_329HZ",            // 2
                        "NF_BW_162HZ",            // 3
                        "NF_BW_80HZ",             // 4
                        "NF_BW_40HZ",             // 5
                        "NF_BW_20HZ",             // 6
                        "NF_BW_10HZ",             // 7
                    };

                    // GYRO_UIF_LN (注意：枚举值不连续，需要填充空位)
                    const char* GYRO_UIF_LN_str[16] = {
                        "UIF_LN_ODR_2",           // 0
                        "UIF_LN_400InODR_4",      // 1
                        "UIF_LN_400InODR_5",      // 2
                        "UIF_LN_400InODR_8",      // 3
                        "UIF_LN_400InODR_10",     // 4
                        "UIF_LN_400InODR_16",     // 5
                        "UIF_LN_400InODR_20",     // 6
                        "UIF_LN_400InODR_40",     // 7
                        "UNKNOWN",                // 8  (空位)
                        "UNKNOWN",                // 9  (空位)
                        "UNKNOWN",                // 10 (空位)
                        "UNKNOWN",                // 11 (空位)
                        "UNKNOWN",                // 12 (空位)
                        "UNKNOWN",                // 13 (空位)
                        "UIF_LN_400InODR",        // 14
                        "UIF_LN_200In8ODR",       // 15
                    };

                    // TEMP_DLPF_BW
                    const char* TEMP_DLPF_BW_str[7] = {
                        "4000Hz",                 // 0
                        "170Hz",                  // 1
                        "82Hz",                   // 2
                        "40Hz",                   // 3
                        "20Hz",                   // 4
                        "10Hz",                   // 5
                        "5Hz",                    // 6
                    };

                    // FILTER_LEVEL
                    const char* FILTER_LEVEL_str[6] = {
                        "VERY_HIGH",              // 0
                        "HIGH",                   // 1
                        "MEDIUM",                 // 2
                        "LOW",                    // 3
                        "VERY_LOW",               // 4
                        "VERT_VERY_LOW",          // 5
                    };

                #endif

                const uint16_t ACCLE_FS_SENSITIVITY[4] = {
                    2047,       // 16G
                    4095,       // 8G
                    8191,       // 4G
                    16383       // 2G
                };

                const float GYRO_FS_SENSITIVITY[8] = {
                    16.4f,      // 2000dps
                    32.8f,      // 1000dps
                    65.5f,      // 500dps
                    131.0f,     // 250dps
                    262.0f,     // 125dps
                    524.0f,     // 62.5dps
                    1048.0f,    // 31.25dps
                    2096.0f     // 15.625dps
                };

                /**
                 * @brief General类下的读取寄存器
                 * @param reg 寄存器号
                 * @param pdata 存储读取值指针
                 * @param len 读取数量
                 * @return General类状态
                 */
                StatusTypeDef writeReg(uint8_t reg, uint8_t* pdata, uint16_t len);
                /**
                 * @brief General类下的写入寄存器
                 * @param reg 寄存器号
                 * @param pdata 存储读取值指针
                 * @param len 读取数量
                 * @return General类状态
                 */
                StatusTypeDef readReg(uint8_t reg, uint8_t* pdata, uint16_t len);

                /**
                 * @brief 自动求解AFF参数值(用于加速度计)
                 * @param odr 加速度计ODR
                 * @param AAF_DELT AFF相关参数
                 * @param AAF_DELTSQR AFF相关参数
                 * @param AAF_BITSHIFT AFF相关参数
                 * @return General类状态
                 * @note AFF自动求解是直接取的最接近ODR/2的值
                 */
                StatusTypeDef find_AFF(ACCEL_ODR odr, uint8_t* AAF_DELT, uint16_t* AAF_DELTSQR, uint8_t* AAF_BITSHIFT);
                /**
                 * @brief 自动求解AFF参数值(用于陀螺仪)
                 * @param odr 陀螺仪ODR
                 * @param AAF_DELT AFF相关参数
                 * @param AAF_DELTSQR AFF相关参数
                 * @param AAF_BITSHIFT AFF相关参数
                 * @return General类状态
                 * @note AFF自动求解是直接取的最接近ODR/2的值
                 */
                StatusTypeDef find_AFF(GYRO_ODR odr, uint8_t* AAF_DELT, uint16_t* AAF_DELTSQR, uint8_t* AAF_BITSHIFT);
                /**
                 * @brief 自动求解NF参数值
                 * @param fdesired 所需中心频率
                 * @param NF_COSWZ_SEL NF相关参数
                 * @param NF_COSWZ NF相关参数
                 * @return General类状态
                 */
                StatusTypeDef find_NF(uint16_t fdesired, uint8_t* NF_COSWZ_SEL, uint16_t* NF_COSWZ);

                /**
                 * @brief 设置加速度计相关滤波器
                 * @param mode 加速度计模式,分为LN和LP,根据需要以及手册限制写入
                 * @param UIF_bw_LN LN模式下的BW,如果是LP模式,则该参数无效
                 * @param UIF_bw_LP LP模式下的BW,如果是LN模式,则该参数无效
                 * @param UIF_order UI滤波器的阶数,阶数越高滤波约好,延迟更高
                 * @param M2F_enable M2滤波器开启,再在UI滤波器基础上再加一层滤波
                 * @param AAF_enable 抗混叠滤波器开启
                 * @param AAF_DELT AFF相关参数
                 * @param AAF_DELTSQR AFF相关参数
                 * @param AAF_BITSHIFT AFF相关参数
                 * @return General类状态
                 */
                StatusTypeDef set_acc_filter(ACCEL_MODE mode, ACCEL_UIF_LN UIF_bw_LN, ACCEL_UIF_LP UIF_bw_LP, ACCEL_ORDER_UIF UIF_order, ICM42688_SWITCH M2F_enable, ICM42688_SWITCH AAF_enable, uint8_t AAF_DELT, uint16_t AAF_DELTSQR, uint8_t AAF_BITSHIFT);
                /**
                 * @brief 设置陀螺仪相关滤波器
                 * @param UIF_bw UI滤波器的BW
                 * @param UIF_order UI滤波器的阶数,阶数越高滤波约好,延迟更高
                 * @param M2F_enable M2滤波器开启,再在UI滤波器基础上再加一层滤波
                 * @param NF_enable 缺陷滤波器开启,该滤波器可以滤除指定中心频率以及带宽的信号,如可以滤除噪声
                 * @param NF_certenfreq_x x轴缺陷滤波器中心频率
                 * @param NF_certenfreq_y y轴缺陷滤波器中心频率
                 * @param NF_certenfreq_z z轴缺陷滤波器中心频率
                 * @param NF_bw 缺陷滤波器带宽
                 * @param AAF_enable 抗混叠滤波器开启
                 * @param AAF_DELT AFF相关参数
                 * @param AAF_DELTSQR AFF相关参数
                 * @param AAF_BITSHIFT AFF相关参数
                 * @return General类状态
                 */         
                StatusTypeDef set_gyro_filter(GYRO_UIF_LN UIF_bw, GYRO_ORDER_UIF UIF_order, ICM42688_SWITCH M2F_enable,ICM42688_SWITCH NF_enable, uint16_t NF_certenfreq_x, uint16_t NF_certenfreq_y, uint16_t NF_certenfreq_z, NF_BW NF_bw, ICM42688_SWITCH AAF_enable, uint8_t AAF_DELT, uint16_t AAF_DELTSQR, uint8_t AAF_BITSHIFT);

                ICM42688* _parent;

                uint8_t _acc_fs;
                uint8_t _gyro_fs;
                uint8_t _tmst_res;

                int16_t _ax_raw;
                int16_t _ay_raw;
                int16_t _az_raw;
                int16_t _gx_raw;
                int16_t _gy_raw;
                int16_t _gz_raw;
                int16_t _temp_raw;
                uint16_t _tmst_fsync_raw;
                uint32_t _tmst_raw;

                float _acc_offset_x;
                float _acc_offset_y;
                float _acc_offset_z;
                float _gyro_offset_x;
                float _gyro_offset_y;
                float _gyro_offset_z;
                
                float _ax;
                float _ay;
                float _az;
                float _gx;
                float _gy;
                float _gz;
                float _temp;
                float _tmst_fsync;
                float _tmst;
        };
        General general;

        class INT
        {
            public:
                enum class StatusTypeDef : uint8_t
                {
                    OK = 0,
                    ERROR,
                };

                enum class CLEAR_MODE : uint8_t
                {
                    CLEAR_ON_STATUS_BIT_READ = 0,
                    CLEAR_ON_REG_READ = 1,
                    CLEAR_ON_STATUS_BIT_AND_REG_READ = 2,
                };

                enum class LEVEL : uint8_t
                {
                    LEVEL_ACTIVE_LOW = 0,
                    LEVEL_ACTIVE_HIGH = 1,
                };

                enum class DRIVE : uint8_t
                {
                    DRIVE_OPEN_DRAIN = 0,
                    DRIVE_PUSH_PULL = 1,
                };

                enum class MODE : uint8_t
                {
                    PULSE = 0,
                    LATCHED = 1,
                };

                enum class INT_NUM : uint8_t
                {
                    NULL_INT = 0,
                    INT1 = 1,
                    INT2 = 2,
                    BOTH = 3,
                };

                enum class IntEvent : uint8_t 
                {
                    UI_FSYNC_INT,       // 0
                    PLL_RDY_INT,        // 1
                    RESET_DONE_INT,     // 2
                    DATA_RDY_INT,       // 3
                    FIFO_THS_INT,       // 4
                    FIFO_FULL_INT,      // 5
                    UI_AGC_RDY_INT,     // 6
                    I3C_ERROR_INT,      // 7
                    SMD_INT,            // 8
                    WOM_Z_INT,          // 9
                    WOM_Y_INT,          // 10
                    WOM_X_INT,          // 11
                    STEP_DET_INT,       // 12
                    STEP_CNT_OF_INT,    // 13
                    TILT_DET_INT,       // 14
                    WAKE_DET_INT,       // 15
                    SLEEP_DET_INT,      // 16
                    TAP_DET_INT,        // 17 
                    NUM_EVENTS          // 18 - 用于数组大小
                };

                enum class EVENT_PRIORITY : uint8_t {
                    PRIORITY_HIGHEST = 0,
                    PRIORITY_HIGH = 1,
                    PRIORITY_NORMAL = 2,
                    PRIORITY_LOW = 3,
                    PRIORITY_LOWEST = 4
                };

                typedef void (*IntCallback)(void* context);

                INT(ICM42688* parent);

                ICM42688::ICM42688_StatusTypeDef begin(void);

                StatusTypeDef set_int_latchClear_mode(CLEAR_MODE DRDY_clear_mode = CLEAR_MODE::CLEAR_ON_STATUS_BIT_READ,
                                                     CLEAR_MODE FIFO_THS_clear_mode = CLEAR_MODE::CLEAR_ON_STATUS_BIT_READ,
                                                     CLEAR_MODE FIFO_FULL_clear_mode = CLEAR_MODE::CLEAR_ON_STATUS_BIT_READ);
                StatusTypeDef set_int1_pin_cfg(LEVEL level = LEVEL::LEVEL_ACTIVE_HIGH,
                                              DRIVE drive = DRIVE::DRIVE_PUSH_PULL,
                                              MODE mode = MODE::LATCHED);
                // StatusTypeDef set_int2_pin_cfg(GPIO_TypeDef* int2_port,
                //                               uint16_t int2_pin,
                //                               LEVEL level = LEVEL::LEVEL_ACTIVE_HIGH,
                //                               DRIVE drive = DRIVE::DRIVE_PUSH_PULL,
                //                               MODE mode = MODE::LATCHED);

                StatusTypeDef register_event(IntEvent event, INT_NUM int_num, IntCallback callback, void* context = nullptr, EVENT_PRIORITY priority = EVENT_PRIORITY::PRIORITY_NORMAL);
                StatusTypeDef unregister_event(IntEvent event, INT_NUM int_num);
                StatusTypeDef set_event_priority(IntEvent event, INT_NUM int_num, EVENT_PRIORITY priority);
                StatusTypeDef set_event_enable(IntEvent event, INT_NUM int_num, bool enable);

                StatusTypeDef process_interrupts(INT_NUM int_num); 

                bool is_event_registered(IntEvent event, INT_NUM int_num) const;
                
                #ifdef ENABLE_DEBUG
                    void print_int_source(void);
                #endif

                // GPIO_TypeDef* _int1_port;
                // uint16_t _int1_pin;
                // GPIO_TypeDef* _int2_port;
                // uint16_t _int2_pin;

            private:

                #ifdef ENABLE_DEBUG
                    // CLEAR_MODE 字符串数组定义
                    const char* CLEAR_MODE_str[3] = {
                        "CLEAR_ON_STATUS_BIT_READ",          // 0
                        "CLEAR_ON_REG_READ",                 // 1
                        "CLEAR_ON_STATUS_BIT_AND_REG_READ"   // 2
                    };

                    // LEVEL 字符串数组定义
                    const char* LEVEL_str[2] = {
                        "LEVEL_ACTIVE_LOW",  // 0
                        "LEVEL_ACTIVE_HIGH"  // 1
                    };

                    // DRIVE 字符串数组定义
                    const char* DRIVE_str[2] = {
                        "DRIVE_OPEN_DRAIN",  // 0
                        "DRIVE_PUSH_PULL"    // 1
                    };

                    // MODE 字符串数组定义
                    const char* MODE_str[2] = {
                        "PULSE",    // 0
                        "LATCHED"   // 1
                    };
                #endif

                // 事件信息结构
                struct EventInfo 
                {
                    IntCallback callback_INT1;           // 回调函数
                    void* context_INT1;                  // 上下文数据
                    EVENT_PRIORITY priority_INT1;        // 事件优先级
                    bool enabled_INT1;                   // 是否启用
                    bool registered_INT1;                // 是否已注册

                    IntCallback callback_INT2;           // 回调函数
                    void* context_INT2;                  // 上下文数据
                    EVENT_PRIORITY priority_INT2;        // 事件优先级
                    bool enabled_INT2;                   // 是否启用
                    bool registered_INT2;                // 是否已注册
                    
                    // 构造函数
                    EventInfo() 
                        : callback_INT1(nullptr), 
                        context_INT1(nullptr), 
                        priority_INT1(EVENT_PRIORITY::PRIORITY_NORMAL),
                        enabled_INT1(false),
                        registered_INT1(false),
                        callback_INT2(nullptr), 
                        context_INT2(nullptr), 
                        priority_INT2(EVENT_PRIORITY::PRIORITY_NORMAL),
                        enabled_INT2(false),
                        registered_INT2(false) {}
                };
                
                // 事件信息数组（每个事件一个）
                EventInfo _events[static_cast<size_t>(IntEvent::NUM_EVENTS)];
                IntEvent _event;

                StatusTypeDef writeReg(uint8_t reg, uint8_t* pdata, uint16_t len);
                StatusTypeDef readReg(uint8_t reg, uint8_t* pdata, uint16_t len);   

                StatusTypeDef set_int_source(INT_NUM int_num, IntEvent event, bool enable);
                StatusTypeDef get_int_state(void);

                bool _UI_FSYNC_INT_state;
                bool _PLL_RDY_INT_state;
                bool _RESET_DONE_INT_state;
                bool _DATA_RDY_INT_state;
                bool _FIFO_THS_INT_state;
                bool _FIFO_FULL_INT_state;
                bool _UI_AGC_RDY_INT_state;
                bool _I3C_ERROR_INT_state;
                bool _SMD_INT_state;
                bool _WOM_Z_INT_state;
                bool _WOM_Y_INT_state;
                bool _WOM_X_INT_state;
                bool _STEP_DET_INT_state;
                bool _STEP_CNT_OF_INT_state;
                bool _TILT_DET_INT_state;
                bool _WAKE_DET_INT_state;
                bool _SLEEP_DET_INT_state;
                bool _TAP_DET_INT_state;
                
                ICM42688* _parent;
        };
        INT Int;

        class FIFO
        {
            public:
                enum class StatusTypeDef : uint8_t
                {
                    OK = 0,
                    ERROR,
                };

                enum class FIFO_MODE_1 : uint8_t
                {
                    FIFO_MODE_BYPASS = 0,
                    FIFO_MODE_STREAM = 1,
                    FIFO_MODE_TRIGGERED = 2,
                    FIFO_MODE_RESERVED = 3,
                };

                enum class FIFO_MODE_2 : uint8_t
                {
                    FIFO_MODE_PARTIAL_RD_CLOSE = 0,
                    FIFO_MODE_PARTIAL_RD_ENABLE = 1,
                };

                enum class FIFO_MODE_3 : uint8_t
                {
                    FIFO_MODE_WM_GT_TH_MORE = 0,
                    FIFO_MODE_WM_GT_TH_MORETHAN = 1,
                };

                FIFO(ICM42688* parent);

                StatusTypeDef begin(void);

                StatusTypeDef enable(FIFO_MODE_1 fifo_mode_1 = FIFO_MODE_1::FIFO_MODE_STREAM,
                                    FIFO_MODE_2 fifo_mode_2 = FIFO_MODE_2::FIFO_MODE_PARTIAL_RD_CLOSE,
                                    FIFO_MODE_3 fifo_mode_3 = FIFO_MODE_3::FIFO_MODE_WM_GT_TH_MORE,
                                    ICM42688_SWITCH acc_enable = ICM42688_SWITCH::CLOSE,
                                    ICM42688_SWITCH gyro_enable = ICM42688_SWITCH::CLOSE,
                                    ICM42688_SWITCH temp_enable = ICM42688_SWITCH::CLOSE,
                                    ICM42688_SWITCH timestamp_enable = ICM42688_SWITCH::CLOSE);
                StatusTypeDef disable(void);
                StatusTypeDef read(uint8_t* pdata, uint16_t len);
                StatusTypeDef get_count(uint16_t* count);

            private:
                StatusTypeDef writeReg(uint8_t reg, uint8_t* pdata, uint16_t len);
                StatusTypeDef readReg(uint8_t reg, uint8_t* pdata, uint16_t len);

                ICM42688* _parent;
        };
        FIFO fifo;

        class APEX
        {
            public:
                enum class StatusTypeDef : uint8_t
                {
                    OK = 0,
                    ERROR,
                };

                enum class PED_AMP_TH : uint8_t
                {
                    PED_AMP_TH_0 = 0,
                    PED_AMP_TH_1 = 1,
                    PED_AMP_TH_2 = 2,
                    PED_AMP_TH_3 = 3,
                    PED_AMP_TH_4 = 4,
                    PED_AMP_TH_5 = 5,
                    PED_AMP_TH_6 = 6,
                    PED_AMP_TH_7 = 7,
                    PED_AMP_TH_8 = 8,
                    PED_AMP_TH_9 = 9,
                    PED_AMP_TH_10 = 10,
                    PED_AMP_TH_11 = 11,
                    PED_AMP_TH_12 = 12,
                    PED_AMP_TH_13 = 13,
                    PED_AMP_TH_14 = 14,
                    PED_AMP_TH_15 = 15,
                };

                enum class PED_DET_TH : uint8_t
                {
                    PED_DET_TH_0 = 0,
                    PED_DET_TH_1 = 1,
                    PED_DET_TH_2 = 2,
                    PED_DET_TH_3 = 3,
                    PED_DET_TH_4 = 4,
                    PED_DET_TH_5 = 5,
                    PED_DET_TH_6 = 6,
                    PED_DET_TH_7 = 7,
                };

                enum class PED_BUF_TH : uint8_t
                {
                    PED_BUF_TH_0 = 0,
                    PED_BUF_TH_1 = 1,
                    PED_BUF_TH_2 = 2,
                    PED_BUF_TH_3 = 3,
                    PED_BUF_TH_4 = 4,
                    PED_BUF_TH_5 = 5,
                    PED_BUF_TH_6 = 6,
                    PED_BUF_TH_7 = 7,
                };

                enum class TILT_DELAY_TIME : uint8_t
                {
                    TILT_DELAY_TIME_0S = 0,
                    TILT_DELAY_TIME_2S = 1,
                    TILT_DELAY_TIME_4S = 2,
                    TILT_DELAY_TIME_6S = 3,
                };

                enum class R2W_SLEEP_TIMEOUT : uint8_t
                {
                    R2W_SLEEP_TIME_1_28S = 0,
                    R2W_SLEEP_TIME_2_56S = 1,
                    R2W_SLEEP_TIME_3_84S = 2,
                    R2W_SLEEP_TIME_5_12S = 3,
                    R2W_SLEEP_TIME_6_40S = 4,
                    R2W_SLEEP_TIME_7_68S = 5,
                    R2W_SLEEP_TIME_8_96S = 6,
                    R2W_SLEEP_TIME_10_24S = 7,
                };

                enum class R2W_GESTURE_DELAY : uint8_t
                {
                    R2W_GESTURE_DELAY_0_32S = 0,
                    R2W_GESTURE_DELAY_0_64S = 1,
                    R2W_GESTURE_DELAY_0_96S = 2,
                    R2W_GESTURE_DELAY_1_28S = 3,
                    R2W_GESTURE_DELAY_1_60S = 4,
                    R2W_GESTURE_DELAY_1_92S = 5,
                    R2W_GESTURE_DELAY_2_24S = 6,
                    R2W_GESTURE_DELAY_2_56S = 7,
                };

                enum class SMD_DETECT_TIME : uint8_t
                {
                    SMD_DETECT_TIME_1S = 0,
                    SMD_DETECT_TIME_3S = 1,
                };

                enum class PED_ACTIVITY_STATE : uint8_t
                {
                    PED_ACTIVITY_STATE_UNKNOEN = 0,
                    PED_ACTIVITY_STATE_WALKING = 1,
                    PED_ACTIVITY_STATE_RUNNING = 2,
                };

                APEX(void);

                StatusTypeDef begin(void);

                StatusTypeDef enable(ICM42688_SWITCH tap_enable = ICM42688_SWITCH::CLOSE,
                                    ICM42688_SWITCH pedometer_enable = ICM42688_SWITCH::CLOSE,
                                    ICM42688_SWITCH tilt_detection_enable = ICM42688_SWITCH::CLOSE,
                                    ICM42688_SWITCH R2W_enable = ICM42688_SWITCH::CLOSE,
                                    ICM42688_SWITCH WOM_x_enable = ICM42688_SWITCH::CLOSE,
                                    ICM42688_SWITCH WOM_y_enable = ICM42688_SWITCH::CLOSE,
                                    ICM42688_SWITCH WOM_z_enable = ICM42688_SWITCH::CLOSE,
                                    ICM42688_SWITCH SMD_enable = ICM42688_SWITCH::CLOSE);
                StatusTypeDef disable(void);

                StatusTypeDef set_tap_param(uint8_t min_jerk_th = 0x11, 
                                           uint8_t max_peak_tol = 0x01,
                                           uint8_t min_window_time = 0x11,
                                           uint8_t max_window_time = 0x01,
                                           uint8_t sensitivity = 0x01);
                StatusTypeDef set_pedometer_param(PED_AMP_TH amp_th = PED_AMP_TH::PED_AMP_TH_5,
                                                 PED_DET_TH detection_th = PED_DET_TH::PED_DET_TH_2,
                                                 PED_BUF_TH buffer_th = PED_BUF_TH::PED_BUF_TH_4);
                StatusTypeDef set_tilt_param(TILT_DELAY_TIME tilt_time_th = TILT_DELAY_TIME::TILT_DELAY_TIME_2S);
                StatusTypeDef set_R2W_param(R2W_SLEEP_TIMEOUT sleep_timeOut = R2W_SLEEP_TIMEOUT::R2W_SLEEP_TIME_5_12S, 
                                           R2W_GESTURE_DELAY R2W_angle_th = R2W_GESTURE_DELAY::R2W_GESTURE_DELAY_0_96S);
                StatusTypeDef set_WON_param(uint8_t wom_x_th = 0x98,
                                          uint8_t wom_y_th = 0x98,
                                          uint8_t wom_z_th = 0x98);
                StatusTypeDef set_SMD_param(uint8_t wom_x_th = 0x98,
                                          uint8_t wom_y_th = 0x98,
                                          uint8_t wom_z_th = 0x98, 
                                          SMD_DETECT_TIME smd_time = SMD_DETECT_TIME::SMD_DETECT_TIME_1S);
                 
                StatusTypeDef get_pedometer_data(uint16_t* pedometer_count, uint8_t* step_cadence, PED_ACTIVITY_STATE* walk_run_status);
                StatusTypeDef get_tap_data(uint8_t* tap_count, uint8_t* tap_direction, uint8_t* tap_dir, uint8_t* tap_double_timing);
        };

    private:

        bool _is_right_address = true;

        bool _is_i2c = false;
        bool _is_spi = false;

    protected:
        void icm_delay(uint32_t ms);
        ICM42688_StatusTypeDef writeReg(uint8_t reg, uint8_t* pdata, uint16_t len);
        ICM42688_StatusTypeDef readReg(uint8_t reg, uint8_t* pdata, uint16_t len);
    
        ICM42688_MountingOrientation _mounting;

        #ifdef HAL_I2C_MODULE_ENABLED
        I2C_HandleTypeDef* _hi2c = nullptr;
        uint8_t _address = NULL;
        #endif

        #ifdef HAL_SPI_MODULE_ENABLED
        ICM42688_StatusTypeDef csSet();
        ICM42688_StatusTypeDef csReset();

        SPI_HandleTypeDef* _hspi = nullptr;
        GPIO_TypeDef* _cs_port = nullptr;
        uint16_t _cs_pin = NULL;
        #endif

        #ifdef R_SPI_API_H
        ICM42688_StatusTypeDef csSet();
        ICM42688_StatusTypeDef csReset();
        #endif
};

#endif

#if 1
/**
 * @brief UI FSYNC中断默认回调
 * @param context 用户上下文
 */
void UI_FSYNC_callback(void* context);
/**
 * @brief PLL就绪中断默认回调
 * @param context 用户上下文
 */
void PLL_RDY_callback(void* context);
/**
 * @brief 复位完成中断默认回调
 * @param context 用户上下文
 */
void RESET_DONE_callback(void* context);
/**
 * @brief 数据就绪中断默认回调
 * @param context 用户上下文
 */
void DATA_RDY_callback(void* context);
/**
 * @brief FIFO阈值中断默认回调
 * @param context 用户上下文
 */
void FIFO_THS_callback(void* context);
/**
 * @brief FIFO满中断默认回调
 * @param context 用户上下文
 */
void FIFO_FULL_callback(void* context);
/**
 * @brief UI AGC就绪中断默认回调
 * @param context 用户上下文
 */
void UI_AGC_RDY_callback(void* context);
/**
 * @brief I3C错误中断默认回调
 * @param context 用户上下文
 */
void I3C_ERROR_callback(void* context);
/**
 * @brief 显著运动检测中断默认回调
 * @param context 用户上下文
 */
void SMD_callback(void* context);
/**
 * @brief Z轴唤醒中断默认回调
 * @param context 用户上下文
 */
void WOM_Z_callback(void* context);
/**
 * @brief Y轴唤醒中断默认回调
 * @param context 用户上下文
 */
void WOM_Y_callback(void* context);
/**
 * @brief X轴唤醒中断默认回调
 * @param context 用户上下文
 */
void WOM_X_callback(void* context);
/**
 * @brief 步数检测中断默认回调
 * @param context 用户上下文
 */
void STEP_DET_callback(void* context);
/**
 * @brief 步数计数器溢出中断默认回调
 * @param context 用户上下文
 */
void STEP_CNT_OF_callback(void* context);
/**
 * @brief 倾斜检测中断默认回调
 * @param context 用户上下文
 */
void TILT_DET_callback(void* context);
/**
 * @brief 唤醒检测中断默认回调
 * @param context 用户上下文
 */
void WAKE_DET_callback(void* context);
/**
 * @brief 睡眠检测中断默认回调
 * @param context 用户上下文
 */
void SLEEP_DET_callback(void* context);
/**
 * @brief 敲击检测中断默认回调
 * @param context 用户上下文
 */
void TAP_DET_callback(void* context);

#endif

