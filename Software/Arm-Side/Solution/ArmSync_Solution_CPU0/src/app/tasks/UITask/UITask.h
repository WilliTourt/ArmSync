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
            MANUAL  = 0,
            RECORD  = 1,
            PLAYING = 2,
            STOP    = 3
        };

        // NPU status shown on the UI (NPUStatus dot + NPUText).
        enum class NpuState {
            OFF      = 0,   // grey  33808
            IDLE     = 1,   // orange 62756
            RUNNING  = 2,   // green  16049
        };

        UITask(FreeRTOS::Queue<sharedDatatype::IPCFeedback>  &feedbackInQueue,
            FreeRTOS::Queue<sharedDatatype::EndEffectorData> &eeUIQueue)
            : Task(tskIDLE_PRIORITY + 3, 1536, "UI"),
            _fdbk(feedbackInQueue), _eeUIQueue(eeUIQueue) {}

        static void uart3Callback(uart_callback_args_t *p_args);
        void booting(BootingPhase phase);

        // Static so any CPU0 task (RecPlay/Normalize/Fusion/...) can report
        // the shared UI status / message line.
        static void updateStatusText(StatusText text);
        static void updateFreq(int hz);
        static void updateHMS(const char* msg);        // circular HMS_Msg0~4 slot, usages: any task
        static void updateNPUStatus(NpuState state);   // NPUStatus dot + NPUText

        void setTaskHandles(TaskHandle_t fusion, TaskHandle_t recplay,
                            TaskHandle_t uartRecv, TaskHandle_t normalize,
                            TaskHandle_t ik);

    private:
        void taskFunction() override;

        static void _send(const char* fmt, ...);
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
        static constexpr uint16_t tjcCOLOR_GRAY   = 33808;   // grey
        static constexpr uint16_t tjcCOLOR_ORANGE  = 62756;   // orange

        // Joint angle limits (deg) for over-limit alarm
        // MotionPlanningTask.cpp Motor config (J1..J6 min/max)
        static constexpr float JOINT_LIMIT_MIN[6] = {-90.0f, -22.5f, -90.0f, -122.0f, -90.0f, -90.0f};
        static constexpr float JOINT_LIMIT_MAX[6] = { 90.0f, 125.0f,  90.0f,    0.0f,  90.0f,  90.0f};

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
        bool _loopMode    = false;   // LOOP (true) vs SINGLE (false) playback, from UI radio

        // Per-joint alarm latch (bit i = joint i was alarmed last frame),
        // used to fire the HMS/STOP message only on the rising edge.
        uint32_t _alarmPrev  = 0;
        bool     _alarmActive = false;   // any joint currently alarmed (STOP shown)

        FreeRTOS::Queue<sharedDatatype::IPCFeedback>     &_fdbk;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData> &_eeUIQueue;
};
