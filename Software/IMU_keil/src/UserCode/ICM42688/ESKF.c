#include "ESKF.h"
#include "cpp_main.h"

// 辅助：四元数归一化
static void quat_norm(float* w, float* x, float* y, float* z) 
{
    float n = sqrtf(*w**w + *x**x + *y**y + *z**z);
    if (n > 0.001f)
        *w /= n; *x /= n; *y /= n; *z /= n;
}

// 辅助：限制数值范围
static float clamp(float val, float min, float max) 
{
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

// 初始化
void eskf_init(ESKF_State* s) 
{
    s->qw = 1.0f; s->qx = 0.0f; s->qy = 0.0f; s->qz = 0.0f;
    s->bgx = s->bgy = s->bgz = 0.0f;
    s->bax = s->bay = s->baz = 0.0f;
    
    // 初始化 P 为对角矩阵，初始值较小表示初始不确定性小
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            s->P[i][j] = (i == j) ? 0.01f : 0.0f;
}

// 步骤1: 预测
static void eskf_predict(ESKF_State* s, float gx, float gy, float gz, float dt) 
{
    // 去零偏（输入是 rad/s）
    float wx = gx - s->bgx;
    float wy = gy - s->bgy;
    float wz = gz - s->bgz;
    
    // 限制角速度，防止异常
    wx = clamp(wx, -20.0f, 20.0f);
    wy = clamp(wy, -20.0f, 20.0f);
    wz = clamp(wz, -20.0f, 20.0f);
    
    // 动态零偏衰减：长时间没有运动时，逐渐减小零偏估计
    // 这有助于消除累积的零偏误差
    float gyro_mag = sqrtf(wx*wx + wy*wy + wz*wz);
    if (gyro_mag < 0.1f)    // 几乎静止时
    {  
        float decay = 0.999f;  // 零偏衰减因子
        s->bgx *= decay;
        s->bgy *= decay;
        s->bgz *= decay;
    }
    
    // 四元数积分（一阶龙格库塔）
    float qw = s->qw, qx = s->qx, qy = s->qy, qz = s->qz;
    float dt2 = dt * 0.5f;
    
    float qw_dot = 0.5f * (-qx*wx - qy*wy - qz*wz);
    float qx_dot = 0.5f * ( qw*wx + qz*wy - qy*wz);
    float qy_dot = 0.5f * (-qz*wx + qw*wy + qx*wz);
    float qz_dot = 0.5f * ( qy*wx - qx*wy + qw*wz);
    
    float qw_new = qw + qw_dot * dt;
    float qx_new = qx + qx_dot * dt;
    float qy_new = qy + qy_dot * dt;
    float qz_new = qz + qz_dot * dt;
    
    quat_norm(&qw_new, &qx_new, &qy_new, &qz_new);
    s->qw = qw_new; s->qx = qx_new; s->qy = qy_new; s->qz = qz_new;
    
    // 构建 F 矩阵（9x9）- 使用简单形式
    // 状态: [δθx, δθy, δθz, δbgx, δbgy, δbgz, δbax, δbay, δbaz]
    float F[9][9];
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            F[i][j] = (i == j) ? 1.0f : 0.0f;
    
    float wxt = wx*dt, wyt = wy*dt, wzt = wz*dt;
    
    // 姿态误差转移: I - [w]x * dt
    F[0][1] = -wzt;  F[0][2] = wyt;
    F[1][0] = wzt;   F[1][2] = -wxt;
    F[2][0] = -wyt;  F[2][1] = wxt;
    
    // 耦合: -I * dt
    F[0][3] = -dt; F[1][4] = -dt; F[2][5] = -dt;
    
    // P = F * P * F' + Q (简化计算，只算对角和关键非对角)
    float P_new[9][9];
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++) 
        {
            P_new[i][j] = 0;
            for (int k = 0; k < 9; k++)
                for (int m = 0; m < 9; m++)
                    P_new[i][j] += F[i][k] * s->P[k][m] * F[j][m];
        }
    
    // 加 Q
    float q_g = SIGMA_G * SIGMA_G * dt;
    float q_bg = SIGMA_BG * SIGMA_BG * dt;
    float q_ba = SIGMA_BA * SIGMA_BA * dt;
    
    P_new[0][0] += q_g;  P_new[1][1] += q_g;  P_new[2][2] += q_g;
    P_new[3][3] += q_bg; P_new[4][4] += q_bg; P_new[5][5] += q_bg;
    P_new[6][6] += q_ba; P_new[7][7] += q_ba; P_new[8][8] += q_ba;
    
    // 限制协方差增长，防止发散
    for (int i = 0; i < 9; i++)
        P_new[i][i] = clamp(P_new[i][i], 0.0001f, 10.0f);
    
    // 保持对称性
    for (int i = 0; i < 9; i++)
        for (int j = i+1; j < 9; j++)
            P_new[i][j] = P_new[j][i] = (P_new[i][j] + P_new[j][i]) * 0.5f;
    
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            s->P[i][j] = P_new[i][j];
}

// 步骤2: 加速度计更新 - 使用最简化的稳定版本
static void eskf_update_accel(ESKF_State* s, float ax, float ay, float az) 
{
    // 检查加速度有效性
    float acc_mag = sqrtf(ax*ax + ay*ay + az*az);
    if (acc_mag < 0.1f || acc_mag > 20.0f) return; // 无效数据，跳过更新
    
    float qw = s->qw, qx = s->qx, qy = s->qy, qz = s->qz;
    
    // 从四元数提取重力方向（简化计算）
    // 重力在机体坐标系 = R' * [0,0,G]
    float gx_pred = 2.0f * (qx*qz - qw*qy) * G;
    float gy_pred = 2.0f * (qy*qz + qw*qx) * G;
    float gz_pred = (1.0f - 2.0f*qx*qx - 2.0f*qy*qy) * G;
    
    // 归一化测量和预测（只关心方向）
    float a_mag = sqrtf(ax*ax + ay*ay + az*az);
    float p_mag = sqrtf(gx_pred*gx_pred + gy_pred*gy_pred + gz_pred*gz_pred);
    
    if (a_mag < 0.001f || p_mag < 0.001f) return;
    
    // 叉积得到旋转误差（最稳定的姿态修正方法）
    // error = measured_dir × predicted_dir
    float mx = ax / a_mag, my = ay / a_mag, mz = az / a_mag;
    float px = gx_pred / p_mag, py = gy_pred / p_mag, pz = gz_pred / p_mag;
    
    float ex = my*pz - mz*py;
    float ey = mz*px - mx*pz;
    float ez = mx*py - my*px;
    
    // 使用互补滤波的思想，但用 ESKF 框架
    // 误差角度 = asin(|error|)，但小角度时 ≈ |error|
    float error_mag = sqrtf(ex*ex + ey*ey + ez*ez);
    if (error_mag < 0.001f) return; // 已经对齐
    
    // 动态增益：增大以加快收敛
    // 静止时快速收敛，运动时保持平滑
    float trust = (fabsf(acc_mag - G) < 0.5f) ? 0.14f : 0.03f;
    
    // 直接修正姿态（简化 ESKF）
    // 关键：加速度计只能修正 roll/pitch，不能修正 yaw！
    float dtheta_x = ex * trust;
    float dtheta_y = ey * trust;
    float dtheta_z = 0.0f;  // 不修正 yaw，留给磁力计
    
    // 构建误差四元数
    float angle = sqrtf(dtheta_x*dtheta_x + dtheta_y*dtheta_y + dtheta_z*dtheta_z);
    float dq_w, dq_x, dq_y, dq_z;
    
    if (angle > 0.001f) 
    {
        float half = angle * 0.5f;
        dq_w = cosf(half);
        float s = sinf(half) / angle;
        dq_x = dtheta_x * s;
        dq_y = dtheta_y * s;
        dq_z = dtheta_z * s;
    } 
    else 
    {
        dq_w = 1.0f;
        dq_x = dtheta_x * 0.5f;
        dq_y = dtheta_y * 0.5f;
        dq_z = dtheta_z * 0.5f;
    }
    
    // 应用修正
    float qw_new = qw*dq_w - qx*dq_x - qy*dq_y - qz*dq_z;
    float qx_new = qw*dq_x + qx*dq_w + qy*dq_z - qz*dq_y;
    float qy_new = qw*dq_y - qx*dq_z + qy*dq_w + qz*dq_x;
    float qz_new = qw*dq_z + qx*dq_y - qy*dq_x + qz*dq_w;
    
    quat_norm(&qw_new, &qx_new, &qy_new, &qz_new);
    s->qw = qw_new; s->qx = qx_new; s->qy = qy_new; s->qz = qz_new;
    
    // 简单的零偏估计（积分误差）- 增大以加快零偏收敛
    // 注意：z轴零偏主要由磁力计修正，这里只修正x,y
    float bias_gain_xy = 0.002f;   // roll/pitch 零偏快速收敛
    float bias_gain_z = 0.0005f;   // yaw 零偏慢速收敛（避免干扰）
    s->bgx += dtheta_x * bias_gain_xy;
    s->bgy += dtheta_y * bias_gain_xy;
    s->bgz += dtheta_z * bias_gain_z;  // dtheta_z 现在是 0
    
    // 限制零偏
    s->bgx = clamp(s->bgx, -0.1f, 0.1f);
    s->bgy = clamp(s->bgy, -0.1f, 0.1f);
    s->bgz = clamp(s->bgz, -0.1f, 0.1f);
}

void eskf_update_mag(ESKF_State* s, float mx, float my, float mz)
{
    static float last_mag[3] = {0};
    const float a = 0.0f;
    mx = mx * (1 - a) + last_mag[0] * a;
    my = my * (1 - a) + last_mag[1] * a;
    mz = mz * (1 - a) + last_mag[2] * a;
    last_mag[0] = mx;
    last_mag[1] = my;
    last_mag[2] = mz;

    // 1. 归一化磁力计
    float mag_norm = sqrtf(mx*mx + my*my + mz*mz);
    if (mag_norm < 1e-6f) return;
    mx /= mag_norm;
    my /= mag_norm;
    mz /= mag_norm;

    // 2. 世界系参考磁场（假设水平指向北，无倾角）
    float ref_mag_w[3] = {1.0f, 0.0f, 0.0f};
    
    // 3. 将参考磁场旋转到机体系
    float qw = s->qw, qx = s->qx, qy = s->qy, qz = s->qz;
    // 旋转矩阵（世界→机体）
    float r11 = qw*qw + qx*qx - qy*qy - qz*qz;
    float r12 = 2.0f*(qx*qy - qw*qz);
    float r13 = 2.0f*(qx*qz + qw*qy);
    float r21 = 2.0f*(qx*qy + qw*qz);
    float r22 = qw*qw - qx*qx + qy*qy - qz*qz;
    float r23 = 2.0f*(qy*qz - qw*qx);
    float r31 = 2.0f*(qx*qz - qw*qy);
    float r32 = 2.0f*(qy*qz + qw*qx);
    float r33 = qw*qw - qx*qx - qy*qy + qz*qz;
    
    float pred_mx = r11*ref_mag_w[0] + r12*ref_mag_w[1] + r13*ref_mag_w[2];
    float pred_my = r21*ref_mag_w[0] + r22*ref_mag_w[1] + r23*ref_mag_w[2];
    float pred_mz = r31*ref_mag_w[0] + r32*ref_mag_w[1] + r33*ref_mag_w[2];
    
    // 归一化预测向量
    float pred_norm = sqrtf(pred_mx*pred_mx + pred_my*pred_my + pred_mz*pred_mz);
    if (pred_norm < 1e-6f) return;
    pred_mx /= pred_norm;
    pred_my /= pred_norm;
    pred_mz /= pred_norm;
    
    // 4. 叉积误差
    float ex = pred_my*mz - pred_mz*my;   // predicted × measured
    float ey = pred_mz*mx - pred_mx*mz;
    float ez = pred_mx*my - pred_my*mx;
    
    // 5. 动态增益（可调）
    float mag_gain = 0.02f;   // 小于加速度计增益（0.14），避免干扰roll/pitch
    
    // 6. 直接修正姿态（同加速度计更新）
    float dtheta_x = ex * mag_gain;
    float dtheta_y = ey * mag_gain;
    float dtheta_z = ez * mag_gain;
    
    float angle = sqrtf(dtheta_x*dtheta_x + dtheta_y*dtheta_y + dtheta_z*dtheta_z);
    float dq_w, dq_x, dq_y, dq_z;
    if (angle > 0.001f) {
        float half = angle * 0.5f;
        dq_w = cosf(half);
        float s = sinf(half) / angle;
        dq_x = dtheta_x * s;
        dq_y = dtheta_y * s;
        dq_z = dtheta_z * s;
    } else {
        dq_w = 1.0f;
        dq_x = dtheta_x * 0.5f;
        dq_y = dtheta_y * 0.5f;
        dq_z = dtheta_z * 0.5f;
    }
    
    float qw_new = qw*dq_w - qx*dq_x - qy*dq_y - qz*dq_z;
    float qx_new = qw*dq_x + qx*dq_w + qy*dq_z - qz*dq_y;
    float qy_new = qw*dq_y - qx*dq_z + qy*dq_w + qz*dq_x;
    float qz_new = qw*dq_z + qx*dq_y - qy*dq_x + qz*dq_w;
    
    quat_norm(&qw_new, &qx_new, &qy_new, &qz_new);
    s->qw = qw_new; s->qx = qx_new; s->qy = qy_new; s->qz = qz_new;
    
    // 7. 可选：更新陀螺零偏（主要修正z轴）
    float bias_gain = 0.005f;
    s->bgx += dtheta_x * bias_gain;
    s->bgy += dtheta_y * bias_gain;
    s->bgz += dtheta_z * bias_gain;
    s->bgx = clamp(s->bgx, -0.1f, 0.1f);
    s->bgy = clamp(s->bgy, -0.1f, 0.1f);
    s->bgz = clamp(s->bgz, -0.1f, 0.1f);
}

// 主更新函数
void eskf_update(ESKF_State* s, float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) 
{
    static int last_time = 0;
    int current_time = sysTick;
    float dt = (current_time - last_time) / 1000.0f;
    last_time = current_time;
    
    // 陀螺仪：°/s → rad/s
    eskf_predict(s, gx * DEG2RAD, gy * DEG2RAD, gz * DEG2RAD, dt);
    eskf_update_accel(s, ax, ay, az);
//    eskf_update_mag(s, mx, my, mz);

    eskf_get_euler(s, &(s->pitch), &(s->roll), &(s->yaw));
    s->qw_f = s->qw; s->qx_f = s->qx; s->qy_f = s->qy; s->qz_f = s->qz; 

    static float last_yaw = 0;
    const float a = 0;
    s->yaw = s->yaw * (1 - a) + last_yaw * a;
    last_yaw = s->yaw;
}

// 获取欧拉角
void eskf_get_euler(ESKF_State* s, float* roll, float* pitch, float* yaw) 
{
    float qw = s->qw, qx = s->qx, qy = s->qy, qz = s->qz;
    
    *roll = atan2f(2.0f*(qw*qx + qy*qz), 1.0f - 2.0f*(qx*qx + qy*qy)) / DEG2RAD;
    *pitch = asinf(clamp(2.0f*(qw*qy - qz*qx), -1.0f, 1.0f)) / DEG2RAD;
    *yaw = atan2f(2.0f*(qw*qz + qx*qy), 1.0f - 2.0f*(qy*qy + qz*qz)) / DEG2RAD;
}

