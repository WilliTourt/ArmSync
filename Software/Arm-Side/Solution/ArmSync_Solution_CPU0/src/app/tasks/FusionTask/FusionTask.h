#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "queues.h"

/**
 * @brief Complementary fusion of IK and hand joint angles, with a folded-in
 *        Ethos-U NPU smoothing filter on J1..J5 (EdgeCortix 9x5 model -> deltas).
 *
 * Inputs:
 *   - ikJointQueue  : JointAngleData from IKTask       (J1~J4)
 *   - handJointQueue: HandJointData from NormalizeTask (J3, J5, pitch->J6)
 *
 * Output:
 *   - fusedJointQueue : JointAngleData -> MotionPlanningTask
 *
 * Per-joint source:
 *   J1/J2/J4 : IK only
 *   J3       : IK + hand
 *   J5       : hand only (IK can't sense roll)
 *   J6       : hand only (pitch_percent -> angle)
 *
 * Smoothing (double): 1) folded-in NPU filter adds deltas to J1..J5,
 *                      2) one-pole low-pass on all six.
 */
class FusionTask : public FreeRTOS::Task {
    public:
        FusionTask(FreeRTOS::Queue<sharedDatatype::JointAngleData> &ikQueue,
                   FreeRTOS::Queue<sharedDatatype::HandJointData>   &handQueue,
                   FreeRTOS::Queue<sharedDatatype::JointAngleData> &outQueue,
                   FreeRTOS::Queue<sharedDatatype::JointAngleData> &recQueue)
            : Task(tskIDLE_PRIORITY + 3, 1024, "Fusion"),
              _ikQueue(ikQueue),
              _handQueue(handQueue), _outQueue(outQueue), _recQueue(recQueue) {}

        void setUIHandle(TaskHandle_t handle);
        void setRecHandle(TaskHandle_t handle);   // RecPlayTask (for REC_DONE notify)

    private:
        void taskFunction() override;

        static constexpr float J3_IK_ALPHA   = 0.8;
        static constexpr float J3_HAND_ALPHA = 1.0 - J3_IK_ALPHA;

        // One-pole low-pass on the fused output, ~5 Hz cutoff at ~33 Hz
        // sample rate: alpha = 1 - exp(-2*pi*fc/fs) = 1 - exp(-2*pi*5/33) ~ 0.61.
        static constexpr float LP_FILTER_ALPHA = 0.25f;   // strong smoothing: Jetson input jitters

        // pitch_percent (0~100) -> J6 angle in deg
        static constexpr float J6_MIN_DEG = -90.0f;
        static constexpr float J6_MAX_DEG =  90.0f;

        void _applyLowPass(sharedDatatype::JointAngleData &out);
        bool _applyNpuFilter(sharedDatatype::JointAngleData &out);  // 9x5 NPU filter -> deltas

        // NPU filter (EdgeCortix model, 9x5 window J1..J5 -> 5 deltas).
        // Folded directly into FusionTask (no separate NPUTask).
        fsp_err_t _initNPU();   // RM_ETHOSU_Open + PMU setup (once, before loop)

        TaskHandle_t _uiHandle = nullptr;
        TaskHandle_t _recHandle = nullptr;
        bool         _recording = false;   // true while UI says REC

        // Low-pass filter state (one value per joint, last filtered frame).
        float _filtered[6] = {0.0f};
        bool  _filterInit   = false;   // first frame seeds the filter

        // NPU filter model state: sliding window of the last 9 frames of
        // J1..J5 (45 floats, [frame][joint]), plus the number of valid frames.
        static constexpr int NPU_WIN_FRAMES = 9;
        static constexpr int NPU_WIN_JOINTS  = 5;
        float _npuWin[NPU_WIN_FRAMES * NPU_WIN_JOINTS] = {0.0f};
        int   _npuFrames = 0;
        bool  _npuReady  = false;   // true once _initNPU() succeeded

        // NPU inference frequency estimation (sliding window of ticks).
        static constexpr int NPU_FREQ_WINDOW = 5;
        uint32_t _npuTicks[NPU_FREQ_WINDOW] = {0};
        int   _npuTickIdx  = 0;
        bool  _npuTickFull = false;

        float _mapPitchToJ6(float pitch_percent) const;

        FreeRTOS::Queue<sharedDatatype::JointAngleData> &_ikQueue;
        FreeRTOS::Queue<sharedDatatype::HandJointData>   &_handQueue;  // J3/J5 roll + pitch
        FreeRTOS::Queue<sharedDatatype::JointAngleData> &_outQueue;
        FreeRTOS::Queue<sharedDatatype::JointAngleData> &_recQueue;  // record tap
};
