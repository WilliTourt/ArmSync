#include "tasks.h"

BlinkTask blinkTask;

// Task 1: UART receive (controller + Jetson)
UartRecvTask uartRecvTask(originalDataQueue);

// Task 2: FK + fusion
NormalizeTask normalizeTask(originalDataQueue, armKPCoordsQueue, eeDataQueue);

// Task 4: Inverse Kinematics (FABRIK)
IKTask ikTask(armKPCoordsQueue, jointOutputQueue);
