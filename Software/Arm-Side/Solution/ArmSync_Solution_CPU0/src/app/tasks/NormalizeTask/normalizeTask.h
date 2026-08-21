#pragma once

#include <FreeRTOS/Task.hpp>
#include "queues.h"

class NormalizeTask : public FreeRTOS::Task {
    public:
        NormalizeTask(FreeRTOS::Queue<UartRecvTask::TransmitData>      &inQueue,
                      FreeRTOS::Queue<sharedDatatype::ArmKPCoords>     &kpQueue,
                      FreeRTOS::Queue<sharedDatatype::HandJointData>   &handQueue,
                      FreeRTOS::Queue<sharedDatatype::EndEffectorData> &eeQueue,
                      FreeRTOS::Queue<sharedDatatype::EndEffectorData> &eeUIQueue,
                      FreeRTOS::Queue<sharedDatatype::EndEffectorData> &gripQueue)
            : Task(tskIDLE_PRIORITY + 3, 2048, "Normalize"),
            _inQueue(inQueue), _kpQueue(kpQueue), _handQueue(handQueue),
            _eeQueue(eeQueue), _eeUIQueue(eeUIQueue), _gripQueue(gripQueue) {}

    private:
        void taskFunction() override;
        void _updateAlphaFromWz(float wz); // Update hand/Jetson blend alpha from the (scaled) hand wrist Z (mm).

        static constexpr float HUMAN_UPPER_M   = 0.35f;   // shoulder -> elbow
        static constexpr float HUMAN_FOREARM_M = 0.26f;   // elbow -> wrist

        // blend alpha vs. wrist Z (mm, scaled coords). Wrist low (very negative Z)
        // => trust Jetson more; wrist raised (Z rises toward -300) => trust hand more.
        static constexpr float ALPHA_WZ_TRUST_HIGH = 0.72f;   // alpha when wrist at its lowest
        static constexpr float ALPHA_WZ_TRUST_LOW  = 0.06f;  // alpha when wrist raised (>= ALPHA_WZ_RAISED)
        static constexpr float ALPHA_WZ_LOWEST   = -600.0f;  // Z below this => max Jetson trust
        static constexpr float ALPHA_WZ_RAISED   = -300.0f;  // Z above this => min Jetson trust
        float _alpha = 0.06f;   // hand/Jetson blend alpha

        // One-pole low-pass on the elbow/wrist X (Jetson lateral jitter). Y/Z stay raw.
        static constexpr float LP_X_ALPHA = 0.18f;   // 0..1, smaller = smoother
        float _lpElX = 0.0f;   // filtered elbow X (mm)
        float _lpWrX = 0.0f;   // filtered wrist X (mm)
        bool  _lpXInit = false; // seed filter with first frame

        FreeRTOS::Queue<UartRecvTask::TransmitData>      &_inQueue;
        FreeRTOS::Queue<sharedDatatype::ArmKPCoords>     &_kpQueue;
        FreeRTOS::Queue<sharedDatatype::HandJointData>   &_handQueue;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData> &_eeQueue;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData> &_eeUIQueue;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData> &_gripQueue;  // -> RecPlayTask (grip recorded for playback)
};
