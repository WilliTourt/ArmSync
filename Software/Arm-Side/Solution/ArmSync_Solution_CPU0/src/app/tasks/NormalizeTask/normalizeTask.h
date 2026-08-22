#pragma once

#include <FreeRTOS/Task.hpp>
#include "queues.h"

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
        void _updateAlphaFromJ2(float j2deg); // Update hand/Jetson blend alpha from fused J2 (deg) pushed by FusionTask.

        static constexpr float HUMAN_UPPER_M   = 0.35f;   // shoulder -> elbow
        static constexpr float HUMAN_FOREARM_M = 0.26f;   // elbow -> wrist

        static constexpr float ALPHA_J2_LOW   = 0.34f;
        static constexpr float ALPHA_J2_HIGH  = 0.04f;
        static constexpr float ALPHA_J2_HIGH_DEG = 70.0f;
        float _alpha = 0.04f;   // hand/Jetson blend alpha

        FreeRTOS::Queue<UartRecvTask::TransmitData>      &_inQueue;
        FreeRTOS::Queue<sharedDatatype::ArmKPCoords>     &_kpQueue;
        FreeRTOS::Queue<sharedDatatype::HandJointData>   &_handQueue;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData> &_eeQueue;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData> &_eeUIQueue;
};
