#include "queues.h"

FreeRTOS::Queue<UartRecvTask::TransmitData> originalDataQueue(4);
FreeRTOS::Queue<sharedDatatype::ArmKPCoords> armKPCoordsQueue(4);
FreeRTOS::Queue<sharedDatatype::EndEffectorData> eeDataQueue(2);
FreeRTOS::Queue<sharedDatatype::JointOutput> jointOutputQueue(4);
