#include "IKTask.h"
#include "ElegantDebug.h"
#include <cmath>
#include <string.h>

extern ElegantDebug dbg;

// Arm segment lengths (meters) — matched to NormalizeTask mapping
static constexpr float ROBOT_UPPER_M  = 0.260f;  // 260 mm
static constexpr float ROBOT_FOREARM_M = 0.190f;  // 190 mm

void IKTask::taskFunction() {
    dbg.info("IKTask started (analytical 2-segment).\n");

    for (;;) {
        auto target = _inQueue.receive(portMAX_DELAY);
        if (!target) continue;

        float ex = target->elbowCoord[0];  // mm
        float ey = target->elbowCoord[1];
        float ez = target->elbowCoord[2];
        float wx = target->wristCoord[0];
        float wy = target->wristCoord[1];
        float wz = target->wristCoord[2];

        // --- Length checks (should always pass after NormalizeTask mapping) ---
        float elLen = sqrtf(ex*ex + ey*ey + ez*ez);
        float wrLen = sqrtf(wx*wx + wy*wy + wz*wz);
        float ewLen = sqrtf((wx-ex)*(wx-ex) + (wy-ey)*(wy-ey) + (wz-ez)*(wz-ez));

        bool converged = true;
        if (fabsf(elLen - ROBOT_UPPER_M * 1000.0f) > 2.0f) converged = false;
        if (fabsf(ewLen - ROBOT_FOREARM_M * 1000.0f) > 2.0f) converged = false;

        sharedDatatype::JointOutput out = {};
        out.timestamp = target->timestamp;
        out.converged = converged;

        if (converged) {
            // --- Compute joint angles from positions ---
            // Upper arm direction (base → elbow), normalized
            float ux = ex / elLen;
            float uy = ey / elLen;
            float uz = ez / elLen;

            // Forearm direction (elbow → wrist), normalized
            float fx = (wx - ex) / ewLen;
            float fy = (wy - ey) / ewLen;
            float fz = (wz - ez) / ewLen;

            // Elbow bend angle: angle between upper arm and forearm
            float dot = ux*fx + uy*fy + uz*fz;
            if (dot >  1.0f) dot =  1.0f;
            if (dot < -1.0f) dot = -1.0f;
            float elbowBend = acosf(dot);  // radians, [0,π]

            // Upper arm spherical angles (for J1/J2/J3 decomposition later)
            // Yaw   = atan2(ux, uz) in XZ plane
            // Pitch = asin(uy) or atan2(uy, sqrt(ux²+uz²))
            float upperYaw   = atan2f(ux, -uz);    // horizontal angle
            float upperPitch = atan2f(-uy, sqrtf(ux*ux + uz*uz));  // elevation

            // Store as J1~J6 angles (J1=yaw, J2=pitch, J3=0 for now, J4=elbowBend, J5=0, J6=0)
            out.angles[0] = upperYaw;
            out.angles[1] = upperPitch;
            out.angles[2] = 0.0f;
            out.angles[3] = elbowBend;
            out.angles[4] = 0.0f;
            out.angles[5] = 0.0f;

            dbg.logWithType("IK", COLOR_GREEN,
                "J: yaw=%.1f° pitch=%.1f° bend=%.1f° | E(%.0f,%.0f,%.0f)|%.0fmm| W(%.0f,%.0f,%.0f)|%.0fmm| wr%.0f ew%.0f\n",
                upperYaw * 57.3f, upperPitch * 57.3f, elbowBend * 57.3f,
                ex, ey, ez, elLen, wx, wy, wz, wrLen, ewLen);
        } else {
            dbg.info("IK: bad lengths (el=%.0f ew=%.0f, expect %.0f/%.0f)\n",
                elLen, ewLen, ROBOT_UPPER_M*1000, ROBOT_FOREARM_M*1000);
        }

        _outQueue.sendToBack(out, 0);
    }
}
