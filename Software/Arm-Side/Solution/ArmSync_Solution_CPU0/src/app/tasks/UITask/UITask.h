#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "queues.h"
#include "RecPlayTask/RecPlayTask.h"
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

        enum class StatusText {
            MANUAL = 0,
            RECORD = 1,
            AUTO   = 2,
            ERROR  = 3
        };

        UITask(FreeRTOS::Queue<sharedDatatype::IPCFeedback>  &feedbackInQueue,
            FreeRTOS::Queue<sharedDatatype::EndEffectorData> &eeUIQueue)
            : Task(tskIDLE_PRIORITY + 2, 768, "UI"),
            _fdbk(feedbackInQueue), _eeUIQueue(eeUIQueue) {}

        static void uart3Callback(uart_callback_args_t *p_args);
        void booting(BootingPhase phase);

        void updateStatusText(StatusText text);
        void updateFreq(int hz);
        void updateHMS(int line, const char* msg);

        void setTaskHandles(TaskHandle_t fusion, TaskHandle_t recplay,
                            TaskHandle_t uartRecv, TaskHandle_t normalize,
                            TaskHandle_t ik);

    private:
        void taskFunction() override;

        void _send(const char* fmt, ...);
        void _parseScreenInput();
        void _notifyFusion();   // push current UICommand to FusionTask
        void _notifyRecPlay(RecPlayTask::RecCmd cmd);
        void _suspendUpstream();
        void _resumeUpstream();

        void _updateJointAngle(int idx, float angle_deg);
        void _updateJointStatus(int idx, bool ok);
        void _updateGrip(float percent, bool stuck);


        static uint8_t      _rxBuf[128];
        static RingBuf      _rxRing;
        static volatile bool _txDone;    // true when UART is idle

        static constexpr uint16_t tjcCOLOR_GREEN = 16049;
        static constexpr uint16_t tjcCOLOR_RED   = 57929;

        TaskHandle_t _fusionHandle = nullptr;   // receives UICommand (REC start/end)
        TaskHandle_t _recPlayHandle = nullptr;  // receives RecCmd (PLAY start/end)

        // Tasks suspended while PLAY is active (everything upstream of Motion).
        TaskHandle_t _suspendHandles[4] = {nullptr, nullptr, nullptr, nullptr};

        // UI button state rules:
        //  - ESTOP always valid
        //  - while PLAY: REC and HOME are ignored
        //  - while REC : PLAY is ignored; HOME remains valid
        bool _isRecording = false;
        bool _isPlaying   = false;

        FreeRTOS::Queue<sharedDatatype::IPCFeedback>     &_fdbk;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData> &_eeUIQueue;
};
