#include "motor.h"
#include "app/drivers/Emm_V5.h"



void Motor::uartCallback(uart_callback_args_t *p_args) {
    Emm_V5_UartCallback(p_args);
}



Motor::Motor(uint8_t addr, float minDeg, float maxDeg,
             uint16_t maxVel, uint8_t acc, uint16_t stepsPerDeg) :
    _addr(addr), _minDeg(minDeg), _maxDeg(maxDeg) {
    _cfg.maxVel = maxVel;
    _cfg.acc = acc;
    _cfg.stepsPerDeg = stepsPerDeg;
}

bool Motor::init() {
    Emm_V5_Modify_Ctrl_Mode(_addr, true, 2);   // 闭环模式
    Emm_V5_En_Control(_addr, true, false);     // 使能
    return true;
}

void Motor::setAngle(float deg) {
    /* clamp to limits */
    if (deg < _minDeg) deg = _minDeg;
    if (deg > _maxDeg) deg = _maxDeg;

    uint8_t  dir = (deg >= 0) ? 0 : 1;
    uint32_t clk = (uint32_t)(ABS(deg) * (float)_cfg.stepsPerDeg);

    if (clk > 0) {
        Emm_V5_Pos_Control(_addr, dir, _cfg.maxVel, _cfg.acc, clk, false, false);
    }
}

void Motor::stop() {
    Emm_V5_Stop_Now(_addr, false);
}

bool Motor::isLocked() const {
    return false;  // TODO: Emm_V5_Read_Sys_Params(_addr, S_FLAG)
}
