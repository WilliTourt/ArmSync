#pragma once

#include <FreeRTOS/Queue.hpp>

#include "UartRecvTask.h"  // Task 1: Recv two attitude data

namespace sharedDatatype {

// Task 2 output
struct ArmKPCoords {
    float elbowCoord[3]; // [x, y, z] mm
    float wristCoord[3]; // [x, y, z] mm
    uint32_t timestamp;
};

struct EndEffectorData {
    float grip_percent;
    float pitch_percent;
    uint32_t timestamp;
};

// Task 4 output
struct JointOutput {
    float angles[6];      // J1~J6 in deg
    uint32_t timestamp;
    bool converged;
};

// M85 to M33 ctrl
struct IPCCtrlPacket {
    float    jointAngle[6];   // J1~J6 target degrees
    float    grip_percent;    // 0.0 ~ 100.0
    float    pitch_percent;
    uint32_t timestamp;       // FreeRTOS tick when written
};

// M33 feedback
struct IPCFeedback {
    float    jointAngle[6];   // J1~J6 actual degrees
    bool     isLockedRotor;
    bool     isGripperStuck;
    uint32_t timestamp;       // CPU1 counter when written
};

} // namespace sharedDatatype

extern FreeRTOS::Queue<UartRecvTask::TransmitData> originalDataQueue;
extern FreeRTOS::Queue<sharedDatatype::ArmKPCoords> armKPCoordsQueue;
extern FreeRTOS::Queue<sharedDatatype::EndEffectorData> eeDataQueue;
extern FreeRTOS::Queue<sharedDatatype::JointOutput> jointOutputQueue;
extern FreeRTOS::Queue<sharedDatatype::IPCFeedback> IPCFeedbackQueue;
