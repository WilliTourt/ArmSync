#pragma once

#include <FreeRTOS/Queue.hpp>

#include "UartRecvTask.h"  // Task 1: Recv two attitude data

namespace sharedDatatype {

// Task 2 output
struct Attitude6DOF {
    float upper_pitch;   // 上臂俯仰 (rad)
    float upper_roll;    // 上臂横滚 (rad)
    float upper_yaw;     // 上臂偏航 (rad)
    float rel_pitch;     // 小臂相对俯仰 (rad)
    float rel_roll;      // 小臂相对横滚 (rad)
    float rel_yaw;       // 小臂相对偏航 (rad)
    float grip_percent;
    float pitch_percent;
    uint32_t timestamp;
};

} // namespace sharedDatatype



extern FreeRTOS::Queue<UartRecvTask::TransmitData> originalDataQueue;
extern FreeRTOS::Queue<sharedDatatype::Attitude6DOF> normalizedDataQueue;
