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
 * J1/J2/J4 are blended per-joint: angle = alpha*IK + (1-alpha)*NPU.
 * J3 gets its own J3_ALPHA, weighting the handset's J3 roll (more reliable).
 * J5 is not blended; it comes from the handset's forearm roll.
 * J6 is not blended; it is mapped directly from pitch_percent.
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

        // Blend weight: 1.0 = trust IK fully (no NPU model yet)
        static constexpr float FUSION_ALPHA = 1.0f;

        // J3 dedicated weight: weights the handset J3 roll vs the IK/NPU source.
        // Higher = trust the handset more (hand J3 is the more reliable one, like J5).
        // TODO: tune after real-world validation (e.g. 0.7 - 0.9).
        static constexpr float J3_ALPHA = 0.8f;

        // pitch_percent (0~100) -> J6 angle in deg
        static constexpr float J6_MIN_DEG = -90.0f;
        static constexpr float J6_MAX_DEG =  90.0f;

        // One-pole low-pass on the fused output, ~5 Hz cutoff at ~33 Hz
        // sample rate: alpha = 1 - exp(-2*pi*fc/fs) = 1 - exp(-2*pi*5/33) ~ 0.61.
        static constexpr float LP_FILTER_ALPHA = 0.61f;

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
