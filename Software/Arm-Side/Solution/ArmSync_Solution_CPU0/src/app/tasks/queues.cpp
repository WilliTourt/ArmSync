#include "queues.h"

FreeRTOS::Queue<UartRecvTask::TransmitData> originalDataQueue(4);
FreeRTOS::Queue<sharedDatatype::ArmKPCoords> armKPCoordsQueue(4);
FreeRTOS::Queue<sharedDatatype::EndEffectorData> eeDataQueue(2);
FreeRTOS::Queue<sharedDatatype::EndEffectorData> eeUIQueue(2);
FreeRTOS::Queue<sharedDatatype::HandJointData> handJointQueue(4);
FreeRTOS::Queue<sharedDatatype::JointAngleData> ikJointQueue(4);
FreeRTOS::Queue<sharedDatatype::JointAngleData> npuJointQueue(4);
FreeRTOS::Queue<sharedDatatype::JointAngleData> fusedJointQueue(4);
FreeRTOS::Queue<sharedDatatype::JointAngleData> recQueue(8);   // record tap (30Hz, give margin)
FreeRTOS::Queue<sharedDatatype::JointAngleData> replayQueue(8); // playback to MotionPlanningTask
FreeRTOS::Queue<sharedDatatype::MotionPlanPacket> motionPlanQueue(4);
FreeRTOS::Queue<sharedDatatype::IPCFeedback> IPCFeedbackQueue(4);
