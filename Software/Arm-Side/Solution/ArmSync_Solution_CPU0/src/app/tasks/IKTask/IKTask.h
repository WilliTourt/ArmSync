#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "queues.h"

class IKTask : public FreeRTOS::Task {
    public:
        IKTask(FreeRTOS::Queue<sharedDatatype::ArmKPCoords> &inQueue,
            FreeRTOS::Queue<sharedDatatype::JointAngleData>  &outQueue)
            : Task(tskIDLE_PRIORITY + 3, 2048, "IK"),
            _inQueue(inQueue), _outQueue(outQueue) {}

    private:
        void taskFunction() override;

        // Solve analytic IK from elbow/wrist positions (mm, base frame).
        // Returns J1~J4 in radians. J5/J6 left at 0 (no orientation sensor yet).
        // Outputs projected elbow/wrist for logging.
        void solveIK(float ex, float ey, float ez,
                    float wx, float wy, float wz,
                    float &j1, float &j2, float &j3, float &j4,
                    float &exP, float &eyP, float &ezP,
                    float &wxP, float &wyP, float &wzP);

        FreeRTOS::Queue<sharedDatatype::ArmKPCoords> &_inQueue;
        FreeRTOS::Queue<sharedDatatype::JointAngleData> &_outQueue;
};
