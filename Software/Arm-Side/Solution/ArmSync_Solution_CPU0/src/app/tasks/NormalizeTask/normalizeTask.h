#pragma once

#include <FreeRTOS/Task.hpp>
#include "queues.h"

/**
 * @brief Task 2: Time synchronizaion and data normalization
 * 
 * Get UartRecvTask::TransmitData from UartRecvTask,
 * runs the normalize function, output the result to Attitude6DOF queue.
 */
class NormalizeTask : public FreeRTOS::Task {
    public:
        NormalizeTask(FreeRTOS::Queue<UartRecvTask::TransmitData>   &inQueue,
                      FreeRTOS::Queue<sharedDatatype::Attitude6DOF> &outQueue)
            : Task(tskIDLE_PRIORITY + 3, 1024, "Normalize"), _inQueue(inQueue), _outQueue(outQueue) {}

    private:
        void taskFunction() override;

        FreeRTOS::Queue<UartRecvTask::TransmitData>   &_inQueue;
        FreeRTOS::Queue<sharedDatatype::Attitude6DOF> &_outQueue;
};
