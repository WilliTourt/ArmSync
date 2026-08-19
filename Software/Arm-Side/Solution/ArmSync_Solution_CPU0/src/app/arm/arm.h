#pragma once

#include "motor.h"
#include "queues.h"

/**
 * @brief Coordinates 6 motors: converts fused joint angles (deg) directly
 *        into absolute pulse commands and packs them into a MotionPlanPacket.
 *
 * No timing/sync logic — each motor is planned independently with a fixed
 * speed/accel (see Motor::planMove).
 */
class Arm {
    public:
        Arm(Motor &j1, Motor &j2, Motor &j3,
            Motor &j4, Motor &j5, Motor &j6);

        sharedDatatype::MotionPlanPacket setAngles(const float angles_deg[6]);

    private:
        Motor *_motors[6];
};
