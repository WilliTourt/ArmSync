#include "CtrllerHandleTask.h"
#include "ElegantDebug.h"
#include <cstdlib>
#include <cstring>

extern ElegantDebug dbg;

uint8_t CtrllerHandleTask::_rxRaw[1024];
RingBuf CtrllerHandleTask::_rxRing(CtrllerHandleTask::_rxRaw, sizeof(CtrllerHandleTask::_rxRaw));

void CtrllerHandleTask::uartCallback(uart_callback_args_t *p_args) {
    if (p_args->event == UART_EVENT_RX_CHAR) {
        _rxRing.put((uint8_t)p_args->data);
    }
}

void CtrllerHandleTask::taskFunction() {
    dbg.info("CtrllerHandleTask started.\n");

    for (;;) {
        if (_rxRing.hasLine()) {
            uint8_t line[128];
            size_t  len = _rxRing.readLine(line, sizeof(line));

            if (len > 0) {
                
                CtrllerHandleTask::CtrllerData data = {};
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
                        data.angles[i] = val;
                    } else {
                        data.adc[i - 6] = val;
                    }
                    count++;

                    if (saved == '\0' || saved == '\n' || saved == '\r') break;
                    *end = saved;
                    tok = end + 1;
                }

                if (count >= 6) {
                    // print raw line since printf %%f requires -u _printf_float
                    // dbg.info("OK %d: %s\n", count, (char*)line);
                    _queue.sendToBack(data, 0);
                }
                // else {
                //     // dbg.warning("Got %d values: %s\n", count, (char*)line);
                // }
            }
        }

        this->delay(pdMS_TO_TICKS(2));
    }
}
