#include "UartRecvTask.h"
#include "ElegantDebug.h"
#include <cstdlib>
#include <cstring>

extern ElegantDebug dbg;

uint8_t UartRecvTask::_rxRawCtrller[1024];
RingBuf UartRecvTask::_rxRingCtrller(UartRecvTask::_rxRawCtrller, sizeof(UartRecvTask::_rxRawCtrller));

void UartRecvTask::uartCallback(uart_callback_args_t *p_args) {
    if (p_args->event == UART_EVENT_RX_CHAR) {
        _rxRingCtrller.put((uint8_t)p_args->data);
    }
}

void UartRecvTask::taskFunction() {
    dbg.info("UartRecvTask started.\n");

    for (;;) {
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
                    // print raw line since printf %%f requires -u _printf_float
                    // dbg.info("OK %d: %s\n", count, (char*)line);
                    UartRecvTask::TransmitData data;
                    data.ctrllerData = ctrl_data;
                    // data.jetsonData = jetson_data;
                    data.timestamp = FreeRTOS::Kernel::getTickCount();

                    _queue.sendToBack(data, 0);
                } // else {
                //     // dbg.warning("Got %d values: %s\n", count, (char*)line);
                // }
            }
        }

        this->delay(pdMS_TO_TICKS(2));
    }
}
