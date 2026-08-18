#include "CPUCommTask.h"
#include "ElegantDebug.h"

// NOTICE: CPU1 SHOULD ALSO DECLARE THESE
static sharedDatatype::IPCCtrlPacket ctrlData BSP_PLACE_IN_SECTION(".shared_ram");
static sharedDatatype::IPCFeedback   fbData   BSP_PLACE_IN_SECTION(".shared_ram");



volatile bool CPUCommTask::_fbReady = false;
bool CPUCommTask::_estopActive = true;   // Default true to prevent powerup accidents
bool CPUCommTask::_btzPending  = false;
TaskHandle_t CPUCommTask::_uiHandle = nullptr;   // UITask (freq notification)

extern ElegantDebug dbg;

extern "C" void IPC0_Callback(ipc_callback_args_t *p_args) {
    if (p_args->event == IPC_EVENT_MESSAGE_RECEIVED) {
        if (p_args->message == static_cast<uint32_t>(CPUCommTask::MsgToken::MSG_FB_READY)) {
            CPUCommTask::onFbReady();
        }
    }
}

void CPUCommTask::taskFunction() {

    _tx = &ctrlData;
    _rx = &fbData;
    memset(_tx, 0, sizeof(sharedDatatype::IPCCtrlPacket));
    memset(_rx, 0, sizeof(sharedDatatype::IPCFeedback));

    R_IPC_Open(&g_ipc0_ctrl, &g_ipc0_cfg);
    dbg.ok("CPUCommTask: IPC0 opened, task started.\n");

    TickType_t lastSendTick = 0;
    for (;;) {

        this->delay(pdMS_TO_TICKS(100));

        // ---- Poll motion plan with short timeout (keep latest) ----
        auto plan = _planQueue.receive(pdMS_TO_TICKS(10));
        if (plan) {
            _latestPlan = *plan;
        }

        // Always grab latest EE data (non-blocking)
        auto eeData = _eeQueue.receive(0);
        if (eeData) {
            _latestEE = *eeData;
        }

        // Send IPC periodically
        TickType_t now = xTaskGetTickCount();
        if ((now - lastSendTick) >= pdMS_TO_TICKS(100)) {
            lastSendTick = now;

            while (FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&_lock));
            _tx->motion_pkt = _latestPlan;
            _tx->grip_percent = _latestEE.grip_percent;
            _tx->timestamp = now;
            _tx->estop = _estopActive;   // level: persists until BTZ clears
            _tx->btz   = _btzPending;    // pulse: clear after sending
            _btzPending = false;         // one-shot, latch consumed
            R_BSP_IpcSemaphoreGive(&_lock);

            R_IPC_MessageSend(&g_ipc0_ctrl, static_cast<uint32_t>(MsgToken::MSG_CTRL_READY));

            // Control frequency estimate
            _lastSendTicks[_sendIdx] = now;
            _sendIdx = (_sendIdx + 1) % FREQ_WINDOW;
            if (_sendIdx == 0) _sendFilled = true;

            if (_sendFilled && _uiHandle) {
                uint32_t oldest = _lastSendTicks[_sendIdx];  // index now points at oldest
                uint32_t span   = now - oldest;              // 5 sends = 4 periods
                uint32_t avgPer = span / (FREQ_WINDOW - 1);  // avg period (ms)
                uint32_t freqHz = (avgPer > 0) ? (1000u / avgPer) : 0u;
                xTaskNotifyIndexed(_uiHandle, 1, freqHz, eSetValueWithOverwrite);
            }

            // dbg.logWithType("IPC", COLOR_BLUE,
            //     "TX->M33: grip=%.0f%% | m0(dir%d r%d a%d p%lu)\n",
            //     _latestEE.grip_percent,
            //     _latestPlan.motors[0].dir, _latestPlan.motors[0].rpm,
            //     _latestPlan.motors[0].acc, (unsigned long)_latestPlan.motors[0].pulse);
        }

        // ---- Feedback handling (always check) ----
        if (_fbReady) {
            _fbReady = false;

            while (FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&_lock));
            if (_rx->timestamp != _lastRx) {
                _lastRx = _rx->timestamp;
                _fb = *_rx;
            }
            R_BSP_IpcSemaphoreGive(&_lock);

            _fbQueue.sendToBack(_fb, 0);
        }
    }
}
