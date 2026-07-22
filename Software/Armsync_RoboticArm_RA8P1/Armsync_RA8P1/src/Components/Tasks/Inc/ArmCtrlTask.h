#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "CtrllerHandleTask.h"
#include "cpp_main.h"

class ArmCtrlTask : public FreeRTOS::Task {
    public:
        ArmCtrlTask(FreeRTOS::Queue<CtrllerHandleTask::CtrllerData> &queue)
            : Task(tskIDLE_PRIORITY + 4, 512, "ArmCtrl"), _queue(queue) {}

    private:
        void taskFunction() override;
        FreeRTOS::Queue<CtrllerHandleTask::CtrllerData> &_queue;
};
