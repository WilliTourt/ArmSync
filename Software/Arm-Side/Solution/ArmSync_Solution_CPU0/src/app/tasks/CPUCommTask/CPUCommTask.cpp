#include "CPUCommTask.h"

// NOTICE: CPU1 SHOULD ALSO DECLARE THESE
static sharedDatatype::IPCCtrlPacket ctrlData BSP_PLACE_IN_SECTION(".shared_ram");
static sharedDatatype::IPCFeedback   fbData   BSP_PLACE_IN_SECTION(".shared_ram");



volatile bool CPUCommTask::_fbReady = false;

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
        auto jointOut = _jointQueue.receive(pdMS_TO_TICKS(30));
        if (jointOut) {
            while (FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&_lock));
            for (int i = 0; i < 6; i++) {
                _tx->jointAngle[i] = jointOut->angles[i];
            }
            _tx->timestamp = xTaskGetTickCount();

            R_BSP_IpcSemaphoreGive(&_lock);
            R_IPC_MessageSend(&g_ipc0_ctrl, static_cast<uint32_t>(MsgToken::MSG_CTRL_READY));
        }

        auto eeData = _eeQueue.receive(pdMS_TO_TICKS(30));
        if (eeData) {
            while (FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&_lock));
            _tx->grip_percent  = eeData->grip_percent;
            _tx->pitch_percent = eeData->pitch_percent;
            _tx->timestamp = xTaskGetTickCount();

            R_BSP_IpcSemaphoreGive(&_lock);
            R_IPC_MessageSend(&g_ipc0_ctrl, static_cast<uint32_t>(MsgToken::MSG_CTRL_READY));
        }

        if (_fbReady) {
            _fbReady = false;

            while (FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&_lock));
            if (_rx->timestamp != _lastRx) {  // Is a new message
                _lastRx = _rx->timestamp;
                _fb = *_rx;
            }
            R_BSP_IpcSemaphoreGive(&_lock);

            _fbQueue.sendToBack(_fb, 0);
        }
    }
}
