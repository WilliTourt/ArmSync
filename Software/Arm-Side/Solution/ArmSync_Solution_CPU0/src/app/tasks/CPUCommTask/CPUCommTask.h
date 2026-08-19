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
                    FreeRTOS::Queue<sharedDatatype::IPCFeedback>      &fbQueue,
                    FreeRTOS::Queue<sharedDatatype::IPCFeedback>      &pidFbQueue)
            : Task(tskIDLE_PRIORITY + 3, 1536, "CPUComm"),
            _planQueue(planQueue), _eeQueue(eeQueue), _fbQueue(fbQueue),
            _pidFbQueue(pidFbQueue) {}

        static inline void onFbReady() {
            _fbReady = true;
        }

        // UI-driven estop / btz flags, packed into the next ctrl packet to M33.
        // These are read by CPUCommTask (send loop) and written by UITask from
        // another task, so guard the crossing with a critical section.
        static void setEstop(bool estop) {
            taskENTER_CRITICAL();
            _estopActive = estop;
            taskEXIT_CRITICAL();
        }
        static bool getEstop() {
            bool v;
            taskENTER_CRITICAL();
            v = _estopActive;
            taskEXIT_CRITICAL();
            return v;
        }
        static void setBtz(bool btz) {
            taskENTER_CRITICAL();
            _btzPending = btz;
            taskEXIT_CRITICAL();
        }
        static bool getBtz() {
            bool v;
            taskENTER_CRITICAL();
            v = _btzPending;
            taskEXIT_CRITICAL();
            return v;
        }

        // Atomically latch the current estop level + one-shot btz pulse into
        // out variables, clearing the btz latch in the same critical section so
        // a UI write between the read and clear can't get lost.
        static void snapshotCtrl(bool &estopOut, bool &btzOut) {
            taskENTER_CRITICAL();
            estopOut = _estopActive;
            btzOut   = _btzPending;
            _btzPending = false;   // one-shot, consumed
            taskEXIT_CRITICAL();
        }

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
        FreeRTOS::Queue<sharedDatatype::IPCFeedback>      &_fbQueue;      // -> UITask (display)
        FreeRTOS::Queue<sharedDatatype::IPCFeedback>      &_pidFbQueue;   // -> MotionPlanningTask (PID)

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
