#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "hal_data.h"
#include "queues.h"

class CPUCommTask : public FreeRTOS::Task {
    public:

        enum class MsgToken : uint32_t {
            MSG_CTRL_READY = 0xA1u,
            MSG_FB_READY   = 0xA2u,
        };

        CPUCommTask(FreeRTOS::Queue<sharedDatatype::MotionPlanPacket> &planQueue,
                    FreeRTOS::Queue<sharedDatatype::EndEffectorData>  &eeQueue,
                    FreeRTOS::Queue<sharedDatatype::IPCFeedback>      &fbQueue)
            : Task(tskIDLE_PRIORITY + 3, 1024, "CPUComm"),
            _planQueue(planQueue), _eeQueue(eeQueue), _fbQueue(fbQueue) {}

        static inline void onFbReady() {
            _fbReady = true;
        }

        // UI-driven estop / btz flags, packed into the next ctrl packet to M33
        static void setEstop(bool estop) { _estopActive = estop; }
        static bool getEstop()            { return _estopActive; }
        static void setBtz(bool btz)      { _btzPending = btz; }
        static bool getBtz()              { return _btzPending; }

        static void setUIHandle(TaskHandle_t h) { _uiHandle = h; }

    private:
        void taskFunction() override;

        // Sliding window of the last N send ticks for control-frequency estimate
        static constexpr uint8_t FREQ_WINDOW = 5;
        uint32_t _lastSendTicks[FREQ_WINDOW] = {};
        uint8_t  _sendIdx  = 0;
        bool     _sendFilled = false;

        FreeRTOS::Queue<sharedDatatype::MotionPlanPacket> &_planQueue;
        FreeRTOS::Queue<sharedDatatype::EndEffectorData>  &_eeQueue;
        FreeRTOS::Queue<sharedDatatype::IPCFeedback>      &_fbQueue;

        sharedDatatype::IPCCtrlPacket *_tx = nullptr;
        sharedDatatype::IPCFeedback   *_rx = nullptr;
        bsp_ipc_semaphore_handle_t    _lock = { .semaphore_num = 0 };

        sharedDatatype::IPCFeedback   _fb;
        sharedDatatype::MotionPlanPacket _latestPlan = {};
        sharedDatatype::EndEffectorData  _latestEE = {};
        uint32_t _lastRx = 0;

        static volatile bool _fbReady;
        static bool _estopActive;        // UI estop level state
        static bool _btzPending;         // UI one-shot home-zero request
        static TaskHandle_t _uiHandle;   // UITask, for freq notification
};
