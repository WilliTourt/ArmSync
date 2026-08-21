#include "UartRecvTask.h"
#include "ElegantDebug.h"
#include <cstdlib>
#include <cstring>

extern ElegantDebug dbg;

uint8_t UartRecvTask::_rxRawCtrller[1024];
RingBuf UartRecvTask::_rxRingCtrller(UartRecvTask::_rxRawCtrller, sizeof(UartRecvTask::_rxRawCtrller));

uint8_t UartRecvTask::_rxRawJetson[512];
RingBuf UartRecvTask::_rxRingJetson(UartRecvTask::_rxRawJetson, sizeof(UartRecvTask::_rxRawJetson));

void UartRecvTask::uart0Callback(uart_callback_args_t *p_args) {
    if (p_args->event == UART_EVENT_RX_CHAR) {
        _rxRingCtrller.put((uint8_t)p_args->data);
    }
}

void UartRecvTask::uart4Callback(uart_callback_args_t *p_args) {
    if (p_args->event == UART_EVENT_RX_CHAR) {
        _rxRingJetson.put((uint8_t)p_args->data);
    }
}

void UartRecvTask::_parseCtrller() {
    if (_rxRingCtrller.hasLine()) {
        uint8_t line[128];
        size_t  len = _rxRingCtrller.readLine(line, sizeof(line));

        if (len > 0) {
            UartRecvTask::CtrllerData ctrl_data = {};
            int   count = 0;
            char *tok = (char*)line;

            // Protocol: elbow[3] + wrist[3] + upperarm_pitch + forearm_pitch + grip + pitch = 10 floats
            for (int i = 0; i < 10; i++) {
                char *end = tok;
                while (*end && *end != ',' && *end != '\n' && *end != '\r') {
                    end++;
                }
                char saved = *end;
                *end = '\0';
                float val = strtof(tok, nullptr);

                switch (i) {
                    case 0: ctrl_data.elbowVec[0] = val; break;
                    case 1: ctrl_data.elbowVec[1] = val; break;
                    case 2: ctrl_data.elbowVec[2] = val; break;
                    case 3: ctrl_data.wristVec[0] = val; break;
                    case 4: ctrl_data.wristVec[1] = val; break;
                    case 5: ctrl_data.wristVec[2] = val; break;
                    case 6: ctrl_data.upperarmPitch = val; break;
                    case 7: ctrl_data.forearmPitch = val; break;
                    case 8: ctrl_data.gripPercent = val; break;
                    case 9: ctrl_data.pitchPercent = val; break;
                }
                count++;

                if (saved == '\0' || saved == '\n' || saved == '\r') break;
                *end = saved;
                tok = end + 1;
            }

            if (count >= 10) {
                // Cache newest handset frame only
                _latestCtrllerData = ctrl_data;
                _handsetValid = true;

                // dbg.info("Controller: E(%f, %f, %f) W(%f, %f, %f)\n",
                //     ctrl_data.elbowVec[0], ctrl_data.elbowVec[1], ctrl_data.elbowVec[2],
                //     ctrl_data.wristVec[0], ctrl_data.wristVec[1], ctrl_data.wristVec[2]);
            }
        }
    }
}

void UartRecvTask::_parseJetson() {
    while (_rxRingJetson.available() >= JETSON_FRAME_LEN) {

        // 1. Read header
        int b = _rxRingJetson.get();
        if (b != JETSON_FRAME_HEADER) {
            continue;  // not a frame start, discarded
        }

        // 2. Read remaining payload + footer into buf
        uint8_t buf[JETSON_FRAME_LEN];
        buf[0] = JETSON_FRAME_HEADER;  // already consumed
        for (size_t i = 1; i < JETSON_FRAME_LEN; i++) {
            buf[i] = (uint8_t)_rxRingJetson.get();
        }

        // 3. Validate footer
        if (buf[JETSON_FRAME_LEN - 1] != JETSON_FRAME_FOOTER) {
            continue;  // bad footer, frame discarded
        }

        // 4. Valid frame — parse payload (elbow/wrist/index/thumb) 
        JetsonData jd;
        const uint8_t *payload = &buf[1];  // skip header
        for (size_t i = 0; i < JETSON_NUM_KEYPOINTS; i++) {
            jd.points[i][0] = (int16_t)(payload[i*6+0] | ((int16_t)payload[i*6+1] << 8));
            jd.points[i][1] = (int16_t)(payload[i*6+2] | ((int16_t)payload[i*6+3] << 8));
            jd.points[i][2] = (int16_t)(payload[i*6+4] | ((int16_t)payload[i*6+5] << 8));
        }
        jd.valid = true;
        _latestJetsonData = jd;

        // dbg.info("Jetson: E(%d, %d, %d) W(%d, %d, %d)\n",
            // jd.points[0][0], jd.points[0][1], jd.points[0][2],
            // jd.points[1][0], jd.points[1][1], jd.points[1][2]);

        _newJetsonFlag    = true; // mark: a fresh Jetson frame
        _lastJetsonTick   = FreeRTOS::Kernel::getTickCount();
    }
}

void UartRecvTask::_send(uint32_t now) {
    UartRecvTask::TransmitData data;
    data.ctrllerData = _latestCtrllerData;
    data.jetsonData  = _latestJetsonData;
    data.timestamp   = now;
    _queue.sendToBack(data, 0);
}

// Decide WHEN to emit, and emit the merged frame. Jetson is the clock source, 
// the camera carries latency, so:
//   - fresh Jetson frame: emit now (Jetson cadence)
//   - Jetson dead but handset valid -> emit handset-only
//   - otherwise emit nothing
void UartRecvTask::_emitFrame() {
    uint32_t now = FreeRTOS::Kernel::getTickCount();

    // Case 1
    if (_newJetsonFlag) {
        _newJetsonFlag = false;
        _send(now);
        return;
    }

    // Case 2
    bool jetsonAlive = ((_lastJetsonTick != 0) && ((now - _lastJetsonTick) < JETSON_TIMEOUT_MS));
    if (_handsetValid && !jetsonAlive) {
        _send(now);
        return;
    }

    // Case 3
    // dbg.error("UART: NO JETSON AND HANDSET INPUT!\n");
}

void UartRecvTask::taskFunction() {
    dbg.ok("UartRecvTask started.\n");

    for (;;) {
        _parseJetson();
        _parseCtrller();
        _emitFrame();
        this->delay(pdMS_TO_TICKS(3));
    }
}
