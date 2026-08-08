#include "IKTask.h"
#include "ElegantDebug.h"
#include "ik/ik.h"

extern ElegantDebug dbg;

// Arm segment lengths (meters) — from your spec
// base→J2=125mm  J2→J4=246mm  J4→J6=192mm  J6→ee=116mm
static constexpr float L_J1 = 0.125f;
static constexpr float L_J2 = 0.246f;
static constexpr float L_J4 = 0.192f;
static constexpr float L_J6 = 0.116f;

bool IKTask::initSolver() {
    // Init the IK library
    if (IKAPI.init() != IK_OK) {
        dbg.error("IK: ik.init() failed\n");
        return false;
    }

    // Create FABRIK solver
    _solver = IKAPI.solver.create(IK_FABRIK);
    if (!_solver) {
        dbg.error("IK: solver.create() failed\n");
        return false;
    }
    _solver->max_iterations = 20;
    _solver->tolerance = 1e-3f;

    // Build bone chain: base → J2 → J4 → J6 → ee
    _base = _solver->node->create(0);
    _j2   = _solver->node->create_child(_base, 1);
    _j4   = _solver->node->create_child(_j2,   2);
    _j6   = _solver->node->create_child(_j4,   3);
    struct ik_node_t* ee = _solver->node->create_child(_j6, 4);

    // Set rest pose (arm pointing down, -Z)
    _base->position = IKAPI.vec3.vec3(0, 0, 0);
    _j2->position   = IKAPI.vec3.vec3(0, 0, -L_J1);
    _j4->position   = IKAPI.vec3.vec3(0, 0, -(L_J1 + L_J2));
    _j6->position   = IKAPI.vec3.vec3(0, 0, -(L_J1 + L_J2 + L_J4));
    ee->position    = IKAPI.vec3.vec3(0, 0, -(L_J1 + L_J2 + L_J4 + L_J6));

    // Attach effectors
    _efElbow = _solver->effector->create();
    _efWrist = _solver->effector->create();
    _solver->effector->attach(_efElbow, _j4);  // J4 = elbow
    _solver->effector->attach(_efWrist, _j6);  // J6 = wrist

    // Give tree to solver
    IKAPI.solver.set_tree(_solver, _base);
    IKAPI.solver.rebuild(_solver);

    dbg.ok("IK: FABRIK solver ready (chain: base→J2(%.0f)→J4(%.0f)→J6(%.0f)→ee(%.0f) mm)\n",
        L_J1*1000, L_J2*1000, L_J4*1000, L_J6*1000);
    return true;
}

void IKTask::taskFunction() {
    dbg.info("IKTask started.\n");

    if (!initSolver()) {
        dbg.error("IK: init failed, task suspended\n");
        this->suspend();
    }

    for (;;) {
        auto target = _inQueue.receive(portMAX_DELAY);
        if (!target) continue;

        // --- Set targets (mm → m) ---
        _efElbow->target_position.x = target->elbowCoord[0] / 1000.0f;
        _efElbow->target_position.y = target->elbowCoord[1] / 1000.0f;
        _efElbow->target_position.z = target->elbowCoord[2] / 1000.0f;

        _efWrist->target_position.x = target->wristCoord[0] / 1000.0f;
        _efWrist->target_position.y = target->wristCoord[1] / 1000.0f;
        _efWrist->target_position.z = target->wristCoord[2] / 1000.0f;

        // --- Solve ---
        int result = IKAPI.solver.solve(_solver);

        sharedDatatype::JointOutput out = {};
        out.timestamp = target->timestamp;
        out.converged = (result == 1);

        // TODO: extract J1~J6 angles from _j2/_j4/_j6 positions
        // For now, stub zeros
        for (int i = 0; i < 6; i++) out.angles[i] = 0.0f;

        if (out.converged) {
            dbg.logWithType("IK", COLOR_GREEN,
                "converged: J4=(%.3f,%.3f,%.3f) J6=(%.3f,%.3f,%.3f) m\n",
                _j4->position.x, _j4->position.y, _j4->position.z,
                _j6->position.x, _j6->position.y, _j6->position.z);
        } else {
            dbg.warning("IK: did not converge (target unreachable?)\n");
        }

        _outQueue.sendToBack(out, 0);
    }
}
