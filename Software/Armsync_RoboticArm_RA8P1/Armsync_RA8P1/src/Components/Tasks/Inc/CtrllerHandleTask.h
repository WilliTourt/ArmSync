#pragma once

#include <FreeRTOS/Task.hpp>
#include "cpp_main.h"

class CtrllerHandleTask : public FreeRTOS::Task {
    public:
        CtrllerHandleTask() : Task(tskIDLE_PRIORITY + 3, 512, "CtrllerHandle") {}

    private:
        void taskFunction() override;

};
