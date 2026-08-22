#include "normalizeTask.h"
#include "ElegantDebug.h"
#include <cmath>

#include "UITask.h"

extern ElegantDebug dbg;

void NormalizeTask::_updateAlphaFromJ2(float j2deg) {

    if (j2deg <= 0.0f) {
        _alpha = ALPHA_J2_LOW;
    } else if (j2deg >= ALPHA_J2_HIGH_DEG) {
        _alpha = ALPHA_J2_HIGH;
    } else {
        _alpha = ALPHA_J2_LOW
               - (ALPHA_J2_LOW - ALPHA_J2_HIGH) * (j2deg / ALPHA_J2_HIGH_DEG);
    }
}

// ====== NormalizeTask ======

void NormalizeTask::taskFunction() {
    dbg.ok("NormalizeTask started.\n");

    for (;;) {
        auto rx = _inQueue.receive(portMAX_DELAY);
        if (!rx) continue;

        sharedDatatype::ArmKPCoords kp = {};
        sharedDatatype::HandJointData hd = {};
        sharedDatatype::EndEffectorData ee = {};

        // 1. Hand unit-vectors -> elbow/wrist coords (mm)
        // elbow = ev * UPPER_LEN ; wrist = elbow + wv * FORE_LEN
        const float *ev = rx->ctrllerData.elbowVec;   // upper-arm unit vec
        const float *wv = rx->ctrllerData.wristVec;   // forearm unit vec
        float el[3] = {
            ev[0] * (HUMAN_UPPER_M * 1000.0f),
            ev[1] * (HUMAN_UPPER_M * 1000.0f),
            ev[2] * (HUMAN_UPPER_M * 1000.0f)
        };
        float wr[3] = {
            el[0] + wv[0] * (HUMAN_FOREARM_M * 1000.0f),
            el[1] + wv[1] * (HUMAN_FOREARM_M * 1000.0f),
            el[2] + wv[2] * (HUMAN_FOREARM_M * 1000.0f)
        };

        // No shoulder offset, IKTask applies the J1->J2 121mm shift

        // 2. Fuse with Jetson keypoints (J1~J4 alpha blend)

        // Pull the latest fused J2 from FusionTask (index 0, non-blocking).
        // One-frame delayed: last J2 decides the alpha for this frame.
        uint32_t j2Notif = 0;
        if (xTaskNotifyWaitIndexed(0, 0, 0xFFFFFFFF, &j2Notif, 0) == pdTRUE) {
            float j2deg = (float)((int32_t)j2Notif) / 100.0f;   // signed fixed-point
            _updateAlphaFromJ2(j2deg);
            dbg.logWithType("NORM ALPHA", COLOR_MAGENTA, "Trust jetson %.2f%\n", _alpha);
        }

        if (rx->jetsonData.valid) {
            for (int i = 0; i < 3; i++) {
                float je = (float)rx->jetsonData.points[0][i];
                float jw = (float)rx->jetsonData.points[1][i];
                el[i] = _alpha * je + (1.0f - _alpha) * el[i];
                wr[i] = _alpha * jw + (1.0f - _alpha) * wr[i];
            }
        }

        kp.elbowCoord[0] = el[0];
        kp.elbowCoord[1] = el[1];
        kp.elbowCoord[2] = el[2];
        kp.wristCoord[0] = wr[0];
        kp.wristCoord[1] = wr[1];
        kp.wristCoord[2] = wr[2];
        kp.timestamp = rx->timestamp;

        // 3. J3 & J5 arm rotation from controller, directly send to FusionTask
        hd.j3deg         = rx->ctrllerData.upperarmPitch;
        hd.j5deg         = rx->ctrllerData.forearmPitch;
        hd.pitch_percent = rx->ctrllerData.pitchPercent;
        hd.timestamp     = rx->timestamp;

        // 4. Outputs
        _kpQueue.sendToBack(kp, 0);
        _handQueue.sendToBack(hd, 0);

        // Grip (UI + control)
        ee.grip_percent = rx->ctrllerData.gripPercent;
        ee.timestamp    = rx->timestamp;
        _eeQueue.sendToBack(ee, 0);
        _eeUIQueue.sendToBack(ee, 0);

        // DEBUG:
        // dbg.logWithType("NORMALIZED INPUT", COLOR_MAGENTA,
            // "Elbow(%d,%d,%d), Wrist(%d,%d,%d)\n",
            // (int)el[0], (int)el[1], (int)el[2],
            // (int)wr[0], (int)wr[1], (int)wr[2]);
    }
}
