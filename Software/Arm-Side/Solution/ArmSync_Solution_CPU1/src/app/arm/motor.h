#pragma once

#include "hal_data.h"
#include <stdint.h>

class Motor {
    public:
        struct Config {
            uint16_t maxVel  = 1000;
            uint8_t  acc     = 180;
            uint16_t stepsPerDeg = 100;   // TODO: should check ZDT settings
        };

        Motor(uint8_t addr, float minDeg, float maxDeg,
              uint16_t maxVel = 1000, uint8_t acc = 180, uint16_t stepsPerDeg = 100);

        bool init();
        void setAngle(float deg);
        void stop();
        bool isLocked() const;

        uint8_t addr() const { return _addr; }

        static void uartCallback(uart_callback_args_t *p_args);

    private:
        uint8_t _addr;
        float   _minDeg, _maxDeg;
        Config  _cfg;
};
