#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "queues.h"
#include "ringbuf.h"

class UITask : public FreeRTOS::Task {
    public:
        enum class BootingPhase : uint8_t {
            MCU_PERIPH  = 12,
            COMM_INIT   = 28,
            MEMORY_INIT = 45,
            ARM_SELFCHK = 68,
            DONE        = 100
        };

        enum CmdFlag {
            CMD_ESTOP = (0b0001 << 0),
            CMD_HOME  = (0b0001 << 1),
            CMD_PLAY  = (0b0001 << 2),
            CMD_REC   = (0b0001 << 3)
        };

        UITask(FreeRTOS::Queue<sharedDatatype::IPCFeedback>  &feedbackInQueue,
            FreeRTOS::Queue<sharedDatatype::EndEffectorData> &eeQueue)
            : Task(tskIDLE_PRIORITY + 2, 768, "UI"),
            _fdbk(feedbackInQueue), _eeQueue(eeQueue) {}

        static void uart3Callback(uart_callback_args_t *p_args);
        void booting(BootingPhase phase);

    private:
        void taskFunction() override;

        void _send(const char* fmt, ...);
        void _parseScreenInput();

        void _updateJointAngle(int idx, float angle_deg);
        void _updateJointStatus(int idx, bool ok);
        void _updateGrip(float percent, bool stuck);
        void _updateStatusText(const char* text);
        void _updateFreq(int hz);
        void _updateHMS(int line, const char* msg);

        static uint8_t      _rxBuf[128];
        static RingBuf      _rxRing;
        static volatile bool _txDone;    // true when UART is idle

        static constexpr uint16_t tjcCOLOR_GREEN = 16049;
        static constexpr uint16_t tjcCOLOR_RED   = 57929;

        FreeRTOS::Queue<sharedDatatype::IPCFeedback>     &_fdbk;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData> &_eeQueue;
};
