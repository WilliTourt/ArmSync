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

    // TickType_t lastSendTick = 0;
    for (;;) {

        // this->delay(pdMS_TO_TICKS(50));

        // Poll motion plan
        auto plan = _planQueue.receive(pdMS_TO_TICKS(33));
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
        bool estopLvl = false, btzPulse = false;
        CPUCommTask::snapshotCtrl(estopLvl, btzPulse);   // atomic read+clear btz
        bool needSend = (plan.has_value() || estopLvl || btzPulse);
        if (needSend) {

            while (FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&_lock));
            _tx->motion_pkt = _latestPlan;
            _tx->grip_percent = _latestEE.grip_percent;
            _tx->timestamp = now;
            _tx->estop = estopLvl;   // level: persists until BTZ clears
            _tx->btz   = btzPulse;   // pulse latched in the same atomic snapshot
            R_BSP_IpcSemaphoreGive(&_lock);

            R_IPC_MessageSend(&g_ipc0_ctrl, static_cast<uint32_t>(MsgToken::MSG_CTRL_READY));

            // Control frequency estimate
            _lastSendTicks[_sendIdx] = now;
            _sendIdx = (_sendIdx + 1) % FREQ_WINDOW;
            if (_sendIdx == 0) _sendFilled = true;

            if (_sendFilled && _uiHandle) {
                uint32_t oldest = _lastSendTicks[_sendIdx];  // index now points at oldest
                uint32_t span   = now - oldest;              // 5 sends = 4 periods
                uint32_t avgPeriod = span / (FREQ_WINDOW - 1);
                uint32_t hz = (avgPeriod > 0) ? (1000u / avgPeriod) : 0u;
                xTaskNotifyIndexed(_uiHandle, 1, hz, eSetValueWithOverwrite);
            }

            // dbg.logWithType("IPC", COLOR_BLUE,
                // "TX->M33: grip=%.0f%% | m2(dir%d r%d a%d p%lu)\n",
                // _latestEE.grip_percent,
                // _latestPlan.motors[2].dir, _latestPlan.motors[2].rpm,
                // _latestPlan.motors[2].acc, (unsigned long)_latestPlan.motors[2].pulse);
        }

        // ---- Feedback handling (always check) ----
        if (_fbReady) {
            _fbReady = false;

            while (FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&_lock));
            if (_rx->timestamp != _lastRx) {
                _lastRx = _rx->timestamp;
                _fb = *_rx;
                // Inverted motors (J2, J4, J6) report angle with reversed sign.
                // Fix it here at the single source so UI display and PID both
                // see physically-correct values (this also clears spurious
                // over-limit on J4 whose zero sits at its upper bound 0).
                _fb.jointAngle[1] = -_fb.jointAngle[1];   // J2 inverted
                _fb.jointAngle[3] = -_fb.jointAngle[3];   // J4 inverted
                _fb.jointAngle[5] = -_fb.jointAngle[5];   // J6 inverted
            }
            R_BSP_IpcSemaphoreGive(&_lock);

            // Mirror one copy to the UI and one to the PID loop so the
            // two consumers never steal each other's frames.
            _fbQueue.sendToBack(_fb, 0);     // UITask (display)
            // pidFbQueue is length-1: overwrite so the PID always sees the
            // newest feedback instead of dropping into a stale backlog.
            _pidFbQueue.overwrite(_fb);      // MotionPlanningTask (PID)
        }
    }
}
