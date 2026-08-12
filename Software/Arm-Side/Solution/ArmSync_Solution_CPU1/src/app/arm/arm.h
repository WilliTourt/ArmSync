#pragma once

#include "motor.h"

class Arm {
    public:
        Arm(Motor &j1, Motor &j2, Motor &j3,
            Motor &j4, Motor &j5, Motor &j6);

        bool init();
        bool isReady() const;

        void setAngles(const float angles_deg[6]);
        bool getFeedback(float angles_deg[6]) const;
        bool isStuck() const;

        // CALL IN AGT ISR: query position or status feedback of the next motor
        void pollNext();

        void stop();
        void enable(bool enabled);

    private:
        Motor *_motors[6];

        uint8_t _pollIndex = 0U;
        bool    _pollStatus = false;   // false = position (S_CPOS), true = status (S_FLAG)

        float _lastTargets[6] = {0.0f};
        bool  _hasTargets = false;
};
