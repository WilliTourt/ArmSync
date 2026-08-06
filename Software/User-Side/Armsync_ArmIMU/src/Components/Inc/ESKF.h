#ifndef __ESKF_H__
#define __ESKF_H__

// ESKF
// 假定坐标系为：y朝北，x朝东，z朝上（右手坐标系）（NEU）。所有的角速度方向都是右手定则的方向，即绕y轴正向旋转为正roll，绕x轴正向旋转为正pitch，绕z轴正向旋转为正yaw。
// z
// ^
// |   y
// |  /
// | /
// |/_________>x
//
// 输入单位：陀螺仪：rad/s；加速度计：m/s²；磁力计：任意（通常是微特斯拉）
//
// Date: 2026-4-7
// Author: Zodiac-321
//


// STM32 HAL环境
#ifdef USE_HAL_DRIVER
    #include "cpp_main.h"
    #include "stdint.h"
    #include "math.h"
#else
    #include "cpp_main.h"
    #include "stdint.h"
    #include "math.h"
#endif

// 避免与ICM42688.h重复定义
#ifndef g
#define g 9.806651f
#endif
#ifndef PI
#define PI 3.14159265f
#endif

class ESKF
{
    public:
        ESKF(float GYRO_NOISE = 0.01f, float GYRO_BIAS_NOISE = 0.002f, float ACC_NOISE = 0.05f);

        /**
         * update函数：用于更新输出四元数
         * 输入参数：
         * ax, ay, az: 加速度计测量的加速度值，单位为m/s^2
         * gx, gy, gz: 陀螺仪测量的角速度值，单位为rad/s
         * mx, my, mz: 磁力计测量的磁场值，单位为任意（通常是微特斯拉）
         */
        void update(float ax, float ay, float az, 
                    float gx, float gy, float gz, 
                    float mx, float my, float mz);

        inline float get_qw() { return q_o.qw; }
        inline float get_qx() { return q_o.qx; }
        inline float get_qy() { return q_o.qy; }
        inline float get_qz() { return q_o.qz; }
    
    private:
        float GYRO_NOISE = 0.01f;     // rad/s
        float GYRO_BIAS_NOISE = 0.002f; // rad/s
        float ACC_NOISE = 0.05f;       // m/s^2

        typedef struct
        {
            float qw, qx, qy, qz;
        } Quaternion;

        typedef struct
        {
            float roll, pitch, yaw;
        } EulerAngles;

        uint32_t getTick(void);
        void q_dot_compute(const Quaternion *q, float gx, float gy, float gz, Quaternion* q_dot);
        void q_norm(Quaternion* q);
        void q_mult(Quaternion* q1, Quaternion* q2, Quaternion* result);     // q1 * q2
        void q_conjugate(Quaternion* q, Quaternion* result);                 // q^-1
        void q_to_rot_matrix(Quaternion* q, float R[3][3]);

        void predict(float gx, float gy, float gz, float dt);
        void update_accel(float ax, float ay, float az);
        void update_mag(float mx, float my, float mz);
        
        Quaternion q_o;

        // 名义状态
        Quaternion q;    // 姿态四元数
        float bgx, bgy, bgz;  // 陀螺仪零偏估计

        // 误差状态
        float ex, ey, ez;     // 姿态误差
        float ebgx, ebgy, ebgz;  // 零偏误差

        float P[6][6];  // 误差协方差矩阵
        float F[6][6];  // 状态转移矩阵
        float H[3][6];  // 观测矩阵
        float Q_diag[6]; // 过程噪声协方差（对角线元素）
};

#endif
