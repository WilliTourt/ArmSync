#include "CPUCommTask.h"
#include "ElegantDebug.h"

// NOTICE: CPU1 SHOULD ALSO DECLARE THESE
static sharedDatatype::IPCCtrlPacket ctrlData BSP_PLACE_IN_SECTION(".shared_ram");
static sharedDatatype::IPCFeedback   fbData   BSP_PLACE_IN_SECTION(".shared_ram");



volatile bool CPUCommTask::_fbReady = false;

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

    for (;;) {
        // Block on joint data (sync point for control loop frame)
        auto jointOut = _jointQueue.receive(portMAX_DELAY);
        if (!jointOut) continue;

        // Try to grab latest EE data (non-blocking, use last known if stale)
        auto eeData = _eeQueue.receive(0);
        if (eeData) {
            _latestEE = *eeData;
        }

        // Write joint + ee together, then send ONE msg
        while (FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&_lock));
        for (int i = 0; i < 6; i++) {
            _tx->jointAngle[i] = jointOut->angles[i];
        }
        _tx->grip_percent  = _latestEE.grip_percent;
        _tx->pitch_percent = _latestEE.pitch_percent;
        _tx->timestamp = xTaskGetTickCount();
        R_BSP_IpcSemaphoreGive(&_lock);

        R_IPC_MessageSend(&g_ipc0_ctrl, static_cast<uint32_t>(MsgToken::MSG_CTRL_READY));

        dbg.logWithType("IPC", COLOR_BLUE,
            "TX->M33: J1=%.1f J2=%.1f J3=%.1f J4=%.1f J5=%.1f J6=%.1f | grip=%.0f%% pitch=%.0f%%\n",
            jointOut->angles[0], jointOut->angles[1], jointOut->angles[2],
            jointOut->angles[3], jointOut->angles[4], jointOut->angles[5],
            _latestEE.grip_percent, _latestEE.pitch_percent);

        // ---- Feedback handling (unchanged) ----
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
