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
 *
 * PWM mapping:
 *   0500 = -135° (fully open)
 *   1500 = 0° (mid)
 *   2500 = +135° (fully closed)
 *
 * For gripper we map 0~100% ratio → appropriate PWM range.
 */
class Gripper {
public:
    Gripper(uint8_t id = 0);

    /**
     * @brief Send init commands: set mode to 1, enable torque.
     */
    bool init();

    /**
     * @brief Set gripper opening ratio 0.0 (open) ~ 100.0 (closed).
     */
    void setRatio(float ratio);

    /**
     * @brief Release torque (free to turn by hand).
     */
    void release();

    /**
     * @brief Restore torque at current position.
     */
    void restore();

    /**
     * @brief Stop immediately at current position.
     */
    void stop();

    /**
     * @brief Request angle readback. Call periodically.
     *        Returns true if a reply was received and parsed.
     */
    bool getAngle();

    /**
     * @brief Last read angle in degrees (-135 ~ +135).
     */
    float lastAngle() const { return _lastAngle; }

    /**
     * @brief Is the gripper stuck? (target vs actual > threshold)
     */
    bool isStuck() const { return _stuck; }

    /**
     * @brief UART callback — feeds RX ring buffer from SCI5.
     */
    static void uartCallback(uart_callback_args_t *p_args);

private:
    void _sendCmd(const char* fmt, ...);

    uint8_t _id;
    float   _lastAngle = 0.0f;
    float   _targetPWM = 1500.0f;
    bool    _stuck = false;
    bool    _reading = false;   // true when waiting for reply

    static constexpr float PWM_MIN = 500.0f;
    static constexpr float PWM_MAX = 2500.0f;
    static constexpr float DEG_MIN = -135.0f;
    static constexpr float DEG_MAX = 135.0f;
    static constexpr float DEG_TO_PWM = (PWM_MAX - PWM_MIN) / (DEG_MAX - DEG_MIN);  // ≈ 7.407
    static constexpr float PWM_TO_DEG = 1.0f / (DEG_TO_PWM);

    static constexpr float STUCK_THRESH_DEG = 10.0f;

    // RX ring buffer for reply parsing
    static constexpr size_t RX_BUF_SIZE = 64;
    static uint8_t  _rxBuf[RX_BUF_SIZE];
    static uint8_t  _rxHead;
    static uint8_t  _rxCount;

    // TX complete flag
    static volatile bool _txDone;

    // Static TX buffer
    static char _txBuf[64];
};
