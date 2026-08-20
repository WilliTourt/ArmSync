#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "queues.h"

/**
 * @brief Complementary fusion of IK, hand and NPU joint angles.
 *
 * Inputs:
 *   - ikJointQueue  : JointAngleData from IKTask       (J1~J4)
 *   - npuJointQueue : JointAngleData from NPUTask      (J3, J5)
 *   - handJointQueue: HandJointData from NormalizeTask (J3, J5, pitch->J6)
 *
 * Output:
 *   - fusedJointQueue : JointAngleData -> MotionPlanningTask
 *
 * Per-joint source:
 *   J1/J2/J4 : IK only
 *   J3       : 3-way  = J3_IK_ALPHA*IK + J3_HAND_ALPHA*hand + J3_NPU_ALPHA*NPU
 *   J5       : 2-way  = J5_HAND_ALPHA*hand + J5_NPU_ALPHA*NPU  (IK can't sense roll)
 *   J6       : hand only (pitch_percent -> angle)
 */
class FusionTask : public FreeRTOS::Task {
    public:
        FusionTask(FreeRTOS::Queue<sharedDatatype::JointAngleData> &ikQueue,
                   FreeRTOS::Queue<sharedDatatype::JointAngleData> &npuQueue,
                   FreeRTOS::Queue<sharedDatatype::HandJointData>   &handQueue,
                   FreeRTOS::Queue<sharedDatatype::JointAngleData> &outQueue,
                   FreeRTOS::Queue<sharedDatatype::JointAngleData> &recQueue)
            : Task(tskIDLE_PRIORITY + 3, 1024, "Fusion"),
              _ikQueue(ikQueue), _npuQueue(npuQueue),
              _handQueue(handQueue), _outQueue(outQueue), _recQueue(recQueue) {}

        void setUIHandle(TaskHandle_t handle);
        void setRecHandle(TaskHandle_t handle);   // RecPlayTask (for REC_DONE notify)
        void setNormalizeHandle(TaskHandle_t handle);   // NormalizeTask (J2 -> alpha)

    private:
        void taskFunction() override;

        static constexpr float J3_IK_ALPHA   = 1;
        static constexpr float J3_HAND_ALPHA = 0.0;
        static constexpr float J3_NPU_ALPHA  = 0.0;

        static constexpr float J5_HAND_ALPHA = 1;
        static constexpr float J5_NPU_ALPHA  = 0;

        // One-pole low-pass on the fused output, ~5 Hz cutoff at ~33 Hz
        // sample rate: alpha = 1 - exp(-2*pi*fc/fs) = 1 - exp(-2*pi*5/33) ~ 0.61.
        static constexpr float LP_FILTER_ALPHA = 0.25f;   // strong smoothing: Jetson input jitters

        // pitch_percent (0~100) -> J6 angle in deg
        static constexpr float J6_MIN_DEG = -90.0f;
        static constexpr float J6_MAX_DEG =  90.0f;

        void _applyLowPass(sharedDatatype::JointAngleData &out);

        TaskHandle_t _uiHandle = nullptr;
        TaskHandle_t _recHandle = nullptr;
        TaskHandle_t _normalizeHandle = nullptr;   // receives J2 (-> NormalizeTask alpha)
        bool         _recording = false;   // true while UI says REC

        // Low-pass filter state (one value per joint, last filtered frame).
        float _filtered[6] = {0.0f};
        bool  _filterInit   = false;   // first frame seeds the filter

        float _mapPitchToJ6(float pitch_percent) const;

        FreeRTOS::Queue<sharedDatatype::JointAngleData> &_ikQueue;
        FreeRTOS::Queue<sharedDatatype::JointAngleData> &_npuQueue;
        FreeRTOS::Queue<sharedDatatype::HandJointData>   &_handQueue;  // J3/J5 roll + pitch
        FreeRTOS::Queue<sharedDatatype::JointAngleData> &_outQueue;
        FreeRTOS::Queue<sharedDatatype::JointAngleData> &_recQueue;  // record tap
};
