#include "queues.h"

FreeRTOS::Queue<UartRecvTask::TransmitData> originalDataQueue(4);     // From UartRecvTask to NormalizeTask
FreeRTOS::Queue<sharedDatatype::Attitude6DOF> normalizedDataQueue(4);