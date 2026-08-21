#include "tasks.h"

BlinkTask blinkTask;

// Task 1: UART receive (controller + Jetson)
UartRecvTask uartRecvTask(originalDataQueue);

// UI: TJC serial screen
UITask uiTask(IPCFeedbackQueue, eeUIQueue);

// Task 2: normalize + hand->coord + J5/pitch
// (grip is fanned out to CPUCommTask + UITask + RecPlayTask for live/record)
NormalizeTask normalizeTask(originalDataQueue, armKPCoordsQueue,
                            handJointQueue, eeDataQueue, eeUIQueue, eeGripQueue);

// Task 4: Inverse Kinematics (analytic)
IKTask ikTask(armKPCoordsQueue, ikJointQueue);

// Task 5: Joint fusion (J1~J4 IK + J5 hand + J6 pitch), NPU filter folded in
FusionTask fusionTask(ikJointQueue, handJointQueue, fusedJointQueue, recQueue);

// Task 5.5: Record / playback of fused joint angles (+ grip)
RecPlayTask recPlayTask(recQueue, replayQueue, fusedJointQueue,
                        eeGripQueue, eeDataQueue);

// Task 6: Motion planning (joint angles -> motion plan)
//   live input: fusedJointQueue, playback input: replayQueue, PID feedback
MotionPlanningTask motionPlanningTask(fusedJointQueue, replayQueue, motionPlanQueue, pidFbQueue);

// Task 7: IPC to CPU1 (motion plan -> M33, feedback mirror to UI + PID)
CPUCommTask cpuCommTask(motionPlanQueue, eeDataQueue, IPCFeedbackQueue, pidFbQueue);
