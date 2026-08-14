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

        // Jetson sends 4 keypoints (elbow/wrist/index/thumb), each 3×int16 (x,y,z) mm
        static constexpr size_t JETSON_NUM_KEYPOINTS  = 4;  // elbow, wrist, index, thumb
        struct JetsonData {
            int16_t points[JETSON_NUM_KEYPOINTS][3];  // [idx][0=x,1=y,2=z] in mm
            bool valid = false;
        };

        static constexpr uint8_t JETSON_FRAME_HEADER = 0xAA;
        static constexpr uint8_t JETSON_FRAME_FOOTER = 0x0D;
        static constexpr size_t  JETSON_PAYLOAD_LEN  = JETSON_NUM_KEYPOINTS * 3 * sizeof(int16_t);  // 4 × 6 = 24 bytes
        static constexpr size_t  JETSON_FRAME_LEN    = 1 + JETSON_PAYLOAD_LEN + 1;  // header + payload + footer = 26

        struct TransmitData {
            CtrllerData ctrllerData;
            JetsonData jetsonData;
            uint32_t timestamp;
        };

        UartRecvTask(FreeRTOS::Queue<TransmitData> &queue)
            : Task(tskIDLE_PRIORITY + 3, 1024, "UartRecv"), _queue(queue) {}

        static void uart0Callback(uart_callback_args_t *p_args);
        static void uart4Callback(uart_callback_args_t *p_args);

    private:
        void taskFunction() override;
        FreeRTOS::Queue<TransmitData> &_queue;

        static uint8_t _rxRawCtrller[1024];
        static RingBuf _rxRingCtrller;

        static uint8_t _rxRawJetson[512];
        static RingBuf _rxRingJetson;

        void _parseJetson();
        void _parseCtrller();
};
