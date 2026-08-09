#pragma once

#include "hal_data.h"
#include <stdint.h>

/*** Shared-memory structs (copied from CPU0 project, queues.h) ***/
// M85 to M33 ctrl
struct IPCCtrlPacket {
    float    jointAngle[6];   // J1~J6 target degrees
    float    grip_percent;    // 0.0 ~ 100.0
    float    pitch_percent;
    uint32_t timestamp;       // CPU0 FreeRTOS tick when written
};

// M33 feedback
struct IPCFeedback {
    float    jointAngle[6];   // J1~J6 actual degrees
    bool     isLockedRotor;
    bool     isGripperStuck;
    uint32_t timestamp;       // CPU1 counter when written
};

/*** Split targets for consumer classes ***/
struct ArmTarget {
    float jointAngle[6];   // J1~J6 target degrees
    float pitch_percent;   // wrist pitch
};

struct GripTarget {
    float ratio;           // 0.0 ~ 100.0
};

/*** IPC message tokens (copied from CPU0 project, CPUCommTask.h) ***/
enum class MsgToken : uint32_t {
    MSG_CTRL_READY = 0xA1u,
    MSG_FB_READY   = 0xA2u,
};



class IPC {
    public:
        void init();
        void tick();                 // called from AGT ISR, 1ms

        // reads control from CPU0, splits into ArmTarget + GripTarget
        bool getCtrlPacket(ArmTarget &arm, GripTarget &grip);
        void sendFeedback(const float angles_deg[6], bool locked, bool stuck);

        static void onCtrlReady();   // ISR hook

    private:
        IPCCtrlPacket              *_rx = nullptr;   // received from CPU0
        IPCFeedback                *_tx = nullptr;   // sent to CPU0
        bsp_ipc_semaphore_handle_t _lock = { .semaphore_num = 0 };

        uint32_t _lastRx = 0;   // last received control timestamp
        uint32_t _tick   = 0;   // millisecond counter, incremented by tick()

        static volatile bool _ctrlPacketRdy;
};
