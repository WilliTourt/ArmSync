#include "queues.h"

FreeRTOS::Queue<UartRecvTask::TransmitData> originalDataQueue(4);
FreeRTOS::Queue<sharedDatatype::ArmKPCoords> armKPCoordsQueue(4);
FreeRTOS::Queue<sharedDatatype::EndEffectorData> eeDataQueue(2);
FreeRTOS::Queue<sharedDatatype::EndEffectorData> eeUIQueue(2);
FreeRTOS::Queue<sharedDatatype::PitchData> pitchDataQueue(4);
FreeRTOS::Queue<sharedDatatype::JointAngleData> ikJointQueue(4);
FreeRTOS::Queue<sharedDatatype::JointAngleData> npuJointQueue(4);
FreeRTOS::Queue<sharedDatatype::JointAngleData> fusedJointQueue(4);
FreeRTOS::Queue<sharedDatatype::MotionPlanPacket> motionPlanQueue(4);
FreeRTOS::Queue<sharedDatatype::IPCFeedback> IPCFeedbackQueue(4);
