#pragma once

#include <cstdint>
#include "r_adc.h"

class AdcBase {
    public:
        static void init();
        static void startScan();
        static void onScanCplt();
        static bool isScanDone();

    private:
        static bool _initialized;
        static volatile bool _scan_done;
};

class AdcSlider : public AdcBase {
    public:
        AdcSlider(adc_channel_t channel, uint16_t min = 0, uint16_t max = 16383);
        
        void update();
        uint16_t getRaw() const { return _raw_value; }
        float getPercent() const { return _mapToPercent(); };

    private:
        adc_channel_t _channel;
        uint16_t _min; // calibration min
        uint16_t _max; // calibration max
        volatile uint16_t _raw_value = 0;

        float _mapToPercent() const;
};
