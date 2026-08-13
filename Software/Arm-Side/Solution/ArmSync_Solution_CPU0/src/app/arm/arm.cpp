#include "arm.h"
#include <cmath>

Arm::Arm(Motor &j1, Motor &j2, Motor &j3,
         Motor &j4, Motor &j5, Motor &j6) :
    _motors{&j1, &j2, &j3, &j4, &j5, &j6} {}

sharedDatatype::MotionPlanPacket Arm::setAngles(const float angles_deg[6]) {
    sharedDatatype::MotionPlanPacket pkt = {};

    if (nullptr == angles_deg) {
        return pkt;
    }

    // -- 1. Set each motor's target, detect change, compute min duration --
    float commonDuration = 0.0f;
    bool  targetChanged = !_hasTargets;

    for (uint32_t i = 0U; i < 6U; ++i) {
        float target = angles_deg[i];
        if (!std::isfinite(target)) {
            continue;   // invalid angle, skip this joint
        }

        _motors[i]->setTgtAngle(target);

        if (std::fabs(target - _lastTargets[i]) >= 0.01f) {
            targetChanged = true;
        }

        float const duration = _motors[i]->getMoveDuration();
        if (duration > commonDuration) {
            commonDuration = duration;
        }
    }

    if (!targetChanged) {
        return pkt;   // no motion needed
    }

    // -- 2. Plan all joints against the slowest one so they arrive together --
    for (uint32_t i = 0U; i < 6U; ++i) {
        pkt.motors[i] = _motors[i]->planMove(commonDuration);
        _lastTargets[i] = _motors[i]->getLastTarget();
    }
    _hasTargets = true;
    pkt.timestamp = 0;   // filled by MotionPlanningTask

    return pkt;
}
