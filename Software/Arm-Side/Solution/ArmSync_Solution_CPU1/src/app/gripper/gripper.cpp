#include "gripper.h"
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cmath>

// Static members
uint8_t Gripper::_rxBuf[RX_BUF_SIZE];
uint8_t Gripper::_rxHead = 0;
uint8_t Gripper::_rxCount = 0;
volatile bool Gripper::_txDone = true;
char    Gripper::_txBuf[64];

Gripper::Gripper(uint8_t id) : _id(id) {}

void Gripper::uartCallback(uart_callback_args_t *p_args) {
    if (p_args->event == UART_EVENT_RX_CHAR) {
        if (_rxCount < RX_BUF_SIZE) {
            uint8_t idx = (_rxHead + _rxCount) % RX_BUF_SIZE;
            _rxBuf[idx] = (uint8_t)p_args->data;
            _rxCount++;
        }
    }
    if (p_args->event == UART_EVENT_TX_COMPLETE) {
        _txDone = true;
    }
}

bool Gripper::init() {
    _sendCmd("#%03dPMOD1!", _id);
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    _sendCmd("#%03dPULR!", _id);
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    _sendCmd("#%03dP1500T0500!", _id);

    return true;
}

void Gripper::setRatio(float ratio) {
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 100.0f) ratio = 100.0f;

    // Map 0~100% -> PWM range (open=800, closed=2200)
    float pwm = OPEN_PWM + (CLOSE_PWM - OPEN_PWM) * ratio / 100.0f;
    _targetPWM = pwm;

    _sendCmd("#%03dP%04dT0000!", _id, (int)pwm);
}

void Gripper::release() {
    _sendCmd("#%03dPULK!", _id);
}

void Gripper::restore() {
    _sendCmd("#%03dPULR!", _id);
}

void Gripper::stop() {
    _sendCmd("#%03dPDST!", _id);
}

void Gripper::updateFeedback() {
    // Step 1: send PRAD query if not already waiting
    if (!_reading) {
        _rxHead = 0;
        _rxCount = 0;
        _reading = true;
        
        _sendCmd("#%03dPRAD!", _id);
        return;
    }

    // Step 2: wait for reply
    if (_rxCount == 0) return;

    // Step 3: parse reply
    uint8_t reply[32];
    size_t n = _rxCount < sizeof(reply) - 1 ? _rxCount : sizeof(reply) - 1;
    for (size_t i = 0; i < n; i++) {
        reply[i] = _rxBuf[(_rxHead + i) % RX_BUF_SIZE];
    }
    reply[n] = '\0';
    _rxHead = 0;
    _rxCount = 0;
    _reading = false;

    // Parse "#000P1500!"
    char* p = (char*)reply + 4;   // skip "#000"
    if (*p != 'P') return;
    p++;

    float pwmVal = 0.0f;
    while (*p >= '0' && *p <= '9') {
        pwmVal = pwmVal * 10.0f + (*p - '0');
        p++;
    }

    _lastAngle = (pwmVal - 1500.0f) * PWM_TO_DEG;

    float targetDeg = (_targetPWM - 1500.0f) * PWM_TO_DEG;
    _stuck = (fabsf(_lastAngle - targetDeg) > STUCK_THRESH_DEG);
}

// ===== Private helpers =====

void Gripper::_sendCmd(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(_txBuf, sizeof(_txBuf), fmt, args);
    va_end(args);
    if (len <= 0) return;

    // Wait for previous TX to complete
    while (!_txDone) {
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
    }
    _txDone = false;
    R_SCI_B_UART_Write(&g_uart5_ctrl, (uint8_t*)_txBuf, (uint32_t)len);
}
