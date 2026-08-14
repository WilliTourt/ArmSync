#include "ipc.h"


static IPCCtrlPacket ctrlData BSP_PLACE_IN_SECTION(".shared_ram");   // from CPU0
static IPCFeedback   fbData   BSP_PLACE_IN_SECTION(".shared_ram");   // to CPU0



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
    _tick++;   // 1ms system clock (kept lightweight; watchdog handled in main loop)
}

bool IPC::getCtrlPacket(MotionPlanPacket &plan, float &grip_percent) {
    if (!_ctrlPacketRdy) return false;
    _ctrlPacketRdy = false;

    while (FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&_lock));

    if (_rx->timestamp == _lastRx) { // Not a new message, return false
        R_BSP_IpcSemaphoreGive(&_lock);
        return false;
    }
    _lastRx = _rx->timestamp;
    _wdgTick = _tick;   // rearm watchdog on every fresh ctrl packet

    plan = _rx->motion_pkt;
    grip_percent = _rx->grip_percent;

    R_BSP_IpcSemaphoreGive(&_lock);
    return true;
}

void IPC::sendFeedback(const float angles_deg[6], float gripAngle, const bool locked[6], bool stuck) {
    while (FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&_lock));

    for (int i = 0; i < 6; i++) {
        _tx->jointAngle[i] = angles_deg[i];
        _tx->isLockedRotor[i] = locked[i];
    }
    _tx->gripAngle      = gripAngle;
    _tx->isGripperStuck = stuck;
    _tx->timestamp = _tick;

    R_BSP_IpcSemaphoreGive(&_lock);

    R_IPC_MessageSend(&g_ipc0_ctrl, static_cast<uint32_t>(MsgToken::MSG_FB_READY));
}
