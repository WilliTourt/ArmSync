#pragma once

#include "queues.h"

#include "Blink.h"
#include "UartRecvTask.h"   // Task 1: Receive controller + Jetson
#include "normalizeTask.h"  // Task 2: FK + fusion
#include "IKTask.h"         // Task 4: FABRIK solver
#include "NPUTask.h"        // Task 4.5: Ethos-U inference (J3+J5)
#include "FusionTask.h"     // Task 5: IK/NPU joint fusion + pitch->J6
#include "RecPlayTask/RecPlayTask.h"     // Task 5.5: record/playback
#include "MotionPlanningTask.h" // Task 6: joint angles -> motion plan
#include "CPUCommTask.h"    // Task 7: IPC to CPU1
#include "UITask.h"         // Task: TJC screen updates

extern CPUCommTask cpuCommTask;
extern FusionTask fusionTask;
extern NPUTask npuTask;
extern RecPlayTask recPlayTask;
extern UITask uiTask;
