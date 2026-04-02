#ifndef __ESKF_H__
#define __ESKF_H__

#include <math.h>

// 参数
#define G           9.80655f
#define PI          3.14159265f
#define DEG2RAD     0.01745329f
#define RAD2DEG     57.2957795f

// 噪声参数 - 平衡响应速度和稳定性
#define SIGMA_G     0.04f      // 陀螺噪声 (rad/s)
#define SIGMA_BG    0.0001f      // 陀螺零偏游走
#define SIGMA_BA    0.0006f      // 加速度零偏游走  
#define SIGMA_A     0.8f        // 加速度观测噪声 - 减小，更信任加速度
#define SIGMA_M     0.02f        // 磁力计观测噪声

typedef struct {
    // 名义状态
    float qw, qx, qy, qz;
    float bgx, bgy, bgz;        // rad/s
    float bax, bay, baz;        // m/s²
    
    // 完整 9x9 协方差矩阵
    float P[9][9];

    float qw_f, qx_f, qy_f, qz_f;
    float pitch, roll, yaw;
} ESKF_State;

void eskf_init(ESKF_State* s);
void eskf_update(ESKF_State* s, float gx, float gy, float gz,  // °/s
                 float ax, float ay, float az,                  // m/s²
                 float mx, float my, float mz);                 // 任意单位
void eskf_get_euler(ESKF_State* s, float* roll, float* pitch, float* yaw);

#endif
