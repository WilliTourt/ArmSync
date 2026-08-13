#pragma once

#include <FreeRTOS/Task.hpp>
#include "queues.h"

/**
 * @brief Task 2: Data normalization
 *
 * Get UartRecvTask::TransmitData, run FK + spatial fusion,
 * output ArmKPCoords (to IKTask) + EndEffectorData(grip) + PitchData.
 */
class NormalizeTask : public FreeRTOS::Task {
    public:
        NormalizeTask(FreeRTOS::Queue<UartRecvTask::TransmitData>      &inQueue,
                      FreeRTOS::Queue<sharedDatatype::ArmKPCoords>     &kpQueue,
                      FreeRTOS::Queue<sharedDatatype::EndEffectorData> &eeQueue,
                      FreeRTOS::Queue<sharedDatatype::EndEffectorData> &eeUIQueue,
                      FreeRTOS::Queue<sharedDatatype::PitchData>       &pitchQueue)
            : Task(tskIDLE_PRIORITY + 3, 1024, "Normalize"),
            _inQueue(inQueue),
            _kpQueue(kpQueue), _eeQueue(eeQueue),
            _eeUIQueue(eeUIQueue), _pitchQueue(pitchQueue) {}

    private:
        void taskFunction() override;
        void _armFK(const float ang_deg[6], float elbow_m[3], float wrist_m[3]);

        // ---- FK constants (human arm) ----
        static constexpr float DEG2RAD        = 3.14159265358979f / 180.0f;
        static constexpr float HUMAN_UPPER_M  = 0.35f;   // shoulder->elbow, meters
        static constexpr float HUMAN_FOREARM_M = 0.26f;   // elbow->wrist, meters

        // ---- Fusion ----
        static constexpr float FUSION_ALPHA = 1.0f;    // trust Jetson 100%

        // ---- Queues ----
        FreeRTOS::Queue<UartRecvTask::TransmitData>      &_inQueue;
        FreeRTOS::Queue<sharedDatatype::ArmKPCoords>     &_kpQueue;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData> &_eeQueue;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData> &_eeUIQueue;
        FreeRTOS::Queue<sharedDatatype::PitchData>       &_pitchQueue;
};
