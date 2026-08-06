#include "adc.h"
#include "hal_data.h"

// ---- AdcBase ----

volatile bool AdcBase::_scan_done = false;
bool AdcBase::_initialized = false;

void AdcBase::init() {
    if (_initialized) return;

    fsp_err_t err = R_ADC_Open(g_adc0.p_ctrl, g_adc0.p_cfg);
    assert(err == FSP_SUCCESS);
    err = R_ADC_ScanCfg(g_adc0.p_ctrl, g_adc0.p_channel_cfg);
    assert(err == FSP_SUCCESS);

    _initialized = true;
}

void AdcBase::startScan() {
    _scan_done = false;
    R_ADC_ScanStart(g_adc0.p_ctrl);
}

void AdcBase::onScanCplt() {
    _scan_done = true;
}

bool AdcBase::isScanDone() {
    return _scan_done;
}

// ---- AdcSlider ----

AdcSlider::AdcSlider(adc_channel_t ch, uint16_t min, uint16_t max) : 
    _channel(ch),
    _min(min),
    _max(max) {}

void AdcSlider::update() {
    if (isScanDone()) {
        R_ADC_Read(g_adc0.p_ctrl, _channel, (uint16_t *)&_raw_value);
    }
}

float AdcSlider::_mapToPercent() const {
    uint16_t val = _raw_value;
    if (val < _min) val = _min;
    if (val > _max) val = _max;
    return ((static_cast<float>(val - _min) / static_cast<float>(_max - _min)) * 100.0f);
}
