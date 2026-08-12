#pragma once

#include "hal_data.h"
#include <stdint.h>
#include <cmath>

class Motor {
    public:
        struct Config {
            float reductionRatio;
            bool inverted;      // true = belt drive reverses the output direction
            uint16_t velocity;
            uint8_t acceleration;
            uint16_t microsteps;
            float stepAngleDeg;
        };

        Motor(uint8_t addr,
            float minDeg,
            float maxDeg,
            float reductionRatio,
            bool inverted = false,
            uint16_t velocity = 100U,
            uint8_t acceleration = 150U,
            uint16_t microsteps = 16U,
            float stepAngleDeg = 1.8f);

        void init();
        void enable(bool enabled);

        // 1. Set target angle
        void setTgtAngle(float deg);

        // 2. Get minimum move duration for this target
        float getMoveDuration() const;

        // 3. Plan this motor's move to finish within the given common duration
        void planMove(float commonDuration);

        // 4. Let all motors to move AT THE SAME TIME
        static void move();

        void stop();

        // Feedback related
        float getAngle();
        bool isStuck();

        inline uint8_t getAddr() { return _addr; }
        inline float getReductionRatio() { return _cfg.reductionRatio; }
        inline float getLastTarget() { return _lastTarget; }

        static void uartCallback(uart_callback_args_t *p_args);

    private:
        float _clampAngle(float degrees) const;

        uint32_t _deg2Pulse(float jointDegrees) const;
        float _accel2Rpm(uint8_t acceleration) const;
        float _calcMoveDuration(float motorDegrees, float maxRpm, uint8_t acceleration) const;

        static constexpr float SECONDS_PER_MINUTE = 60.0f;
        static constexpr float ACCELERATION_TIME_STEP_US = 50.0f;
        static constexpr float MAX_MOTOR_RPM = 5000.0f;
        static constexpr uint8_t MIN_CURVE_ACCELERATION = 1U;
        static constexpr float PLAN_EPSILON_DEGREES = 0.0001f;

        uint8_t _addr;
        float   _minDeg, _maxDeg;

        float _lastTarget = 0.0f;   // start angle
        float _nextTarget = NAN;    // target angle

        Config  _cfg;
};
