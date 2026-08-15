#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "cpp_main.h"
#include "hal_data.h"
#include "ringbuf.h"



class UartRecvTask : public FreeRTOS::Task {
    public:
    
        /* From controller (9 floats, comma/newline separated):
             elbow_x/y/z (unit vec) | wrist_x/y/z (unit vec) | forearm_pitch | grip | pitch
        int len = sprintf(tx_buf, "%.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.2f, %.2f\n",
            elx,ely,elz, wrx,wry,wrz, forearm_pitch, sliderGripPercent, sliderPitchPercent);
        */
        struct CtrllerData {
            float elbowVec[3];      // 上臂单位向量 (x,y,z)
            float wristVec[3];      // 小臂单位向量 (x,y,z)
            float forearmPitch;     // 前臂绕轴旋转角度 (deg, 手柄直接计算)
            float gripPercent;      // sliderGripPercent (0~100)
            float pitchPercent;     // sliderPitchPercent (0~100)
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
        void _emitFrame();
        void _send(uint32_t now);

        // handset/Jetson alignment
        JetsonData  _latestJetsonData  = {};   // last Jetson frame cache
        CtrllerData _latestCtrllerData = {};   // last handset frame cache
        bool        _newJetsonFlag     = false;// a fresh Jetson frame arrived
        bool        _handsetValid      = false;// at least one handset line parsed OK
        uint32_t    _lastJetsonTick    = 0;    // tick of last received Jetson frame

        // If no fresh Jetson frame for this long (ms), fall back to emitting handset frames alone
        static constexpr uint32_t JETSON_TIMEOUT_MS = 80;
};
