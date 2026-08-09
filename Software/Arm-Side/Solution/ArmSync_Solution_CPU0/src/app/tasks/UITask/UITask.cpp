#include "UITask.h"
#include "ElegantDebug.h"
#include "hal_data.h"
#include <cstdio>
#include <cstdarg>
#include <cstring>

extern ElegantDebug dbg;


uint8_t UITask::_rxBuf[128];
RingBuf UITask::_rxRing(UITask::_rxBuf, sizeof(UITask::_rxBuf));
volatile bool UITask::_txDone = true;

void UITask::uart3Callback(uart_callback_args_t *p_args) {
    if (p_args->event == UART_EVENT_RX_CHAR) {
        _rxRing.put((uint8_t)p_args->data);
    }
    if (p_args->event == UART_EVENT_TX_COMPLETE) {
        _txDone = true;
    }
}



static char _tjcBuf[128];

void UITask::_send(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(_tjcBuf, sizeof(_tjcBuf) - 3, fmt, args);
    va_end(args);
    if (len < 0) return;

    _tjcBuf[len]     = '\xFF';
    _tjcBuf[len + 1] = '\xFF';
    _tjcBuf[len + 2] = '\xFF';
    len += 3;

    // Wait for previous TX to complete
    while (!_txDone) {
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
    }
    _txDone = false;
    R_SCI_B_UART_Write(&g_uart3_ctrl, (uint8_t*)_tjcBuf, (uint32_t)len);
}

void UITask::_parseScreenInput() {
    static const char* known[] = {"BTZ_PRESSED", "ESTOP_PRESSED", "REC_PRESSED", "PLAY_PRESSED"};
    static const size_t knownLen[] = {11, 13, 11, 12};

    for (int i = 0; i < 4; i++) {
        if (_rxRing.available() >= knownLen[i]) {
            uint8_t buf[16] = {};
            _rxRing.peek(buf, knownLen[i]);
            if (memcmp(buf, known[i], knownLen[i]) == 0) {
                for (size_t j = 0; j < knownLen[i]; j++) _rxRing.get();
                dbg.logWithType("TJC BTN", COLOR_YELLOW, "%s\n", known[i]);
                // TODO: trigger actions
            }
        }
    }
}



void UITask::_updateJointAngle(int idx, float angle_deg) {
    // J1Angle ~ J6Angle, virtual float vvs1=2
    int val = (int)(angle_deg * 100.0f);
    _send("J%dAngle.val=%d", idx + 1, val);
}

void UITask::_updateJointStatus(int idx, bool ok) {
    // J1Status ~ J6Status: text "●", color green/red
    _send("J%dStatus.txt=\"●\"", idx + 1);
    _send("J%dStatus.pco=%d", idx + 1, (ok ? tjcCOLOR_GREEN : tjcCOLOR_RED));
}

void UITask::_updateGrip(float percent, bool stuck) {
    // GRIPPercent: 0=69mm open, 100=0mm closed
    _send("GRIPPercent.val=%d", (int)percent);
    // GRIPStatus: dot color
    _send("GRIPStatus.txt=\"●\"");
    _send("GRIPStatus.pco=%d", (stuck ? tjcCOLOR_RED : tjcCOLOR_GREEN));
}

void UITask::_updateStatusText(const char* text) {
    _send("StatusText.txt=\"%s\"", text);
}

void UITask::_updateFreq(int hz) {
    _send("Freq.txt=\"%dHz\"", hz);
}

void UITask::_updateHMS(int line, const char* msg) {
    // HMS_Msg0 ~ HMS_Msg4, max 26 chars
    _send("HMS_Msg%d.txt=\"%s\"", line, msg);
}



void UITask::booting(BootingPhase phase) {
    _send("ProgressBar.val=%d", static_cast<int>(phase));

    if (phase == BootingPhase::DONE) {
        _send("page ArmSync_Main");
    }
}


void UITask::taskFunction() {
    dbg.info("UITask started.\n");

    for (;;) {
        _parseScreenInput();

        auto feedback = _fdbk.receive(0);
        if (feedback) {
            for (int i = 0; i < 6; i++) {
                _updateJointAngle(i, feedback->jointAngle[i]);
            }
            for (int i = 0; i < 6; i++) {
                _updateJointStatus(i, !feedback->isLockedRotor);
            }
        }

        auto ee = _eeQueue.receive(0);
        if (ee) {
            _updateGrip(ee->grip_percent, false);
        }

        this->delay(pdMS_TO_TICKS(50));
    }
}
