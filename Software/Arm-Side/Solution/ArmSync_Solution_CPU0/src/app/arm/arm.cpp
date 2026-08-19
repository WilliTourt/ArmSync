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

    for (uint32_t i = 0U; i < 6U; ++i) {
        float const target = angles_deg[i];
        if (!std::isfinite(target)) {
            continue;   // invalid angle, leave this motor idle
        }
        _motors[i]->setTgtAngle(target);
        pkt.motors[i] = _motors[i]->planMove();
    }

    pkt.timestamp = 0;   // filled by MotionPlanningTask

    return pkt;
}
