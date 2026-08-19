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
                      FreeRTOS::Queue<sharedDatatype::EndEffectorData> &eeUIQueue,
                      FreeRTOS::Queue<sharedDatatype::ArmKPCoords>     &npuKpQueue)
            : Task(tskIDLE_PRIORITY + 3, 2048, "Normalize"),
            _inQueue(inQueue), _kpQueue(kpQueue), _handQueue(handQueue),
            _eeQueue(eeQueue), _eeUIQueue(eeUIQueue), _npuKpQueue(npuKpQueue) {}

    private:
        void taskFunction() override;
        // Update hand/Jetson blend alpha from fused J2 (deg) pushed by FusionTask.
        void _updateAlphaFromJ2(float j2deg);

        static constexpr float HUMAN_UPPER_M   = 0.35f;   // shoulder -> elbow
        static constexpr float HUMAN_FOREARM_M = 0.26f;   // elbow -> wrist

        // J2-dependent hand/Jetson blend weight. ALPHA_x = trust Jetson x100.
        //   J2 <= 0  -> 0.78   (arm hangs / low -> trust Jetson more)
        //   J2 >= 75 -> 0.06   (arm raised  -> trust hand more)
        //   linear in between.
        static constexpr float ALPHA_J2_LOW   = 0.40f;   // J2 <=  0
        static constexpr float ALPHA_J2_HIGH  = 0.06f;   // J2 >= 75
        static constexpr float ALPHA_J2_HIGH_DEG = 75.0f;
        float _alpha = 0.76f;   // hand/Jetson blend alpha (starts at old default)

        FreeRTOS::Queue<UartRecvTask::TransmitData>      &_inQueue;
        FreeRTOS::Queue<sharedDatatype::ArmKPCoords>     &_kpQueue;
        FreeRTOS::Queue<sharedDatatype::HandJointData>   &_handQueue;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData> &_eeQueue;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData> &_eeUIQueue;
        FreeRTOS::Queue<sharedDatatype::ArmKPCoords>     &_npuKpQueue;   // mirror to NPUTask
};
