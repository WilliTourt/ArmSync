#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "queues.h"

/**
 * @brief Complementary fusion of IK and NPU joint angles.
 *
 * Inputs:
 *   - ikJointQueue  : JointAngleData from IKTask (J1~J6)
 *   - npuJointQueue : JointAngleData from NPUTask (J1~J6, no model yet)
 *   - pitchDataQueue: pitch_percent from NormalizeTask (maps to J6)
 *
 * Output:
 *   - fusedJointQueue : JointAngleData -> MotionPlanningTask
 *
 * J1~J5 are blended per-joint: angle = alpha*IK + (1-alpha)*NPU.
 * J6 is not blended; it is mapped directly from pitch_percent.
 */
class FusionTask : public FreeRTOS::Task {
    public:
        FusionTask(FreeRTOS::Queue<sharedDatatype::JointAngleData> &ikQueue,
                   FreeRTOS::Queue<sharedDatatype::JointAngleData> &npuQueue,
                   FreeRTOS::Queue<sharedDatatype::PitchData>      &pitchQueue,
                   FreeRTOS::Queue<sharedDatatype::JointAngleData> &outQueue)
            : Task(tskIDLE_PRIORITY + 3, 1024, "Fusion"),
              _ikQueue(ikQueue), _npuQueue(npuQueue),
              _pitchQueue(pitchQueue), _outQueue(outQueue) {}

    private:
        void taskFunction() override;

        // Blend weight: 1.0 = trust IK fully (no NPU model yet)
        static constexpr float FUSION_ALPHA = 1.0f;

        // pitch_percent (0~100) -> J6 angle in deg
        static constexpr float J6_MIN_DEG = -90.0f;
        static constexpr float J6_MAX_DEG =  90.0f;

        float _mapPitchToJ6(float pitch_percent) const;

        FreeRTOS::Queue<sharedDatatype::JointAngleData> &_ikQueue;
        FreeRTOS::Queue<sharedDatatype::JointAngleData> &_npuQueue;
        FreeRTOS::Queue<sharedDatatype::PitchData>      &_pitchQueue;
        FreeRTOS::Queue<sharedDatatype::JointAngleData> &_outQueue;
};
