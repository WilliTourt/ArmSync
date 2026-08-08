#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "queues.h"

class IKTask : public FreeRTOS::Task {
public:
    IKTask(FreeRTOS::Queue<sharedDatatype::ArmKPCoords> &inQueue,
           FreeRTOS::Queue<sharedDatatype::JointOutput>  &outQueue)
        : Task(tskIDLE_PRIORITY + 3, 2048, "IK"),
          _inQueue(inQueue), _outQueue(outQueue) {}

private:
    void taskFunction() override;
    bool initSolver();

    FreeRTOS::Queue<sharedDatatype::ArmKPCoords> &_inQueue;
    FreeRTOS::Queue<sharedDatatype::JointOutput>  &_outQueue;

    struct ik_solver_t*   _solver   = nullptr;
    struct ik_node_t*     _base     = nullptr;
    struct ik_node_t*     _j2       = nullptr;
    struct ik_node_t*     _j4       = nullptr;
    struct ik_node_t*     _j6       = nullptr;
    struct ik_effector_t* _efElbow  = nullptr;
    struct ik_effector_t* _efWrist  = nullptr;
};
