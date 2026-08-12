#include "normalizeTask.h"
#include "ElegantDebug.h"
#include <cmath>

extern ElegantDebug dbg;

static void rotX(float m[9], float rad) {
    float c = cosf(rad), s = sinf(rad);
    m[0]=1; m[1]=0; m[2]=0;
    m[3]=0; m[4]=c; m[5]=-s;
    m[6]=0; m[7]=s; m[8]=c;
}
static void rotY(float m[9], float rad) {
    float c = cosf(rad), s = sinf(rad);
    m[0]=c; m[1]=0; m[2]=s;
    m[3]=0; m[4]=1; m[5]=0;
    m[6]=-s; m[7]=0; m[8]=c;
}
static void rotZ(float m[9], float rad) {
    float c = cosf(rad), s = sinf(rad);
    m[0]=c; m[1]=-s; m[2]=0;
    m[3]=s; m[4]=c; m[5]=0;
    m[6]=0; m[7]=0; m[8]=1;
}
static void matMul(float C[9], const float A[9], const float B[9]) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            C[i*3+j] = A[i*3+0]*B[0*3+j] + A[i*3+1]*B[1*3+j] + A[i*3+2]*B[2*3+j];
}
static void matVec(float out[3], const float M[9], const float v[3]) {
    for (int i = 0; i < 3; i++)
        out[i] = M[i*3+0]*v[0] + M[i*3+1]*v[1] + M[i*3+2]*v[2];
}

/*
 * 6 controller angles (DEGREES) → elbow + wrist 3D positions (METERS)
 *   upperPitch = forward/back → X
 *   upperRoll  = arm twist    → Z
 *   upperYaw   = in/out       → Y
 */
void NormalizeTask::_armFK(const float ang_deg[6], float elbow_m[3], float wrist_m[3]) {
    float up_p = ang_deg[0] * DEG2RAD;
    float up_r = ang_deg[1] * DEG2RAD;
    float up_y = ang_deg[2] * DEG2RAD;
    float rl_p = ang_deg[3] * DEG2RAD;
    float rl_r = ang_deg[4] * DEG2RAD;
    float rl_y = ang_deg[5] * DEG2RAD;

    // R_up = Y(yaw) * X(pitch) * Z(roll)
    float Ry[9], Rx[9], Rz[9], tmp[9], R_up[9];
    rotY(Ry, up_y); rotX(Rx, up_p); rotZ(Rz, up_r);
    matMul(tmp, Ry, Rx);
    matMul(R_up, tmp, Rz);

    float bone_up[3] = {0, 0, -HUMAN_UPPER_M};
    matVec(elbow_m, R_up, bone_up);

    // R_rel = Y(rel_yaw) * X(rel_pitch) * Z(rel_roll)
    rotY(Ry, rl_y); rotX(Rx, rl_p); rotZ(Rz, rl_r);
    float R_rel[9];
    matMul(tmp, Ry, Rx);
    matMul(R_rel, tmp, Rz);

    // R_fa = R_up * R_rel
    float R_fa[9];
    matMul(R_fa, R_up, R_rel);

    float bone_fa[3] = {0, 0, -HUMAN_FOREARM_M};
    float wrist_delta[3];
    matVec(wrist_delta, R_fa, bone_fa);
    wrist_m[0] = elbow_m[0] + wrist_delta[0];
    wrist_m[1] = elbow_m[1] + wrist_delta[1];
    wrist_m[2] = elbow_m[2] + wrist_delta[2];
}


// ====== NormalizeTask ======

void NormalizeTask::taskFunction() {
    dbg.info("NormalizeTask started.\n");

    for (;;) {
        auto rx = _inQueue.receive(portMAX_DELAY);
        if (!rx) continue;

        sharedDatatype::ArmKPCoords kp = {};
        sharedDatatype::EndEffectorData ee = {};
        
        // ---- Controller FK: elbow + wrist (m → mm) ----
        {
            float el_m[3], wr_m[3];
            _armFK(rx->ctrllerData.angles, el_m, wr_m);
            kp.elbowCoord[0] = el_m[0] * 1000.0f;
            kp.elbowCoord[1] = el_m[1] * 1000.0f;
            kp.elbowCoord[2] = el_m[2] * 1000.0f;
            kp.wristCoord[0] = wr_m[0] * 1000.0f;
            kp.wristCoord[1] = wr_m[1] * 1000.0f;
            kp.wristCoord[2] = wr_m[2] * 1000.0f;

            // ---- Fuse with Jetson (if available) ----
            if (rx->jetsonData.valid) {
                for (int i = 0; i < 3; i++) {
                    float je = (float)rx->jetsonData.points[0][i];
                    float jw = (float)rx->jetsonData.points[1][i];
                    kp.elbowCoord[i] = FUSION_ALPHA * je + (1.0f - FUSION_ALPHA) * kp.elbowCoord[i];
                    kp.wristCoord[i] = FUSION_ALPHA * jw + (1.0f - FUSION_ALPHA) * kp.wristCoord[i];
                }
            }
        }

        // Send raw positions (mm) to IKTask; IKTask handles projection + IK
        kp.timestamp = rx->timestamp;
        _kpQueue.sendToBack(kp, 0);

        // ---- Gripper / pitch data (separate queue, for CPU1) ----
        {
            ee.grip_percent  = rx->ctrllerData.adc[0] / 3.3f * 100.0f;
            ee.pitch_percent = rx->ctrllerData.adc[1] / 3.3f * 100.0f;
            ee.timestamp     = rx->timestamp;
            _eeQueue.sendToBack(ee, 0);
        }
    }
}
