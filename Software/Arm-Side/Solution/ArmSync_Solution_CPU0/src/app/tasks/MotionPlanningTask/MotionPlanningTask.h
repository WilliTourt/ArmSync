#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "queues.h"
#include "arm.h"

/**
 * @brief Holds the Arm (6 Motor planners) and turns fused joint angles
 *        into a MotionPlanPacket for CPUCommTask / M33.
 */
class MotionPlanningTask : public FreeRTOS::Task {
    public:
        MotionPlanningTask(FreeRTOS::Queue<sharedDatatype::JointAngleData>  &inQueue,
                           FreeRTOS::Queue<sharedDatatype::MotionPlanPacket> &outQueue)
            : Task(tskIDLE_PRIORITY + 3, 2048, "MotionPlan"),
              _inQueue(inQueue), _outQueue(outQueue),
              _arm(g_j1, g_j2, g_j3, g_j4, g_j5, g_j6) {}

    private:
        void taskFunction() override;

        // Joint motors: minDeg, maxDeg, reductionRatio, inverted, velocity
        static Motor g_j1;   // J1 upper swing
        static Motor g_j2;   // J2 upper abduction
        static Motor g_j3;   // J3 upper rotation
        static Motor g_j4;   // J4 forearm swing
        static Motor g_j5;   // J5 forearm rotation
        static Motor g_j6;   // J6 wrist swing

        Arm _arm;

        FreeRTOS::Queue<sharedDatatype::JointAngleData>  &_inQueue;
        FreeRTOS::Queue<sharedDatatype::MotionPlanPacket> &_outQueue;
};
