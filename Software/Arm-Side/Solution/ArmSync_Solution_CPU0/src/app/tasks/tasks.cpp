#include "tasks.h"

BlinkTask blinkTask;

// Task 1: UART receive (controller + Jetson)
UartRecvTask uartRecvTask(originalDataQueue);

// UI: TJC serial screen
UITask uiTask(IPCFeedbackQueue, eeUIQueue);

// Task 2: normalize + hand->coord + J5/pitch
NormalizeTask normalizeTask(originalDataQueue, armKPCoordsQueue,
                            handJointQueue, eeDataQueue, eeUIQueue, npuKpQueue);

// Task 4: Inverse Kinematics (analytic)
IKTask ikTask(armKPCoordsQueue, ikJointQueue);

// Task 5: Joint fusion (J1~J4 IK/NPU + J5 hand + J6 pitch)
FusionTask fusionTask(ikJointQueue, npuJointQueue, handJointQueue, fusedJointQueue, recQueue);

// Task 4.5: Ethos-U NPU inference (J3+J5 from keypoints) -> Fusion
//   input: npuKpQueue (ArmKPCoords from NormalizeTask, mirrored)
NPUTask npuTask(npuKpQueue, npuJointQueue);

// Task 5.5: Record / playback of fused joint angles
RecPlayTask recPlayTask(recQueue, replayQueue, fusedJointQueue);

// Task 6: Motion planning (joint angles -> motion plan)
//   live input: fusedJointQueue, playback input: replayQueue, PID feedback
MotionPlanningTask motionPlanningTask(fusedJointQueue, replayQueue, motionPlanQueue, pidFbQueue);

// Task 7: IPC to CPU1 (motion plan -> M33, feedback mirror to UI + PID)
CPUCommTask cpuCommTask(motionPlanQueue, eeDataQueue, IPCFeedbackQueue, pidFbQueue);
