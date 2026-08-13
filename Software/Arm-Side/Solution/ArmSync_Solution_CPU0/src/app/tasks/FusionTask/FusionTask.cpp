#include "FusionTask.h"
#include "ElegantDebug.h"
#include <cmath>

extern ElegantDebug dbg;

float FusionTask::_mapPitchToJ6(float pitch_percent) const {
    if (pitch_percent < 0.0f) pitch_percent = 0.0f;
    if (pitch_percent > 100.0f) pitch_percent = 100.0f;

    return J6_MIN_DEG + (pitch_percent / 100.0f) * (J6_MAX_DEG - J6_MIN_DEG);
}

void FusionTask::taskFunction() {
    dbg.info("FusionTask started.\n");

    float latestPitch = 50.0f;   // default: J6 center
    bool  pitchValid = false;

    for (;;) {
        // Always drain pitch (low latency)
        auto pitch = _pitchQueue.receive(0);
        if (pitch) {
            latestPitch = pitch->pitch_percent;
            pitchValid = true;
        }

        // Block on IK data (main fusion trigger)
        auto ik = _ikQueue.receive(portMAX_DELAY);
        if (!ik) continue;

        // Try to grab latest NPU data (non-blocking)
        auto npu = _npuQueue.receive(0);

        sharedDatatype::JointAngleData out = {};
        out.timestamp = ik->timestamp;

        // J1~J5: complementary blend of IK and NPU
        for (int i = 0; i < 5; i++) {
            if (!npu) {
                // no NPU data yet -> pure IK
                out.angles[i] = ik->angles[i];
            } else {
                out.angles[i] = FUSION_ALPHA * ik->angles[i] +
                               (1.0f - FUSION_ALPHA) * npu->angles[i];
            }
        }

        // J6: mapped from pitch (not blended)
        out.angles[5] = pitchValid ? _mapPitchToJ6(latestPitch) : 0.0f;

        _outQueue.sendToBack(out, 0);

        dbg.logWithType("FUSION", COLOR_MAGENTA,
            "J1=%.1f J2=%.1f J3=%.1f J4=%.1f J5=%.1f J6=%.1f (pitch=%.0f%%)\n",
            out.angles[0], out.angles[1], out.angles[2],
            out.angles[3], out.angles[4], out.angles[5], latestPitch);
    }
}
