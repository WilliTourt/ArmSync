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
        MotionPlanningTask(FreeRTOS::Queue<sharedDatatype::JointAngleData>   &inQueue,
                           FreeRTOS::Queue<sharedDatatype::JointAngleData>   &inReplayQueue,
                           FreeRTOS::Queue<sharedDatatype::MotionPlanPacket> &outQueue)
            : Task(tskIDLE_PRIORITY + 3, 2048, "MotionPlan"),
              _inQueue(inQueue), _replayQueue(inReplayQueue), _outQueue(outQueue),
              _arm(_j1, _j2, _j3, _j4, _j5, _j6) {}

    private:
        void taskFunction() override;

        // Joint motors: minDeg, maxDeg, reductionRatio, inverted, velocity
        static Motor _j1;   // J1 upper swing
        static Motor _j2;   // J2 upper abduction
        static Motor _j3;   // J3 upper rotation
        static Motor _j4;   // J4 forearm swing
        static Motor _j5;   // J5 forearm rotation
        static Motor _j6;   // J6 wrist swing

        Arm _arm;

        FreeRTOS::Queue<sharedDatatype::JointAngleData>  &_inQueue;      // live fused joints
        FreeRTOS::Queue<sharedDatatype::JointAngleData>  &_replayQueue;  // playback feed
        FreeRTOS::Queue<sharedDatatype::MotionPlanPacket> &_outQueue;
};
