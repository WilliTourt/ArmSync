#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "queues.h"

class IKTask : public FreeRTOS::Task {
public:
    IKTask(FreeRTOS::Queue<sharedDatatype::ArmKPCoords> &inQueue,
           FreeRTOS::Queue<sharedDatatype::JointOutput>  &outQueue)
        : Task(tskIDLE_PRIORITY + 3, 2048, "IK"),
          _inQueue(inQueue), _outQueue(outQueue) {}

private:
    void taskFunction() override;

    FreeRTOS::Queue<sharedDatatype::ArmKPCoords> &_inQueue;
    FreeRTOS::Queue<sharedDatatype::JointOutput>  &_outQueue;
};
