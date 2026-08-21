#include "normalizeTask.h"
#include "ElegantDebug.h"
#include <cmath>

#include "UITask.h"

extern ElegantDebug dbg;

void NormalizeTask::_updateAlphaFromWz(float wz) {
    // wz is the (scaled) wrist Z in mm. Wrist low (very negative) => trust
    // Jetson more; wrist raised toward ALPHA_WZ_RAISED => trust hand more.
    if (wz <= ALPHA_WZ_LOWEST) {
        _alpha = ALPHA_WZ_TRUST_HIGH;            // wrist at its lowest: max Jetson
    } else if (wz >= ALPHA_WZ_RAISED) {
        _alpha = ALPHA_WZ_TRUST_LOW;             // wrist raised: trust hand
    } else {
        // linear blend between the two Z thresholds
        float t = (wz - ALPHA_WZ_LOWEST) / (ALPHA_WZ_RAISED - ALPHA_WZ_LOWEST);
        _alpha = ALPHA_WZ_TRUST_HIGH
               - (ALPHA_WZ_TRUST_HIGH - ALPHA_WZ_TRUST_LOW) * t;
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

        // 2. Fuse with Jetson keypoints (J1~J4 alpha blend).
        // Decide this frame's blend alpha from the (scaled) hand wrist Z:
        // wrist low (very negative Z) => trust Jetson more; raised => trust hand.
        _updateAlphaFromWz(wr[2]);

        if (rx->jetsonData.valid) {
            for (int i = 0; i < 3; i++) {
                float je = (float)rx->jetsonData.points[0][i];
                float jw = (float)rx->jetsonData.points[1][i];
                el[i] = _alpha * je + (1.0f - _alpha) * el[i];
                wr[i] = _alpha * jw + (1.0f - _alpha) * wr[i];
            }
        }

        // X-axis low-pass on elbow/wrist (Jetson lateral jitter). Y/Z stay raw.
        if (!_lpXInit) {
            _lpElX = el[0];
            _lpWrX = wr[0];
            _lpXInit = true;
        } else {
            _lpElX = LP_X_ALPHA * el[0] + (1.0f - LP_X_ALPHA) * _lpElX;
            _lpWrX = LP_X_ALPHA * wr[0] + (1.0f - LP_X_ALPHA) * _lpWrX;
        }
        el[0] = _lpElX;   // write filtered X back
        wr[0] = _lpWrX;

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

        // Grip (UI + control + record)
        ee.grip_percent = rx->ctrllerData.gripPercent;
        ee.timestamp    = rx->timestamp;
        _eeQueue.sendToBack(ee, 0);        // -> CPUCommTask (live grip to M33)
        _eeUIQueue.sendToBack(ee, 0);      // -> UITask (display)
        _gripQueue.sendToBack(ee, 0);      // -> RecPlayTask (record grip for playback)

        // DEBUG:
        dbg.logWithType("NORMALIZED INPUT", COLOR_MAGENTA,
            "Elbow(%d,%d,%d), Wrist(%d,%d,%d)\n",
            (int)el[0], (int)el[1], (int)el[2],
            (int)wr[0], (int)wr[1], (int)wr[2]);
    }
}
