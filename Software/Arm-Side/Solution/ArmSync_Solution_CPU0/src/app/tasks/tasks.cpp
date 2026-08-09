#include "tasks.h"

BlinkTask blinkTask;

// Task 1: UART receive (controller + Jetson)
UartRecvTask uartRecvTask(originalDataQueue);

// Task 2: FK + fusion
NormalizeTask normalizeTask(originalDataQueue, armKPCoordsQueue, eeDataQueue);

// Task 4: Inverse Kinematics (FABRIK)
IKTask ikTask(armKPCoordsQueue, jointOutputQueue);

// Task 3: IPC to CPU1
CPUCommTask cpuCommTask(jointOutputQueue, eeDataQueue, IPCFeedbackQueue);

// UI: TJC serial screen
UITask uiTask(IPCFeedbackQueue, eeDataQueue);
