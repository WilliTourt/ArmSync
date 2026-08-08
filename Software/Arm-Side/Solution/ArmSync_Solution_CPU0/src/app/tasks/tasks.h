#pragma once

#include "queues.h"

#include "Blink.h"
#include "UartRecvTask.h"   // Task 1: Receive controller + Jetson
#include "normalizeTask.h"  // Task 2: FK + fusion
#include "IKTask.h"         // Task 4: FABRIK solver
