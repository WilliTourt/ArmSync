#pragma once

#include <stdint.h>
#include <cmath>
#include "queues.h"

/**
 * @brief Pure motion planning for a single motor (no hardware access).
 *
 * Motor computes the dir/rpm/acc/pulse command needed to move from the
 * last target to the new target within a given duration. The resulting
 * command is returned as sharedDatatype::MotorCommand so the CPU0 side
 * can aggregate and send it to M33 via IPC. M33 (the driver side) owns
 * the actual Emm_V5 hardware and knows each motor's UART address.
 */
class Motor {
    public:
        struct Config {
            float reductionRatio;
            bool inverted;      // true = belt drive reverses the output direction
            uint16_t velocity;
            uint8_t accel;
            uint16_t microsteps;
            float stepAngleDeg;
        };

        Motor(float minDeg,
              float maxDeg,
              float reductionRatio,
              bool inverted = false,
              uint16_t velocity = 100U,
              uint8_t accel = 182U,
              uint16_t microsteps = 16U,
              float stepAngleDeg = 1.8f);

        // 1. Set target angle (clamped to limits)
        void setTgtAngle(float deg);

        // 2. Minimum move duration for the pending target
        float getMoveDuration() const;

        // 3. Plan the move to finish within commonDuration; returns the command
        sharedDatatype::MotorCommand planMove(float commonDuration);

        float getLastTarget() const { return _lastTarget; }

    private:
        float _clampAngle(float degrees) const;

        uint32_t _deg2Pulse(float jointDegrees) const;
        float _accel2Rpm(uint8_t accel) const;
        float _calcMoveDuration(float motorDegrees, float maxRpm, uint8_t accel) const;

        static constexpr float SECONDS_PER_MINUTE = 60.0f;
        static constexpr float ACCEL_TIME_STEP_US = 50.0f;
        static constexpr float MAX_MOTOR_RPM = 5000.0f;
        static constexpr uint8_t MIN_CURVE_ACCEL = 1U;
        static constexpr float PLAN_EPSILON_DEGREES = 0.0001f;

        float _minDeg, _maxDeg;

        float _lastTarget = 0.0f;   // start angle
        float _nextTarget = NAN;    // target angle (NAN = not set)

        Config _cfg;
};
