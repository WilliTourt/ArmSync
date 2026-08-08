#pragma once

#include <FreeRTOS/Task.hpp>
#include "queues.h"

/**
 * @brief Task 2: Time synchronization and data normalization
 * 
 * Get UartRecvTask::TransmitData from UartRecvTask,
 * runs FK + Jetson fusion, outputs ArmKPCoords + EndEffectorData.
 */
class NormalizeTask : public FreeRTOS::Task {
    public:
        NormalizeTask(FreeRTOS::Queue<UartRecvTask::TransmitData>      &inQueue,
                      FreeRTOS::Queue<sharedDatatype::ArmKPCoords>     &kpQueue,
                      FreeRTOS::Queue<sharedDatatype::EndEffectorData> &eeQueue)
            : Task(tskIDLE_PRIORITY + 3, 1024, "Normalize"),
            _inQueue(inQueue),
            _kpQueue(kpQueue), _eeQueue(eeQueue) {}

    private:
        void taskFunction() override;
        void _armFK(const float ang_deg[6], float elbow_m[3], float wrist_m[3]);

        // ---- FK constants (human arm) ----
        static constexpr float DEG2RAD        = 3.14159265358979f / 180.0f;
        static constexpr float HUMAN_UPPER_M  = 0.35f;   // shoulder→elbow, meters
        static constexpr float HUMAN_FOREARM_M = 0.26f;   // elbow→wrist, meters

        // ---- Robot arm mapping ----
        static constexpr float ROBOT_UPPER_MM  = 260.0f;  // mm
        static constexpr float ROBOT_FOREARM_MM = 190.0f;  // mm

        // ---- Fusion ----
        static constexpr float FUSION_ALPHA = 0.95f;    // trust Jetson 95%

        // ---- Queues ----
        FreeRTOS::Queue<UartRecvTask::TransmitData>      &_inQueue;
        FreeRTOS::Queue<sharedDatatype::ArmKPCoords>     &_kpQueue;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData> &_eeQueue;
};
