#pragma once

#include <FreeRTOS/Task.hpp>
#include "queues.h"

/**
 * @brief Task 2: Data normalization
 *
 * Get UartRecvTask::TransmitData. Convert the hand unit-vectors into elbow /
 * wrist coordinates (in mm), fuse them with the Jetson keypoints (this is the
 * J1~J4 alpha blend), then output:
 *   - ArmKPCoords   -> IKTask      (J1~J4 source)
 *   - HandJointData -> FusionTask  (J5 + pitch; J5 = forearm_pitch from handset)
 *   - EndEffectorData (grip)       -> UI / grip control
 */
class NormalizeTask : public FreeRTOS::Task {
    public:
        NormalizeTask(FreeRTOS::Queue<UartRecvTask::TransmitData>      &inQueue,
                      FreeRTOS::Queue<sharedDatatype::ArmKPCoords>     &kpQueue,
                      FreeRTOS::Queue<sharedDatatype::HandJointData>   &handQueue,
                      FreeRTOS::Queue<sharedDatatype::EndEffectorData> &eeQueue,
                      FreeRTOS::Queue<sharedDatatype::EndEffectorData> &eeUIQueue)
            : Task(tskIDLE_PRIORITY + 3, 2048, "Normalize"),
            _inQueue(inQueue), _kpQueue(kpQueue), _handQueue(handQueue),
            _eeQueue(eeQueue), _eeUIQueue(eeUIQueue) {}

    private:
        void taskFunction() override;

        static constexpr float HUMAN_UPPER_M   = 0.35f;   // shoulder -> elbow
        static constexpr float HUMAN_FOREARM_M = 0.26f;   // elbow -> wrist

        static constexpr float FUSION_ALPHA = 0.07f;    // trust Jetson two-thirds

        FreeRTOS::Queue<UartRecvTask::TransmitData>      &_inQueue;
        FreeRTOS::Queue<sharedDatatype::ArmKPCoords>     &_kpQueue;
        FreeRTOS::Queue<sharedDatatype::HandJointData>   &_handQueue;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData> &_eeQueue;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData> &_eeUIQueue;
};
