#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "queues.h"

/**
 * @brief NPUTask - Ethos-U55 inference for J3/J5.
 *
 * Consumes ArmKPCoords (elbow/wrist) from NormalizeTask, accumulates a
 * 32-frame sliding window, runs the Ethos-U model, and publishes J3/J5 to
 * Fusion via npuJointQueue.
 *
 * NOTE: This is a *skeleton*. The actual Ethos-U model files + FSP NPU
 * configuration are NOT in this project yet. The inference section is gated
 * behind ARMSYNC_NPU_READY so this task compiles today; flip that macro on
 * after you add the model to RASC (see the notes in NPUTask.cpp).
 *
 * Output semantics (as agreed): only J3 and J5 are produced by the model;
 * J1/J2/J4/J6 are published as 0 so Fusion keeps using IK for those.
 */
class NPUTask : public FreeRTOS::Task {
    public:
        NPUTask(FreeRTOS::Queue<sharedDatatype::ArmKPCoords>    &inQueue,
                FreeRTOS::Queue<sharedDatatype::JointAngleData> &outQueue)
            : Task(tskIDLE_PRIORITY + 3, 2048, "NPU"),
              _inQueue(inQueue), _outQueue(outQueue) {}

    private:
        void taskFunction() override;

#ifdef ARMSYNC_NPU_READY
        static void initNPU();   // open Ethos-U, called once before scheduler
#endif

        FreeRTOS::Queue<sharedDatatype::ArmKPCoords>    &_inQueue;   // elbow/wrist from NormalizeTask
        FreeRTOS::Queue<sharedDatatype::JointAngleData> &_outQueue;  // -> Fusion (J3+J5)
};
