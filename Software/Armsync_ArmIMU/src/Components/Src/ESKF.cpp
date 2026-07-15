#include "ESKF.h"

extern volatile uint32_t sysTick;

uint32_t ESKF::getTick(void)
{
    #ifdef USE_HAL_DRIVER       // STM32 HAL
        return HAL_GetTick();
    #else
        return sysTick;
    #endif
}

/*===================================================================================================*/

void ESKF::q_dot_compute(const Quaternion *q, float gx, float gy, float gz, Quaternion* q_dot)
{
    q_dot->qw = 0.5f * (-q->qx * gx - q->qy * gy - q->qz * gz);
    q_dot->qx = 0.5f * (q->qw * gx + q->qy * gz - q->qz * gy);
    q_dot->qy = 0.5f * (q->qw * gy - q->qx * gz + q->qz * gx);
    q_dot->qz = 0.5f * (q->qw * gz + q->qx * gy - q->qy * gx);
}

void ESKF::q_norm(Quaternion* q)
{
    float n = sqrtf(q->qw*q->qw + q->qx*q->qx + q->qy*q->qy + q->qz*q->qz);
    if (n > 0.001f)
    {
        q->qw /= n; q->qx /= n; q->qy /= n; q->qz /= n;
    }
}

void ESKF::q_mult(Quaternion* q1, Quaternion* q2, Quaternion* result)
{
    result->qw = q1->qw*q2->qw - q1->qx*q2->qx - q1->qy*q2->qy - q1->qz*q2->qz;
    result->qx = q1->qw*q2->qx + q1->qx*q2->qw + q1->qy*q2->qz - q1->qz*q2->qy;
    result->qy = q1->qw*q2->qy - q1->qx*q2->qz + q1->qy*q2->qw + q1->qz*q2->qx;
    result->qz = q1->qw*q2->qz + q1->qx*q2->qy - q1->qy*q2->qx + q1->qz*q2->qw;
}

void ESKF::q_conjugate(Quaternion* q, Quaternion* result)
{
    result->qw = q->qw;
    result->qx = -q->qx;
    result->qy = -q->qy;
    result->qz = -q->qz;
}

/**
 * 将单位四元数转换为旋转矩阵 (ZYX 顺序)
 * @param q     四元数 [w, x, y, z]
 * @param R     输出 3x3 矩阵 (行优先)
 */
void ESKF::q_to_rot_matrix(Quaternion* q, float R[3][3]) 
{
    float w = q->qw, x = q->qx, y = q->qy, z = q->qz;
    float xx = x * x, yy = y * y, zz = z * z;
    float xy = x * y, xz = x * z, yz = y * z;
    float wx = w * x, wy = w * y, wz = w * z;

    R[0][0] = 1.0f - 2.0f * (yy + zz);
    R[0][1] = 2.0f * (xy - wz);
    R[0][2] = 2.0f * (xz + wy);
    R[1][0] = 2.0f * (xy + wz);
    R[1][1] = 1.0f - 2.0f * (xx + zz);
    R[1][2] = 2.0f * (yz - wx);
    R[2][0] = 2.0f * (xz - wy);
    R[2][1] = 2.0f * (yz + wx);
    R[2][2] = 1.0f - 2.0f * (xx + yy);
}

/*===================================================================================================*/

void ESKF::predict(float gx, float gy, float gz, float dt)
{
    // 去零偏
    float wx = gx - bgx;
    float wy = gy - bgy;
    float wz = gz - bgz;

    // 四阶RK积分
    Quaternion k1, k2, k3, k4, q_temp;

    q_dot_compute(&q, wx, wy, wz, &k1);      // k1 = f(q, t)
    q_temp = {q.qw + k1.qw * dt * 0.5f, q.qx + k1.qx * dt * 0.5f, q.qy + k1.qy * dt * 0.5f, q.qz + k1.qz * dt * 0.5f};
    q_norm(&q_temp);
    q_dot_compute(&q_temp, wx, wy, wz, &k2); // k2 = f(q + k1*dt/2, t + dt/2)
    q_temp = {q.qw + k2.qw * dt * 0.5f, q.qx + k2.qx * dt * 0.5f, q.qy + k2.qy * dt * 0.5f, q.qz + k2.qz * dt * 0.5f};
    q_norm(&q_temp);
    q_dot_compute(&q_temp, wx, wy, wz, &k3); // k3 = f(q + k2*dt/2, t + dt/2)
    q_temp = {q.qw + k3.qw * dt, q.qx + k3.qx * dt, q.qy + k3.qy * dt, q.qz + k3.qz * dt};
    q_norm(&q_temp);
    q_dot_compute(&q_temp, wx, wy, wz, &k4); // k4 = f(q + k3*dt, t + dt)
    q.qw += (k1.qw + 2.0f * k2.qw + 2.0f * k3.qw + k4.qw) * dt / 6.0f;
    q.qx += (k1.qx + 2.0f * k2.qx + 2.0f * k3.qx + k4.qx) * dt / 6.0f;
    q.qy += (k1.qy + 2.0f * k2.qy + 2.0f * k3.qy + k4.qy) * dt / 6.0f;
    q.qz += (k1.qz + 2.0f * k2.qz + 2.0f * k3.qz + k4.qz) * dt / 6.0f;

    // Quaternion q_dot;
    // q_dot_compute(&q, wx, wy, wz, &q_dot);
    // q.qw += q_dot.qw * dt;
    // q.qx += q_dot.qx * dt;
    // q.qy += q_dot.qy * dt;
    // q.qz += q_dot.qz * dt;

    // 归一化四元数
    q_norm(&q);

    // 构建离散F矩阵，为后面卡尔曼滤波准备    左上角 I - [w]× * dt, 右上角 -I * dt, 左下角为0，右下角为I
    F[0][0] = 1.0f;      F[0][1] =  wz * dt;   F[0][2] = -wy * dt;
    F[1][0] = -wz * dt;  F[1][1] = 1.0f;       F[1][2] =  wx * dt;
    F[2][0] =  wy * dt;  F[2][1] = -wx * dt;   F[2][2] = 1.0f;

    F[0][3] = -dt;   F[1][4] = -dt;   F[2][5] = -dt;

    F[3][3] = 1.0f;  F[4][4] = 1.0f;  F[5][5] = 1.0f;

    // 过程噪声协方差Q（对角线元素）
    Q_diag[0] = Q_diag[1] = Q_diag[2] = GYRO_NOISE * GYRO_NOISE * dt;  // 姿态误差
    Q_diag[3] = Q_diag[4] = Q_diag[5] = GYRO_BIAS_NOISE * GYRO_BIAS_NOISE * dt;  // 零偏误差
}

void ESKF::update_accel(float ax, float ay, float az)
{
    // 将世界坐标系的g转化到机体坐标系 g_body = q^-1 * g_world * q
    Quaternion g_world = {0, 0, 0, g};
    Quaternion g_body, q_conj, g_temp;
    q_conjugate(&q, &q_conj);
    q_mult(&q_conj, &g_world, &g_temp);
    q_mult(&g_temp, &q, &g_body);

    // 计算加速度残差 y = a_measured - a_predicted
    float y[3];
    y[0] = ax - g_body.qx;
    y[1] = ay - g_body.qy;
    y[2] = az - g_body.qz;

    // kalman滤波
    // 构建H矩阵（3x6）- 只测量姿态误差，不直接测量零偏
    H[0][0] = 0;            H[0][1] = -g_body.qz;   H[0][2] = g_body.qy;    H[0][3] = 0; H[0][4] = 0; H[0][5] = 0;
    H[1][0] = g_body.qz;    H[1][1] = 0;            H[1][2] = -g_body.qx;   H[1][3] = 0; H[1][4] = 0; H[1][5] = 0;
    H[2][0] = -g_body.qy;   H[2][1] = g_body.qx;    H[2][2] = 0;            H[2][3] = 0; H[2][4] = 0; H[2][5] = 0;

    // 计算先验P_curr = F * P_last * F' + Q
    // FP = F * P_last
    float FP[6][6];

    FP[0][0] = F[0][0]*P[0][0] + F[0][1]*P[1][0] + F[0][2]*P[2][0] + F[0][3]*P[3][0];
    FP[0][1] = F[0][0]*P[0][1] + F[0][1]*P[1][1] + F[0][2]*P[2][1] + F[0][3]*P[3][1];
    FP[0][2] = F[0][0]*P[0][2] + F[0][1]*P[1][2] + F[0][2]*P[2][2] + F[0][3]*P[3][2];
    FP[0][3] = F[0][0]*P[0][3] + F[0][1]*P[1][3] + F[0][2]*P[2][3] + F[0][3]*P[3][3];
    FP[0][4] = F[0][0]*P[0][4] + F[0][1]*P[1][4] + F[0][2]*P[2][4] + F[0][3]*P[3][4];
    FP[0][5] = F[0][0]*P[0][5] + F[0][1]*P[1][5] + F[0][2]*P[2][5] + F[0][3]*P[3][5];

    FP[1][0] = F[1][0]*P[0][0] + F[1][1]*P[1][0] + F[1][2]*P[2][0] + F[1][4]*P[4][0];
    FP[1][1] = F[1][0]*P[0][1] + F[1][1]*P[1][1] + F[1][2]*P[2][1] + F[1][4]*P[4][1];
    FP[1][2] = F[1][0]*P[0][2] + F[1][1]*P[1][2] + F[1][2]*P[2][2] + F[1][4]*P[4][2];
    FP[1][3] = F[1][0]*P[0][3] + F[1][1]*P[1][3] + F[1][2]*P[2][3] + F[1][4]*P[4][3];
    FP[1][4] = F[1][0]*P[0][4] + F[1][1]*P[1][4] + F[1][2]*P[2][4] + F[1][4]*P[4][4];
    FP[1][5] = F[1][0]*P[0][5] + F[1][1]*P[1][5] + F[1][2]*P[2][5] + F[1][4]*P[4][5];

    FP[2][0] = F[2][0]*P[0][0] + F[2][1]*P[1][0] + F[2][2]*P[2][0] + F[2][5]*P[5][0];
    FP[2][1] = F[2][0]*P[0][1] + F[2][1]*P[1][1] + F[2][2]*P[2][1] + F[2][5]*P[5][1];
    FP[2][2] = F[2][0]*P[0][2] + F[2][1]*P[1][2] + F[2][2]*P[2][2] + F[2][5]*P[5][2];
    FP[2][3] = F[2][0]*P[0][3] + F[2][1]*P[1][3] + F[2][2]*P[2][3] + F[2][5]*P[5][3];
    FP[2][4] = F[2][0]*P[0][4] + F[2][1]*P[1][4] + F[2][2]*P[2][4] + F[2][5]*P[5][4];
    FP[2][5] = F[2][0]*P[0][5] + F[2][1]*P[1][5] + F[2][2]*P[2][5] + F[2][5]*P[5][5];

    FP[3][0] = P[3][0];
    FP[3][1] = P[3][1];
    FP[3][2] = P[3][2];
    FP[3][3] = P[3][3];
    FP[3][4] = P[3][4];
    FP[3][5] = P[3][5];

    FP[4][0] = P[4][0];
    FP[4][1] = P[4][1];
    FP[4][2] = P[4][2];
    FP[4][3] = P[4][3];
    FP[4][4] = P[4][4];
    FP[4][5] = P[4][5];

    FP[5][0] = P[5][0];
    FP[5][1] = P[5][1];
    FP[5][2] = P[5][2];
    FP[5][3] = P[5][3];
    FP[5][4] = P[5][4];
    FP[5][5] = P[5][5];

    // FPF = FP * F'
    float FPF[6][6];
    FPF[0][0] = FP[0][0]*F[0][0] + FP[0][1]*F[0][1] + FP[0][2]*F[0][2] + FP[0][3]*F[0][3] + Q_diag[0];
    FPF[0][1] = FP[0][0]*F[1][0] + FP[0][1]*F[1][1] + FP[0][2]*F[1][2] + FP[0][4]*F[1][4];
    FPF[0][2] = FP[0][0]*F[2][0] + FP[0][1]*F[2][1] + FP[0][2]*F[2][2] + FP[0][5]*F[2][5];
    FPF[0][3] = FP[0][3];
    FPF[0][4] = FP[0][4];
    FPF[0][5] = FP[0][5];

    FPF[1][0] = FP[1][0]*F[0][0] + FP[1][1]*F[0][1] + FP[1][2]*F[0][2] + FP[1][3]*F[0][3];
    FPF[1][1] = FP[1][0]*F[1][0] + FP[1][1]*F[1][1] + FP[1][2]*F[1][2] + FP[1][4]*F[1][4] + Q_diag[1];
    FPF[1][2] = FP[1][0]*F[2][0] + FP[1][1]*F[2][1] + FP[1][2]*F[2][2] + FP[1][5]*F[2][5];
    FPF[1][3] = FP[1][3];
    FPF[1][4] = FP[1][4];
    FPF[1][5] = FP[1][5];

    FPF[2][0] = FP[2][0]*F[0][0] + FP[2][1]*F[0][1] + FP[2][2]*F[0][2] + FP[2][3]*F[0][3];
    FPF[2][1] = FP[2][0]*F[1][0] + FP[2][1]*F[1][1] + FP[2][2]*F[1][2] + FP[2][4]*F[1][4];
    FPF[2][2] = FP[2][0]*F[2][0] + FP[2][1]*F[2][1] + FP[2][2]*F[2][2] + FP[2][5]*F[2][5] + Q_diag[2];
    FPF[2][3] = FP[2][3];
    FPF[2][4] = FP[2][4];
    FPF[2][5] = FP[2][5];

    FPF[3][0] = FP[3][0]*F[0][0] + FP[3][1]*F[0][1] + FP[3][2]*F[0][2] + FP[3][3]*F[0][3];
    FPF[3][1] = FP[3][0]*F[1][0] + FP[3][1]*F[1][1] + FP[3][2]*F[1][2] + FP[3][4]*F[1][4];
    FPF[3][2] = FP[3][0]*F[2][0] + FP[3][1]*F[2][1] + FP[3][2]*F[2][2] + FP[3][5]*F[2][5];
    FPF[3][3] = FP[3][3] + Q_diag[3];
    FPF[3][4] = FP[3][4];
    FPF[3][5] = FP[3][5];

    FPF[4][0] = FP[4][0]*F[0][0] + FP[4][1]*F[0][1] + FP[4][2]*F[0][2] + FP[4][3]*F[0][3];
    FPF[4][1] = FP[4][0]*F[1][0] + FP[4][1]*F[1][1] + FP[4][2]*F[1][2] + FP[4][4]*F[1][4];
    FPF[4][2] = FP[4][0]*F[2][0] + FP[4][1]*F[2][1] + FP[4][2]*F[2][2] + FP[4][5]*F[2][5];
    FPF[4][3] = FP[4][3];
    FPF[4][4] = FP[4][4] + Q_diag[4];
    FPF[4][5] = FP[4][5];

    FPF[5][0] = FP[5][0]*F[0][0] + FP[5][1]*F[0][1] + FP[5][2]*F[0][2] + FP[5][3]*F[0][3];
    FPF[5][1] = FP[5][0]*F[1][0] + FP[5][1]*F[1][1] + FP[5][2]*F[1][2] + FP[5][4]*F[1][4];
    FPF[5][2] = FP[5][0]*F[2][0] + FP[5][1]*F[2][1] + FP[5][2]*F[2][2] + FP[5][5]*F[2][5];
    FPF[5][3] = FP[5][3];
    FPF[5][4] = FP[5][4];
    FPF[5][5] = FP[5][5] + Q_diag[5];

    // 计算卡尔曼增益 K = P * H' * (H * P * H' + R)^-1
    // PHt = P * H'
    float PHt[6][3];
    // 第0行
    PHt[0][0] = FPF[0][1]*H[0][1] + FPF[0][2]*H[0][2];
    PHt[0][1] = FPF[0][0]*H[1][0] + FPF[0][2]*H[1][2];
    PHt[0][2] = FPF[0][0]*H[2][0] + FPF[0][1]*H[2][1];

    // 第1行
    PHt[1][0] = FPF[1][1]*H[0][1] + FPF[1][2]*H[0][2];
    PHt[1][1] = FPF[1][0]*H[1][0] + FPF[1][2]*H[1][2];
    PHt[1][2] = FPF[1][0]*H[2][0] + FPF[1][1]*H[2][1];

    // 第2行
    PHt[2][0] = FPF[2][1]*H[0][1] + FPF[2][2]*H[0][2];
    PHt[2][1] = FPF[2][0]*H[1][0] + FPF[2][2]*H[1][2];
    PHt[2][2] = FPF[2][0]*H[2][0] + FPF[2][1]*H[2][1];

    // 第3行
    PHt[3][0] = FPF[3][1]*H[0][1] + FPF[3][2]*H[0][2];
    PHt[3][1] = FPF[3][0]*H[1][0] + FPF[3][2]*H[1][2];
    PHt[3][2] = FPF[3][0]*H[2][0] + FPF[3][1]*H[2][1];

    // 第4行
    PHt[4][0] = FPF[4][1]*H[0][1] + FPF[4][2]*H[0][2];
    PHt[4][1] = FPF[4][0]*H[1][0] + FPF[4][2]*H[1][2];
    PHt[4][2] = FPF[4][0]*H[2][0] + FPF[4][1]*H[2][1];

    // 第5行
    PHt[5][0] = FPF[5][1]*H[0][1] + FPF[5][2]*H[0][2];
    PHt[5][1] = FPF[5][0]*H[1][0] + FPF[5][2]*H[1][2];
    PHt[5][2] = FPF[5][0]*H[2][0] + FPF[5][1]*H[2][1];

    // S = H * P * H' + R
    float S[3][3];
    float R_diag = ACC_NOISE;
    S[0][0] = H[0][1] * PHt[1][0] + H[0][2] * PHt[2][0] + R_diag;
    S[0][1] = H[0][1] * PHt[1][1] + H[0][2] * PHt[2][1];
    S[0][2] = H[0][1] * PHt[1][2] + H[0][2] * PHt[2][2];

    S[1][0] = H[1][0] * PHt[0][0] + H[1][2] * PHt[2][0];
    S[1][1] = H[1][0] * PHt[0][1] + H[1][2] * PHt[2][1] + R_diag;
    S[1][2] = H[1][0] * PHt[0][2] + H[1][2] * PHt[2][2];

    S[2][0] = H[2][0] * PHt[0][0] + H[2][1] * PHt[1][0];
    S[2][1] = H[2][0] * PHt[0][1] + H[2][1] * PHt[1][1];
    S[2][2] = H[2][0] * PHt[0][2] + H[2][1] * PHt[1][2] + R_diag;

    // 求S的逆矩阵 S^-1
    float detS = S[0][0]*(S[1][1]*S[2][2] - S[1][2]*S[2][1]) - S[0][1]*(S[1][0]*S[2][2] - S[1][2]*S[2][0]) + S[0][2]*(S[1][0]*S[2][1] - S[1][1]*S[2][0]);
    float invS[3][3];
    if (fabsf(detS) > 1e-4f)
    {
        float invDetS = 1.0f / detS;
        invS[0][0] =  (S[1][1]*S[2][2] - S[1][2]*S[2][1]) * invDetS;
        invS[0][1] = -(S[0][1]*S[2][2] - S[0][2]*S[2][1]) * invDetS;
        invS[0][2] =  (S[0][1]*S[1][2] - S[0][2]*S[1][1]) * invDetS;
        invS[1][0] = -(S[1][0]*S[2][2] - S[1][2]*S[2][0]) * invDetS;
        invS[1][1] =  (S[0][0]*S[2][2] - S[0][2]*S[2][0]) * invDetS;
        invS[1][2] = -(S[0][0]*S[1][2] - S[0][2]*S[1][0]) * invDetS;
        invS[2][0] =  (S[1][0]*S[2][1] - S[1][1]*S[2][0]) * invDetS;
        invS[2][1] = -(S[0][0]*S[2][1] - S[0][1]*S[2][0]) * invDetS;
        invS[2][2] =  (S[0][0]*S[1][1] - S[0][1]*S[1][0]) * invDetS;
    }

    // K = P * H' * S^-1
    float K[6][3];
    K[0][0] = PHt[0][0]*invS[0][0] + PHt[0][1]*invS[1][0] + PHt[0][2]*invS[2][0];
    K[0][1] = PHt[0][0]*invS[0][1] + PHt[0][1]*invS[1][1] + PHt[0][2]*invS[2][1];
    K[0][2] = PHt[0][0]*invS[0][2] + PHt[0][1]*invS[1][2] + PHt[0][2]*invS[2][2];

    K[1][0] = PHt[1][0]*invS[0][0] + PHt[1][1]*invS[1][0] + PHt[1][2]*invS[2][0];
    K[1][1] = PHt[1][0]*invS[0][1] + PHt[1][1]*invS[1][1] + PHt[1][2]*invS[2][1];
    K[1][2] = PHt[1][0]*invS[0][2] + PHt[1][1]*invS[1][2] + PHt[1][2]*invS[2][2];

    K[2][0] = PHt[2][0]*invS[0][0] + PHt[2][1]*invS[1][0] + PHt[2][2]*invS[2][0];
    K[2][1] = PHt[2][0]*invS[0][1] + PHt[2][1]*invS[1][1] + PHt[2][2]*invS[2][1];
    K[2][2] = PHt[2][0]*invS[0][2] + PHt[2][1]*invS[1][2] + PHt[2][2]*invS[2][2];

    K[3][0] = PHt[3][0]*invS[0][0] + PHt[3][1]*invS[1][0] + PHt[3][2]*invS[2][0];
    K[3][1] = PHt[3][0]*invS[0][1] + PHt[3][1]*invS[1][1] + PHt[3][2]*invS[2][1];
    K[3][2] = PHt[3][0]*invS[0][2] + PHt[3][1]*invS[1][2] + PHt[3][2]*invS[2][2];

    K[4][0] = PHt[4][0]*invS[0][0] + PHt[4][1]*invS[1][0] + PHt[4][2]*invS[2][0];
    K[4][1] = PHt[4][0]*invS[0][1] + PHt[4][1]*invS[1][1] + PHt[4][2]*invS[2][1];
    K[4][2] = PHt[4][0]*invS[0][2] + PHt[4][1]*invS[1][2] + PHt[4][2]*invS[2][2];

    K[5][0] = PHt[5][0]*invS[0][0] + PHt[5][1]*invS[1][0] + PHt[5][2]*invS[2][0];
    K[5][1] = PHt[5][0]*invS[0][1] + PHt[5][1]*invS[1][1] + PHt[5][2]*invS[2][1];
    K[5][2] = PHt[5][0]*invS[0][2] + PHt[5][1]*invS[1][2] + PHt[5][2]*invS[2][2];

    // 残差注入
    ex = K[0][0]*y[0] + K[0][1]*y[1] + K[0][2]*y[2];
    ey = K[1][0]*y[0] + K[1][1]*y[1] + K[1][2]*y[2];
    ez = K[2][0]*y[0] + K[2][1]*y[1] + K[2][2]*y[2];
    ebgx = K[3][0]*y[0] + K[3][1]*y[1] + K[3][2]*y[2];
    ebgy = K[4][0]*y[0] + K[4][1]*y[1] + K[4][2]*y[2];
    ebgz = K[5][0]*y[0] + K[5][1]*y[1] + K[5][2]*y[2];

    ebgz = 0;  // 不能改变z,会出问题,yaw会不自觉飘

    Quaternion dq = {1.0f, ex/2.0f, ey/2.0f, ez/2.0f};
    q_norm(&dq);

    Quaternion q_new;
    q_mult(&q, &dq, &q_new);  // 更新名义状态
    q = q_new;
    q_norm(&q);

    bgx += ebgx;
    bgy += ebgy;
    bgz += ebgz;

    // 误差状态回零
    ex = ey = ez = 0;
    ebgx = ebgy = ebgz = 0;

    // 求协方差P的后验 P_post = (I - K * H) * P_prior = P_prior - K * H * P_prior
    // 计算H * P_prior
    float HP[3][6];
    // 第0行
    HP[0][0] = H[0][1]*FPF[1][0] + H[0][2]*FPF[2][0];
    HP[0][1] = H[0][1]*FPF[1][1] + H[0][2]*FPF[2][1];
    HP[0][2] = H[0][1]*FPF[1][2] + H[0][2]*FPF[2][2];
    HP[0][3] = H[0][1]*FPF[1][3] + H[0][2]*FPF[2][3];
    HP[0][4] = H[0][1]*FPF[1][4] + H[0][2]*FPF[2][4];
    HP[0][5] = H[0][1]*FPF[1][5] + H[0][2]*FPF[2][5];
    // 第1行
    HP[1][0] = H[1][0]*FPF[0][0] + H[1][2]*FPF[2][0];
    HP[1][1] = H[1][0]*FPF[0][1] + H[1][2]*FPF[2][1];
    HP[1][2] = H[1][0]*FPF[0][2] + H[1][2]*FPF[2][2];
    HP[1][3] = H[1][0]*FPF[0][3] + H[1][2]*FPF[2][3];
    HP[1][4] = H[1][0]*FPF[0][4] + H[1][2]*FPF[2][4];
    HP[1][5] = H[1][0]*FPF[0][5] + H[1][2]*FPF[2][5];
    // 第2行
    HP[2][0] = H[2][0]*FPF[0][0] + H[2][1]*FPF[1][0];
    HP[2][1] = H[2][0]*FPF[0][1] + H[2][1]*FPF[1][1];
    HP[2][2] = H[2][0]*FPF[0][2] + H[2][1]*FPF[1][2];
    HP[2][3] = H[2][0]*FPF[0][3] + H[2][1]*FPF[1][3];
    HP[2][4] = H[2][0]*FPF[0][4] + H[2][1]*FPF[1][4];
    HP[2][5] = H[2][0]*FPF[0][5] + H[2][1]*FPF[1][5];

    float KHP[6][6];
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            KHP[i][j] = K[i][0]*HP[0][j] + K[i][1]*HP[1][j] + K[i][2]*HP[2][j];
        }
    }

    // 计算K * H * P_prior
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            P[i][j] = FPF[i][j] - KHP[i][j];
        }
    }

    for (int i = 0; i < 6; i++) {
        for (int j = i+1; j < 6; j++) {
            float avg = (P[i][j] + P[j][i]) * 0.5f;
            P[i][j] = P[j][i] = avg;
        }
    }
}

void ESKF::update_mag(float mx, float my, float mz)
{
    static float last_mag_x = 0, last_mag_y = 0, last_mag_z = 0;
    const float alpha = 0.2f;  // 滤波系数

    mx = mx * alpha + last_mag_x * (1 - alpha);
    my = my * alpha + last_mag_y * (1 - alpha);
    mz = mz * alpha + last_mag_z * (1 - alpha);

    last_mag_x = mx;
    last_mag_y = my;
    last_mag_z = mz;

    float norm = sqrtf(mx*mx + my*my + mz*mz);
    mx /= norm; my /= norm; mz /= norm;

    // 将世界坐标系下的磁力计数据转换到机体坐标系
    Quaternion mag_world = {0, 0, 1, 0};
    Quaternion mag_body, q_conj, mag_temp;
    q_conjugate(&q, &q_conj);
    q_mult(&q_conj, &mag_world, &mag_temp);
    q_mult(&mag_temp, &q, &mag_body);

    // 计算误差（叉积 measured × predicted）的 Z 分量，只修正偏航
    float ez = my * mag_body.qx - mx * mag_body.qy;
    ez *= -1.0f;

    if (ez > 1.0f) ez = 1.0f;
    if (ez < -1.0f) ez = -1.0f;

    static float integral_ez = 0.0f;
    const float Kp = 0.82f;      // 比例增益（越大修正越快）
    const float Ki = 0.06f;     // 积分增益（消除静态误差）
    const float integral_limit = 0.82f;  // 积分限幅

    integral_ez += Ki * ez;
    if (integral_ez >  integral_limit) integral_ez =  integral_limit;
    if (integral_ez < -integral_limit) integral_ez = -integral_limit;

    float delta_psi = Kp * ez + integral_ez;

    // 注意：delta_psi 为正表示逆时针旋转（北东天逆时针为正），若实际效果反了，可加负号
    float c = cosf(0.5f * -delta_psi);
    float s = sinf(0.5f * -delta_psi);
    Quaternion q_delta = {c, 0, 0, s};   // 绕 Z 轴旋转

    // 更新四元数：q_new = q * q_delta
    Quaternion q_new;
    q_mult(&q, &q_delta, &q_new);
    q_norm(&q_new);

    // 写回成员变量
    q = q_new;
}

/*===================================================================================================*/

ESKF::ESKF(float GYRO_NOISE, float GYRO_BIAS_NOISE, float ACC_NOISE)
    : GYRO_NOISE(GYRO_NOISE), GYRO_BIAS_NOISE(GYRO_BIAS_NOISE), ACC_NOISE(ACC_NOISE)
{
    // 初始化名义状态
    q_o.qw = 1.0f; q_o.qx = 0.0f; q_o.qy = 0.0f; q_o.qz = 0.0f;
    q.qw = 1.0f; q.qx = 0.0f; q.qy = 0.0f; q.qz = 0.0f;
    bgx = bgy = bgz = 0.0f;

    // 初始化误差状态
    ex = ey = ez = 0.0f;
    ebgx = ebgy = ebgz = 0.0f;

    // 初始化协方差矩阵 P 为单位矩阵
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 6; j++)
            P[i][j] = (i == j) ? 0.01f : 0.0f;

     // 初始化状态转移矩阵 F 为单位矩阵
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 6; j++)
            F[i][j] = (i == j) ? 1.0f : 0.0f;

    // 初始化观测矩阵 H - 只测量姿态误差
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 6; j++)
            H[i][j] = (i == j) ? 1.0f : 0.0f;

    // 初始化过程噪声协方差 Q - 只考虑陀螺噪声和零偏噪声
    Q_diag[0] = Q_diag[1] = Q_diag[2] = GYRO_NOISE * GYRO_NOISE;
    Q_diag[3] = Q_diag[4] = Q_diag[5] = GYRO_BIAS_NOISE * GYRO_BIAS_NOISE;
}

void ESKF::update(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
    static uint32_t last_time = 0;
    uint32_t current_time = getTick();
    float dt = (current_time - last_time) / 1000.0f;  // 转换为秒
    last_time = current_time;

    static uint8_t first_update = 0;
    if(first_update == 0)
    {
        first_update = 1;
        return; // 跳过第一次更新，避免dt过大
    }

    // 1. 预测步骤W
    predict(gx, gy, gz, dt);

    // 2. 加速度计更新
    update_accel(ax, ay, az);

    // 3. 磁力计更新)
    if(az > 6.0f || az < -6.0f)  // 只有当加速度计测量到重力时才更新磁力计，避免飞行中磁力计数据不可靠
        update_mag(mx, my, mz);

    q_o.qw = q.qw;
    q_o.qx = q.qx;
    q_o.qy = q.qy;
    q_o.qz = q.qz;
}



