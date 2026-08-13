#include "tasks.h"

BlinkTask blinkTask;

// Task 1: UART receive (controller + Jetson)
UartRecvTask uartRecvTask(originalDataQueue);

// Task 2: FK + spatial fusion
NormalizeTask normalizeTask(originalDataQueue, armKPCoordsQueue,
                            eeDataQueue, eeUIQueue, pitchDataQueue);

// Task 4: Inverse Kinematics (analytic)
IKTask ikTask(armKPCoordsQueue, ikJointQueue);

// Task 5: Joint fusion (IK + NPU + pitch->J6)
FusionTask fusionTask(ikJointQueue, npuJointQueue, pitchDataQueue, fusedJointQueue);

// Task 6: Motion planning (joint angles -> motion plan)
MotionPlanningTask motionPlanningTask(fusedJointQueue, motionPlanQueue);

// Task 7: IPC to CPU1 (motion plan -> M33)
CPUCommTask cpuCommTask(motionPlanQueue, eeDataQueue, IPCFeedbackQueue);

// UI: TJC serial screen
UITask uiTask(IPCFeedbackQueue, eeUIQueue);
