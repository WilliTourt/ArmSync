#pragma once

#include <FreeRTOS/Task.hpp>

#include "cpp_main.h"

class BlinkTask : public FreeRTOS::Task {
    public:
        BlinkTask() : Task(tskIDLE_PRIORITY + 2, 128, "Blink") {}

    private:
        void taskFunction() override {
            for (;;) {
                R_IOPORT_PinWrite(&g_ioport_ctrl, LED_USER, BSP_IO_LEVEL_HIGH);
                this->delayUntil(pdMS_TO_TICKS(100));
                R_IOPORT_PinWrite(&g_ioport_ctrl, LED_USER, BSP_IO_LEVEL_LOW);
                this->delayUntil(pdMS_TO_TICKS(900));
            }
        }
};
