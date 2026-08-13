#include "motor.h"
#include <cmath>

Motor::Motor(float minDeg,
             float maxDeg,
             float reductionRatio,
             bool inverted,
             uint16_t velocity,
             uint8_t accel,
             uint16_t microsteps,
             float stepAngleDeg) :
    _minDeg(minDeg), _maxDeg(maxDeg),
    _cfg{reductionRatio, inverted, velocity, accel, microsteps, stepAngleDeg} {}

void Motor::setTgtAngle(float deg) {
    _nextTarget = _clampAngle(deg);
}

float Motor::getMoveDuration() const {
    float const motorDegrees = std::fabs(_nextTarget - _lastTarget) * _cfg.reductionRatio;
    return _calcMoveDuration(motorDegrees,
                             static_cast<float>(_cfg.velocity),
                             _cfg.accel);
}

sharedDatatype::MotorCommand Motor::planMove(float commonDuration) {
    sharedDatatype::MotorCommand cmd = {};

    if (std::isnan(_nextTarget)) {
        return cmd;   // No target yet
    }

    float const lastTarget = _lastTarget;
    float const nextTarget = _nextTarget;
    float const motorDegrees = std::fabs(nextTarget - lastTarget) * _cfg.reductionRatio;
    if (motorDegrees < PLAN_EPSILON_DEGREES) {
        return cmd;   // Movement too small to bother
    }

    float const minimumTime = _calcMoveDuration(motorDegrees,
                                                static_cast<float>(_cfg.velocity),
                                                _cfg.accel);
    float const duration = (commonDuration > minimumTime) ?
                            commonDuration : minimumTime;

    // Binary-search the rpm that makes the move time close to duration.
    uint16_t const maxRpm = (_cfg.velocity < static_cast<uint16_t>(MAX_MOTOR_RPM)) ?
                             _cfg.velocity : static_cast<uint16_t>(MAX_MOTOR_RPM);

    // Binary search: find smallest rpm whose move time <= duration.
    uint16_t lo = 1;
    uint16_t hi = maxRpm;
    while (lo < hi) {
        uint16_t const mid = lo + (hi - lo) / 2;
        float const midTime = _calcMoveDuration(motorDegrees,
                                                static_cast<float>(mid),
                                                _cfg.accel);
        if (midTime > duration) {
            lo = mid + 1;    // too slow -> speed up
        } else {
            hi = mid;        // fast enough -> keep it (may be the answer)
        }
    }

    // lo == hi is the wanted rpm.
    uint16_t bestRpm = lo;
    float const tLo = _calcMoveDuration(motorDegrees,
                                        static_cast<float>(bestRpm),
                                        _cfg.accel);
    float const tPrev = _calcMoveDuration(motorDegrees,
                                            static_cast<float>(bestRpm - 1),
                                            _cfg.accel);
    if (std::fabs(tPrev - duration) < std::fabs(tLo - duration)) { // if prev is closer, use prev
        bestRpm = bestRpm - 1;
    }

    // positive = CW (dir 0), negative = CCW (dir 1); invert if belt drive
    float const motorSigned = _cfg.inverted ? -nextTarget : nextTarget;
    cmd.dir = (motorSigned >= 0.0f) ? 0U : 1U;
    cmd.rpm = bestRpm;
    cmd.acc = _cfg.accel;
    cmd.pulse = _deg2Pulse(nextTarget);

    _lastTarget = nextTarget;
    return cmd;
}

float Motor::_clampAngle(float degrees) const {
    if (degrees < _minDeg) {
        return _minDeg;
    }
    if (degrees > _maxDeg) {
        return _maxDeg;
    }
    return degrees;
}

uint32_t Motor::_deg2Pulse(float jointDegrees) const {
    float const magnitude = std::fabs(jointDegrees);
    float const pulses = magnitude / _cfg.stepAngleDeg *
                         static_cast<float>(_cfg.microsteps) *
                         _cfg.reductionRatio;
    return static_cast<uint32_t>(std::lround(pulses));
}

float Motor::_accel2Rpm(uint8_t accel) const {
    if (0U == accel) {
        return 0.0f;
    }

    float const stepDurationSec = (256.0f - static_cast<float>(accel)) *
                                  ACCEL_TIME_STEP_US * 1.0e-6f;
    return 1.0f / stepDurationSec;
}

float Motor::_calcMoveDuration(float motorDegrees, float maxRpm, uint8_t accel) const {
    if ((motorDegrees <= 0.0f) || (maxRpm <= 0.0f)) {
        return 0.0f;
    }

    maxRpm = (maxRpm > MAX_MOTOR_RPM) ? MAX_MOTOR_RPM : maxRpm;
    float const distRevolutions = motorDegrees / 360.0f;
    float const accelRpm = _accel2Rpm(accel);
    if (accelRpm <= 0.0f) {
        return distRevolutions * SECONDS_PER_MINUTE / maxRpm;
    }

    float const accelDuration = maxRpm / accelRpm;
    float const accelRevolutions =
        0.5f * maxRpm * accelDuration / SECONDS_PER_MINUTE;
    if (2.0f * accelRevolutions >= distRevolutions) {
        float const peakRpm = std::sqrt(distRevolutions *
                                        SECONDS_PER_MINUTE * accelRpm);
        return 2.0f * peakRpm / accelRpm;
    }

    float const cruiseTime = (distRevolutions - 2.0f * accelRevolutions) *
                             SECONDS_PER_MINUTE / maxRpm;
    return 2.0f * accelDuration + cruiseTime;
}
