#include "arm.h"
#include "app/drivers/Emm_V5.h"
#include <cmath>

Arm::Arm(Motor &j1, Motor &j2, Motor &j3,
         Motor &j4, Motor &j5, Motor &j6) :
    _motors{&j1, &j2, &j3, &j4, &j5, &j6} {}

bool Arm::init() {
    if (!Emm_V5_Init()) {
        return false;
    }

    for (Motor *motor : _motors) {
        motor->init();
    }
    return true;
}

bool Arm::isReady() const {
    return Emm_V5_IsReady();
}

void Arm::setAngles(const float angles_deg[6]) {
    if (nullptr == angles_deg) {
        return;
    }

    // Set each joint's target, detect change, and compute per-joint min duration

    float commonDuration = 0.0f;
    bool  targetChanged = !_hasTargets;

    for (uint32_t i = 0U; i < 6U; ++i) {
        float target = angles_deg[i];
        if (!std::isfinite(target)) {
            continue;
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
        return;
    }

    // Plan all joints against the slowest joint so they arrive together

    for (uint32_t i = 0U; i < 6U; ++i) {
        _motors[i]->planMove(commonDuration);
        _lastTargets[i] = _motors[i]->getLastTarget();
    }
    _hasTargets = true;

    Motor::move();
}

bool Arm::getFeedback(float angles_deg[6]) const {
    if (nullptr == angles_deg) {
        return false;
    }

    for (uint32_t i = 0U; i < 6U; ++i) {
        angles_deg[i] = _motors[i]->getAngle();
    }
    return true;
}

bool Arm::isStuck() const {
    for (Motor *motor : _motors) {
        if (motor->isStuck()) {
            return true;
        }
    }
    return false;
}

void Arm::pollNext() {
    Motor *motor = _motors[_pollIndex];
    Emm_V5_Read_Sys_Params(motor->getAddr(), _pollStatus ? S_FLAG : S_CPOS);

    ++_pollIndex;
    if (_pollIndex >= 6U) {
        _pollIndex = 0U;
        _pollStatus = !_pollStatus;
    }
}

void Arm::stop() {
    for (Motor *motor : _motors) {
        motor->stop();
    }
}

void Arm::enable(bool enabled) {
    for (Motor *motor : _motors) {
        motor->enable(enabled);
    }
}
