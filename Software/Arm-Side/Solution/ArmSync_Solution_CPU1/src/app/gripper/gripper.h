#pragma once

#include "hal_data.h"
#include <stdint.h>

/**
 * @brief ZP20S bus servo gripper, driven by single-wire UART (SCI5 half-duplex).
 *
 * Control protocol:
 *   #000PXXXXTYYYY!  — PWM 0500~2500, time 0000~9999 ms
 *   #000PRAD!         — Read current angle
 *   #000PULK!         — Release torque (free spin)
 *   #000PULR!         — Restore torque
 *
 * Default ID=0, baud=115200, 270° mode (1).
 */
class Gripper {
public:
    Gripper(uint8_t id = 0);

    bool init();

    void setRatio(float ratio);   // 0.0 (open) ~ 100.0 (closed)
    void release();
    void restore();
    void stop();

    // Non-blocking: send PRAD query / parse reply, called periodically
    void updateFeedback();

    // Pure getters
    float getAngle() const { return _lastAngle; }
    bool  isStuck()  const { return _stuck; }

    static void uartCallback(uart_callback_args_t *p_args);

private:
    void _sendCmd(const char* fmt, ...);

    uint8_t _id;
    float   _lastAngle = 0.0f;
    float   _targetPWM = 1500.0f;
    bool    _stuck = false;
    bool    _reading = false;   // true while waiting for PRAD reply

    static constexpr float PWM_MIN = 500.0f;
    static constexpr float PWM_MAX = 2500.0f;
    static constexpr float DEG_MIN = -135.0f;
    static constexpr float DEG_MAX = 135.0f;
    static constexpr float PWM_TO_DEG = (DEG_MAX - DEG_MIN) / (PWM_MAX - PWM_MIN);

    static constexpr float STUCK_THRESH_DEG = 10.0f;

    static constexpr size_t RX_BUF_SIZE = 64;
    static uint8_t  _rxBuf[RX_BUF_SIZE];
    static uint8_t  _rxHead;
    static uint8_t  _rxCount;

    static volatile bool _txDone;
    static char _txBuf[64];
};
