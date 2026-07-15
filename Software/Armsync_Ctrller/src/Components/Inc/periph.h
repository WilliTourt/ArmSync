#pragma once

#include "hal_data.h"

namespace Periph {
    void init();
    void initTimers();
    void setBuzzerDutyCycle(uint8_t dutyCycle);
    void setBuzzerFreq(uint32_t freq);
}