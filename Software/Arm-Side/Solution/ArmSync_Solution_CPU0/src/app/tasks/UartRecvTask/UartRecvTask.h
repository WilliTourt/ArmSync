#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "cpp_main.h"
#include "hal_data.h"
#include "ringbuf.h"



class UartRecvTask : public FreeRTOS::Task {
    public:
    
        /* From controller:
        int len = sprintf(tx_buf, "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.2f,%.2f\n",
            armData.upper_pitch, armData.upper_roll, armData.upper_yaw,
            armData.rel_pitch, armData.rel_roll, armData.rel_yaw,
            sliderGripPercent, sliderPitchPercent);
        */
        struct CtrllerData {
            float angles[6];
            float adc[2];
        };

        struct JetsonData {
            const char *todo = "data to be defined later";
        };

        struct TransmitData {
            CtrllerData ctrllerData;
            JetsonData jetsonData;
            uint32_t timestamp;
        };

        UartRecvTask(FreeRTOS::Queue<TransmitData> &queue)
            : Task(tskIDLE_PRIORITY + 3, 1024, "UartRecv"), _queue(queue) {}

        static void uartCallback(uart_callback_args_t *p_args);

    private:
        void taskFunction() override;
        FreeRTOS::Queue<TransmitData> &_queue;

        static uint8_t _rxRawCtrller[1024];
        static RingBuf _rxRingCtrller;
};
