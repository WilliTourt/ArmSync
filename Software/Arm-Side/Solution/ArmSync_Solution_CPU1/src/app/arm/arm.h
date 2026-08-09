#pragma once

#include "motor.h"

class Arm {
    public:
        Arm(Motor& j1, Motor& j2, Motor& j3, Motor& j4, Motor& j5, Motor& j6);

        bool init();
        bool isReady() const;

        void setAngles(const float angles_deg[6]);
        void getFeedback(float angles_deg[6], float currents_ma[6]);

        void stop();
        void enable(bool en);

    private:
        Motor &_j1, &_j2, &_j3, &_j4, &_j5, &_j6;
};
