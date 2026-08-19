#pragma once

#include <stdint.h>
#include <cmath>
#include "queues.h"

/**
 * @brief Simplest possible single-motor planner (no accel curve / no sync).
 *
 * Converts an absolute target joint angle (deg) directly into an absolute
 * pulse count for the Emm_V5 closed-loop servo, plus a fixed speed and
 * direction. M33 just forwards these to the motor.
 *
 *   pulses = |angle| * microsteps * reductionRatio / stepAngleDeg
 *
 * Direction: positive -> CW (dir 0), negative -> CCW (dir 1); inverted
 * (belt-drive reversal) flips the sign fed to dir.
 */
class Motor {
    public:
        struct Config {
            float reductionRatio;
            bool inverted;      // true = belt drive reverses output direction
            uint16_t velocity;  // fixed speed in RPM (Emm_V5 "vel")
            uint8_t accel;      // fixed accel reg (0~255, Emm_V5 "acc")
            uint16_t microsteps;
            float stepAngleDeg;
        };

        Motor(float minDeg,
              float maxDeg,
              float reductionRatio,
              bool inverted = false,
              uint16_t velocity = 100U,
              uint8_t accel = 150U,
              uint16_t microsteps = 16U,
              float stepAngleDeg = 1.8f);

        // Set the target angle (clamped to [minDeg, maxDeg]).
        void setTgtAngle(float deg);

        // Turn the pending target into a MotorCommand (dir/rpm/acc/pulse).
        sharedDatatype::MotorCommand planMove();

        float getLastTarget() const { return _lastTarget; }

    private:
        float _clampAngle(float degrees) const;
        uint32_t _deg2Pulse(float jointDegrees) const;

        float _minDeg, _maxDeg;

        float _lastTarget = 0.0f;   // last angle fed to the motor
        float _nextTarget = NAN;    // target angle (NAN = not set)

        Config _cfg;
};
