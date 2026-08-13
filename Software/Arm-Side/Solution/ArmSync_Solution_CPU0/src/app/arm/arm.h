#pragma once

#include "motor.h"
#include "queues.h"

/**
 * @brief Coordinates 6 motors so they move together.
 *
 * setAngles() sets each motor's target, finds the slowest motor's duration,
 * plans every motor against that common duration, and aggregates the 6
 * resulting commands into a MotionPlanPacket for M33.
 */
class Arm {
    public:
        Arm(Motor &j1, Motor &j2, Motor &j3,
            Motor &j4, Motor &j5, Motor &j6);

        sharedDatatype::MotionPlanPacket setAngles(const float angles_deg[6]);

    private:
        Motor *_motors[6];

        float _lastTargets[6] = {0.0f};
        bool  _hasTargets = false;
};
