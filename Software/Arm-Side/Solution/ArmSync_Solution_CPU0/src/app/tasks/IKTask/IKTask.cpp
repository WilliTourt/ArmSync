#include "IKTask.h"
#include "ElegantDebug.h"
#include <cmath>

extern ElegantDebug dbg;

namespace {

// ===== Robot arm segment lengths (mm), matched to arm_ik_test.py =====
constexpr float L_J1_J2  = 121.0f;      // base -> J2
constexpr float L_J2_J3  = 160.985f;    // J2 -> J3
constexpr float L_J3_J4  = 103.2f;      // J3 -> J4
constexpr float L_J4_J5  = 95.0f;       // J4 -> J5
constexpr float L_J5_J6  = 90.519f;     // J5 -> J6
constexpr float L_J6_EE  = 143.247f;    // J6 -> end effector

constexpr float L_UPPER  = L_J2_J3 + L_J3_J4;  // J2 -> J4 : 264.185 mm
constexpr float L_FORE   = L_J4_J5 + L_J5_J6;  // J4 -> J6 : 185.519 mm

constexpr float R2D = 180.0f / 3.14159265358979323846f;

// Clamp helper (avoid cos() out-of-domain)
inline float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

} // namespace

void IKTask::solveIK(float ex, float ey, float ez,
                     float wx, float wy, float wz,
                     float &j1, float &j2, float &j3, float &j4,
                     float &exP, float &eyP, float &ezP,
                     float &wxP, float &wyP, float &wzP) {
    // Shift to J1 frame: J1->J2 is 121mm along Y+
    ey += L_J1_J2;
    wy += L_J1_J2;
    const float j2x = 0.0f, j2y = L_J1_J2, j2z = 0.0f;

    // ---- Spherical projection of elbow (preserve dir, scale to upper len) ----
    float uax = ex - j2x, uay = ey - j2y, uaz = ez - j2z;
    float uaLen = sqrtf(uax*uax + uay*uay + uaz*uaz);
    if (uaLen > 0.01f) {
        float s = L_UPPER / uaLen;
        exP = j2x + uax * s;
        eyP = j2y + uay * s;
        ezP = j2z + uaz * s;
    } else {
        exP = ex; eyP = ey; ezP = ez;
    }

    // ---- Forearm direction from RAW data, scale to fore length ----
    float fax = wx - (j2x + uax);
    float fay = wy - (j2y + uay);
    float faz = wz - (j2z + uaz);
    float faLen = sqrtf(fax*fax + fay*fay + faz*faz);
    if (faLen > 0.01f) {
        float s = L_FORE / faLen;
        wxP = exP + fax * s;
        wyP = eyP + fay * s;
        wzP = ezP + faz * s;
    } else {
        wxP = wx; wyP = wy; wzP = wz;
    }

    // ---- J1: yaw in XZ plane ----
    j1 = atan2f(exP - j2x, -(ezP - j2z));

    // ---- J2: law of cosines on triangle (J1-J2, J2-J4) ----
    float dJ1J4 = sqrtf(exP*exP + eyP*eyP + ezP*ezP);
    float cosJ2 = (L_J1_J2*L_J1_J2 + L_UPPER*L_UPPER - dJ1J4*dJ1J4) /
                  (2.0f * L_J1_J2 * L_UPPER);
    cosJ2 = clampf(cosJ2, -1.0f, 1.0f);
    j2 = acosf(cosJ2) - 3.14159265358979323846f / 2.0f;

    // ---- J4: law of cosines on triangle (J2-J4, J4-J6) ----
    float dJ2J6 = sqrtf((wxP-j2x)*(wxP-j2x) + (wyP-j2y)*(wyP-j2y) + (wzP-j2z)*(wzP-j2z));
    float cosJ4 = (L_UPPER*L_UPPER + L_FORE*L_FORE - dJ2J6*dJ2J6) /
                  (2.0f * L_UPPER * L_FORE);
    cosJ4 = clampf(cosJ4, -1.0f, 1.0f);
    j4 = acosf(cosJ4) - 3.14159265358979323846f;

    // ---- J3: forearm direction projected in J4 local frame ----
    float udx = (exP - j2x) / L_UPPER;
    float udy = (eyP - j2y) / L_UPPER;
    float udz = (ezP - j2z) / L_UPPER;

    float j4x_x = udz, j4x_y = 0.0f, j4x_z = -udx;
    float j4xLen = sqrtf(j4x_x*j4x_x + j4x_y*j4x_y + j4x_z*j4x_z);
    if (j4xLen > 0.001f) {
        j4x_x /= j4xLen; j4x_y /= j4xLen; j4x_z /= j4xLen;
    } else {
        j4x_x = 1.0f; j4x_y = 0.0f; j4x_z = 0.0f;
    }

    float fdx = (wxP - exP) / L_FORE;
    float fdy = (wyP - eyP) / L_FORE;
    float fdz = (wzP - ezP) / L_FORE;
    float f_x = fdx * j4x_x + fdy * j4x_y + fdz * j4x_z;
    float f_y = fdx * 0.0f + fdy * 1.0f + fdz * 0.0f;
    j3 = atan2f(-f_y, -f_x);
}

void IKTask::taskFunction() {
    dbg.info("IKTask started.\n");

    for (;;) {
        auto target = _inQueue.receive(portMAX_DELAY);
        if (!target) continue;

        float ex = target->elbowCoord[0];  // mm
        float ey = target->elbowCoord[1];
        float ez = target->elbowCoord[2];
        float wx = target->wristCoord[0];
        float wy = target->wristCoord[1];
        float wz = target->wristCoord[2];

        float j1, j2, j3, j4;
        float exP, eyP, ezP; // P - Projected
        float wxP, wyP, wzP;
        solveIK(ex, ey, ez, wx, wy, wz,
                j1, j2, j3, j4,
                exP, eyP, ezP, wxP, wyP, wzP);

        sharedDatatype::JointAngleData out = {};
        out.timestamp = target->timestamp;
        // JointAngleData.angles[] is in DEG (see queues.h)
        out.angles[0] = j1 * R2D;
        out.angles[1] = j2 * R2D;
        out.angles[2] = j3 * R2D;
        out.angles[3] = j4 * R2D;
        out.angles[4] = NAN;   // J5: no orientation sensor yet
        out.angles[5] = NAN;   // J6: mapped from pitch in FusionTask

        // Validation: projected distances should match arm segment lengths
        float dJ1J4 = sqrtf(exP*exP + eyP*eyP + ezP*ezP);
        float dJ2J6 = sqrtf((wxP - 0.0f)*(wxP - 0.0f)
                          + (wyP - L_J1_J2)*(wyP - L_J1_J2)
                          + (wzP - 0.0f)*(wzP - 0.0f));
        float ewProj = sqrtf((wxP-exP)*(wxP-exP) + (wyP-eyP)*(wyP-eyP) + (wzP-ezP)*(wzP-ezP));

        dbg.logWithType("IK", COLOR_GREEN,
            "J1=%.1f J2=%.1f J3=%.1f J4=%.1f J5=%.1f J6=%.1f | "
            "in: Eraw(%.0f,%.0f,%.0f) Wraw(%.0f,%.0f,%.0f)\n"
            "  proj: E(%.0f,%.0f,%.0f) W(%.0f,%.0f,%.0f) | "
            "len: d14=%.0f/%.0f d26=%.0f/%.0f ew=%.0f/%.0f\n",
            j1 * R2D, j2 * R2D, j3 * R2D, j4 * R2D, 0.0f, 0.0f,
            ex, ey, ez, wx, wy, wz,
            exP, eyP, ezP, wxP, wyP, wzP,
            dJ1J4, L_UPPER, dJ2J6, L_UPPER + L_FORE, ewProj, L_FORE);

        _outQueue.sendToBack(out, 0);
    }
}
