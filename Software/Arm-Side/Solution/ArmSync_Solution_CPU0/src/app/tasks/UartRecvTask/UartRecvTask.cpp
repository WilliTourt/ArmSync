#include "UartRecvTask.h"
#include "ElegantDebug.h"
#include <cstdlib>
#include <cstring>

extern ElegantDebug dbg;

uint8_t UartRecvTask::_rxRawCtrller[1024];
RingBuf UartRecvTask::_rxRingCtrller(UartRecvTask::_rxRawCtrller, sizeof(UartRecvTask::_rxRawCtrller));

uint8_t UartRecvTask::_rxRawJetson[512];
RingBuf UartRecvTask::_rxRingJetson(UartRecvTask::_rxRawJetson, sizeof(UartRecvTask::_rxRawJetson));

static UartRecvTask::JetsonData _latestJetsonData = {};

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

            for (int i = 0; i < 8; i++) {
                char *end = tok;

                while (*end && *end != ',' && *end != '\n' && *end != '\r') {
                    end++;
                }

                char saved = *end;
                *end = '\0';
                float val = strtof(tok, nullptr);
                if (i < 6) {
                    ctrl_data.angles[i] = val;
                } else {
                    ctrl_data.adc[i - 6] = val;
                }
                count++;

                if (saved == '\0' || saved == '\n' || saved == '\r') break;
                *end = saved;
                tok = end + 1;
            }

            if (count >= 6) {
                UartRecvTask::TransmitData data;
                data.ctrllerData = ctrl_data;
                data.jetsonData  = _latestJetsonData;  // latest available Jetson frame
                data.timestamp   = FreeRTOS::Kernel::getTickCount();

                _queue.sendToBack(data, 0);
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

        float jEtLen = sqrtf((float)jd.points[0][0]*jd.points[0][0] + (float)jd.points[0][1]*jd.points[0][1] + (float)jd.points[0][2]*jd.points[0][2]);
        float jWrLen = sqrtf((float)jd.points[1][0]*jd.points[1][0] + (float)jd.points[1][1]*jd.points[1][1] + (float)jd.points[1][2]*jd.points[1][2]);
        dbg.logWithType("JETSON", COLOR_CYAN,
            "KeyPoints: E(%d,%d,%d) |%.0fmm| W(%d,%d,%d) |%.0fmm|\n",
            jd.points[0][0], jd.points[0][1], jd.points[0][2], jEtLen,
            jd.points[1][0], jd.points[1][1], jd.points[1][2], jWrLen);

        // Test
        UartRecvTask::TransmitData data;
        data.ctrllerData = {};
        data.jetsonData  = _latestJetsonData;
        _queue.sendToBack(data, 0);
    }
}

void UartRecvTask::taskFunction() {
    dbg.info("UartRecvTask started.\n");

    for (;;) {
        _parseJetson();
        _parseCtrller();
        this->delay(pdMS_TO_TICKS(2));
    }
}
