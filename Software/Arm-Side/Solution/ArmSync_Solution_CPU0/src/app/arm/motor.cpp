#include "motor.h"

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

sharedDatatype::MotorCommand Motor::planMove() {
    sharedDatatype::MotorCommand cmd = {};

    if (std::isnan(_nextTarget)) {
        return cmd;   // no target set yet
    }

    float const nextTarget = _nextTarget;

    // Direction from the signed angle; invert if the belt drive flips it.
    float const motorSigned = _cfg.inverted ? -nextTarget : nextTarget;
    cmd.dir = (motorSigned >= 0.0f) ? 0U : 1U;
    cmd.rpm = _cfg.velocity;   // fixed speed (Emm_V5 vel, RPM)
    cmd.acc = _cfg.accel;      // fixed accel reg
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
