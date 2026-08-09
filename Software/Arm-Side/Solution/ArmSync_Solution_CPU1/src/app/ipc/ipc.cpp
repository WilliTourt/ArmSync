#include "ipc.h"


static IPCCtrlPacket ctrlData BSP_PLACE_IN_SECTION(".shared_ram");   // from CPU0
static IPCFeedback   fbData   BSP_PLACE_IN_SECTION(".shared_ram");   // to CPU0

// IPC ISR callback
extern "C" void IPC0_Callback(ipc_callback_args_t *p_args) {
    if (p_args->event == IPC_EVENT_MESSAGE_RECEIVED) {
        if (p_args->message == static_cast<uint32_t>(MsgToken::MSG_CTRL_READY)) {
            IPC::onCtrlReady();
        }
    }
}

volatile bool IPC::_ctrlPacketRdy = false;

void IPC::onCtrlReady() {
    _ctrlPacketRdy = true;
}



void IPC::init() {
    _rx = &ctrlData;
    _tx = &fbData;

    R_IPC_Open(&g_ipc0_ctrl, &g_ipc0_cfg);
}

void IPC::tick() {
    _tick++;

    // TODO: WATCHDOG - if no ctrl update for > 500ms, arm.stop()
}

bool IPC::getCtrlPacket(ArmTarget &arm, GripTarget &grip) {
    if (!_ctrlPacketRdy) return false;
    _ctrlPacketRdy = false;

    while (FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&_lock));

    if (_rx->timestamp == _lastRx) { // Not a new message, return false
        R_BSP_IpcSemaphoreGive(&_lock);
        return false;
    }
    _lastRx = _rx->timestamp;

    for (int i = 0; i < 6; i++) {
        arm.jointAngle[i] = _rx->jointAngle[i];
    }
    arm.pitch_percent = _rx->pitch_percent;
    grip.ratio = _rx->grip_percent;

    R_BSP_IpcSemaphoreGive(&_lock);
    return true;
}

void IPC::sendFeedback(const float angles_deg[6], bool locked, bool stuck) {
    while (FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&_lock));

    for (int i = 0; i < 6; i++) {
        _tx->jointAngle[i] = angles_deg[i];
    }
    _tx->isLockedRotor  = locked;
    _tx->isGripperStuck = stuck;
    _tx->timestamp = _tick;

    R_BSP_IpcSemaphoreGive(&_lock);

    R_IPC_MessageSend(&g_ipc0_ctrl, static_cast<uint32_t>(MsgToken::MSG_FB_READY));
}
