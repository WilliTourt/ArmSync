#pragma once

#include "hal_data.h"
#include <stdint.h>

/*** Shared-memory structs (copied from CPU0 project, queues.h) ***/

// Motion plan result for one motor
struct MotorCommand {
    uint8_t  dir;      // 0=CW, 1=CCW
    uint16_t rpm;      // speed
    uint8_t  acc;      // acceleration reg value (0~255)
    uint32_t pulse;    // pulse count
};

// Motion plan packet for all 6 motors
struct MotionPlanPacket {
    MotorCommand motors[6];
    uint32_t timestamp;
};

// M85 to M33 ctrl
struct IPCCtrlPacket {
    MotionPlanPacket motion_pkt;
    float grip_percent;    // 0.0 ~ 100.0
    uint32_t timestamp;    // CPU0 FreeRTOS tick when written
};

// M33 feedback
struct IPCFeedback {
    float    jointAngle[6];   // J1~J6 actual degrees
    float    gripAngle;       // gripper actual angle (deg)
    bool     isLockedRotor[6]; // per-joint stall flag
    bool     isGripperStuck;
    uint32_t timestamp;       // CPU1 counter when written
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
        uint32_t getTick() const { return _tick; }  // millisecond counter

        // reads control from CPU0 (motion plan + grip)
        bool getCtrlPacket(MotionPlanPacket &plan, float &grip_percent);
        void sendFeedback(const float angles_deg[6], float gripAngle, const bool locked[6], bool stuck);

        // Communication watchdog: true if no new ctrl packet from CPU0 within timeoutMs
        inline bool isWatchDogHungry(uint32_t timeoutMs) const {
            return (_tick - _wdgTick) > timeoutMs;
        }

        static void onCtrlReady();   // ISR hook

    private:
        IPCCtrlPacket              *_rx = nullptr;   // received from CPU0
        IPCFeedback                *_tx = nullptr;   // sent to CPU0
        bsp_ipc_semaphore_handle_t _lock = { .semaphore_num = 0 };

        uint32_t _lastRx = 0;   // last received control timestamp (from CPU0)
        uint32_t _tick   = 0;   // millisecond counter, incremented by tick()
        uint32_t _wdgTick = 0;  // tick value when last new ctrl packet armed the watchdog

        static volatile bool _ctrlPacketRdy;
};
