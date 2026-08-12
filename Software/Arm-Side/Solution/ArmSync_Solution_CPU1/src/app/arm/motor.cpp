#include "motor.h"
#include "app/drivers/Emm_V5.h"
#include <cmath>



void Motor::uartCallback(uart_callback_args_t *p_args) {
    Emm_V5_UartCallback(p_args);
}



Motor::Motor(uint8_t addr,
             float minDeg,
             float maxDeg,
             float reductionRatio,
             bool inverted,
             uint16_t velocity,
             uint8_t acceleration,
             uint16_t microsteps,
             float stepAngleDeg) :
    _addr(addr), _minDeg(minDeg), _maxDeg(maxDeg),
    _cfg{reductionRatio, inverted, velocity, acceleration, microsteps, stepAngleDeg} {}

void Motor::init() {
    Emm_V5_Modify_Ctrl_Mode(_addr, true, 2);   // closed-loop mode
    enable(true);
}

void Motor::enable(bool enabled) {
    Emm_V5_En_Control(_addr, enabled, false);
}

void Motor::setTgtAngle(float deg) {
    _nextTarget = _clampAngle(deg);
}

float Motor::getMoveDuration() const {
    float const motorDegrees = std::fabs(_nextTarget - _lastTarget) * _cfg.reductionRatio;
    return _calcMoveDuration(motorDegrees,
                             static_cast<float>(_cfg.velocity),
                             _cfg.acceleration);
}

void Motor::planMove(float commonDuration) {
    if (std::isnan(_nextTarget)) {
        return;   // No target yet
    }

    float const lastTarget = _lastTarget;
    float const nextTarget = _nextTarget;
    float const motorDegrees = std::fabs(nextTarget - lastTarget) * _cfg.reductionRatio;
    if (motorDegrees < PLAN_EPSILON_DEGREES) {
        return;
    }

    float const minimumTime = _calcMoveDuration(motorDegrees,
                                                static_cast<float>(_cfg.velocity),
                                                _cfg.acceleration);
    float const duration = (commonDuration > minimumTime) ?
                            commonDuration : minimumTime;

    // brute-force search for the rpm/acc pair closest to the target duration
    float bestError = 1.0e30f;
    uint16_t bestRpm = 1U;
    uint8_t bestAcceleration = _cfg.acceleration;
    uint16_t const maxRpm = (_cfg.velocity < static_cast<uint16_t>(MAX_MOTOR_RPM)) ?
                             _cfg.velocity : static_cast<uint16_t>(MAX_MOTOR_RPM);
    uint16_t const minAcceleration = (0U == _cfg.acceleration) ? 0U : MIN_CURVE_ACCELERATION;
    uint16_t const maxAcceleration = _cfg.acceleration;
    for (uint16_t rpm = 1U; rpm <= maxRpm; ++rpm) {
        for (uint16_t acceleration = minAcceleration;
             acceleration <= maxAcceleration;
             ++acceleration) {
            float const candidateTime = _calcMoveDuration(motorDegrees,
                                                          static_cast<float>(rpm),
                                                          static_cast<uint8_t>(acceleration));
            float const error = std::fabs(candidateTime - duration);
            if (error < bestError) {
                bestError = error;
                bestRpm = rpm;
                bestAcceleration = static_cast<uint8_t>(acceleration);
            }
        }
    }

    // positive = CW (dir 0), negative = CCW (dir 1); invert if belt drive
    float const motorSigned = _cfg.inverted ? -nextTarget : nextTarget;
    uint8_t const direction = (motorSigned >= 0.0f) ? 0U : 1U;
    Emm_V5_Pos_Control(_addr,
                       direction,
                       bestRpm,
                       bestAcceleration,
                       _deg2Pulse(nextTarget),
                       true,
                       true);

    _lastTarget = nextTarget;
}

void Motor::move() {
    Emm_V5_Synchronous_motion(0U);
}

void Motor::stop() {
    Emm_V5_Stop_Now(_addr, false);
}

float Motor::getAngle() {
    float motorDegrees = 0.0f;
    if (!Emm_V5_GetPositionDegrees(_addr, &motorDegrees)) {
        return 0.0f;
    }
    // belt drive reverses direction: joint angle = motor angle / ratio, sign flipped
    float jointDegrees = motorDegrees / _cfg.reductionRatio;
    return _cfg.inverted ? -jointDegrees : jointDegrees;
}

bool Motor::isStuck() {
    Emm_V5_Feedback_t feedback;
    if (!Emm_V5_GetFeedback(_addr, &feedback) || !feedback.status_valid) {
        return false;
    }
    uint8_t const stuckMask = EMM_V5_STATUS_STALLED | EMM_V5_STATUS_STALL_PROTECTION;
    return 0U != (feedback.status_flags & stuckMask);
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

float Motor::_accel2Rpm(uint8_t acceleration) const {
    if (0U == acceleration) {
        return 0.0f;
    }

    float const stepDurationSec = (256.0f - static_cast<float>(acceleration)) *
                                  ACCELERATION_TIME_STEP_US * 1.0e-6f;
    return 1.0f / stepDurationSec;
}

float Motor::_calcMoveDuration(float motorDegrees, float maxRpm, uint8_t acceleration) const {
    if ((motorDegrees <= 0.0f) || (maxRpm <= 0.0f)) {
        return 0.0f;
    }

    maxRpm = (maxRpm > MAX_MOTOR_RPM) ? MAX_MOTOR_RPM : maxRpm;
    float const distRevolutions = motorDegrees / 360.0f;
    float const accelRpm = _accel2Rpm(acceleration);
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
