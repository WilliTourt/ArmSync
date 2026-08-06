#pragma once
#include <FreeRTOS/Task.hpp>

class BlinkTask : public FreeRTOS::Task {
public:
    BlinkTask() : Task(tskIDLE_PRIORITY + 1, 256, "Blink") {}
private:
    void taskFunction() override;
};
