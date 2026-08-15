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

        // UI-driven emergency-stop / home-zero flags, packed into the next ctrl
        // packet to M33. estop is a level (cleared by BTZ); btz is a one-shot.
        static void setEstop(bool estop) { _estopActive = estop; }
        static bool getEstop()            { return _estopActive; }
        static void setBtz(bool btz)      { _btzPending = btz; }
        static bool getBtz()              { return _btzPending; }

    private:
        void taskFunction() override;

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
        static bool _estopActive;   // UI estop level state
        static bool _btzPending;    // UI one-shot home-zero request
};
