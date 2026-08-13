#pragma once

#include <FreeRTOS/Queue.hpp>

#include "UartRecvTask.h"  // Task 1: Recv two attitude data

namespace sharedDatatype {

// From Task 2 to Task 4 (NormalizeTask to IKTask)
struct ArmKPCoords {
    float elbowCoord[3]; // [x, y, z] mm
    float wristCoord[3]; // [x, y, z] mm
    uint32_t timestamp;
};

// From Task 2 to Task 6 (NormalizeTask to CPUCommTask)
struct EndEffectorData {
    float grip_percent;
    uint32_t timestamp;
};

// Pitch (from NormalizeTask to FusionTask) — maps to J6 later
struct PitchData {
    float pitch_percent;   // 0.0 ~ 100.0
    uint32_t timestamp;
};

// Joint angle data (generic carrier used by IK / NPU / fusion output).
// Invalid joints are expressed as NAN.
struct JointAngleData {
    float angles[6];      // J1~J6 in deg
    uint32_t timestamp;
};

// Motion plan result for one motor (MotionPlanningTask -> CPUCommTask)
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

// M85 to M33 ctrl (CPUCommTask)
struct IPCCtrlPacket {
    MotionPlanPacket motion_pkt;
    float grip_percent;    // 0.0 ~ 100.0
    uint32_t timestamp;    // FreeRTOS tick when written
};

// M33 feedback (CPUCommTask)
struct IPCFeedback {
    float    jointAngle[6];   // J1~J6 actual degrees
    float    gripAngle;       // gripper actual angle (deg)
    bool     isLockedRotor;
    bool     isGripperStuck;
    uint32_t timestamp;       // CPU1 counter when written
};

} // namespace sharedDatatype

extern FreeRTOS::Queue<UartRecvTask::TransmitData> originalDataQueue;
extern FreeRTOS::Queue<sharedDatatype::ArmKPCoords> armKPCoordsQueue;
extern FreeRTOS::Queue<sharedDatatype::EndEffectorData> eeDataQueue;
extern FreeRTOS::Queue<sharedDatatype::EndEffectorData> eeUIQueue;
extern FreeRTOS::Queue<sharedDatatype::PitchData> pitchDataQueue;
extern FreeRTOS::Queue<sharedDatatype::JointAngleData> ikJointQueue;
extern FreeRTOS::Queue<sharedDatatype::JointAngleData> npuJointQueue;
extern FreeRTOS::Queue<sharedDatatype::JointAngleData> fusedJointQueue;
extern FreeRTOS::Queue<sharedDatatype::MotionPlanPacket> motionPlanQueue;
extern FreeRTOS::Queue<sharedDatatype::IPCFeedback> IPCFeedbackQueue;
