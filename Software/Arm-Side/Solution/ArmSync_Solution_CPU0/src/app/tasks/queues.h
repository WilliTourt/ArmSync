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
    float angles[6];      // J1~J6 in radians
    uint32_t timestamp;
    bool converged;
};

} // namespace sharedDatatype



extern FreeRTOS::Queue<UartRecvTask::TransmitData> originalDataQueue;
extern FreeRTOS::Queue<sharedDatatype::ArmKPCoords> armKPCoordsQueue;
extern FreeRTOS::Queue<sharedDatatype::EndEffectorData> eeDataQueue;
extern FreeRTOS::Queue<sharedDatatype::JointOutput> jointOutputQueue;