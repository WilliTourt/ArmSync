#pragma once

#include <FreeRTOS/Task.hpp>

#include "cpp_main.h"
#include "ElegantDebug.h"

extern ElegantDebug dbg;

class BlinkTask : public FreeRTOS::Task {
    public:
        BlinkTask() : Task(tskIDLE_PRIORITY + 1, 512, "Blink") {}

    private:
        void taskFunction() override {
            for (;;) {
                R_IOPORT_PinWrite(&g_ioport_ctrl, LED_USER, BSP_IO_LEVEL_HIGH);
                this->delay(100);
                // dbg.ok("Blinkyy~\n");
                R_IOPORT_PinWrite(&g_ioport_ctrl, LED_USER, BSP_IO_LEVEL_LOW);
                this->delay(900);
            }
        }
};
