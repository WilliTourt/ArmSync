#ifndef IK_VTABLE_SOLVER_FABRIK_H
#define IK_VTABLE_SOLVER_FABRIK_H
#include "ik/solver.h"
#include "ik/solver_base.h"
C_BEGIN
/* FABRIK overrides: type_size, construct, destruct, solve */
IK_PRIVATE_API uintptr_t ik_solver_FABRIK_type_size(void);
IK_PRIVATE_API ikret_t ik_solver_FABRIK_construct(struct ik_solver_t* solver);
IK_PRIVATE_API void ik_solver_FABRIK_destruct(struct ik_solver_t* solver);
IK_PRIVATE_API ikret_t ik_solver_FABRIK_solve(struct ik_solver_t* solver);
#define IK_SOLVER_FABRIK_IMPL \
    ik_solver_FABRIK_type_size, ik_solver_base_create, ik_solver_base_destroy, \
    ik_solver_FABRIK_construct, ik_solver_FABRIK_destruct, \
    ik_solver_base_rebuild, ik_solver_base_update_distances, \
    ik_solver_FABRIK_solve, ik_solver_base_set_tree, ik_solver_base_unlink_tree, \
    ik_solver_base_destroy_tree, ik_solver_base_iterate_all_nodes, \
    ik_solver_base_iterate_affected_nodes, ik_solver_base_iterate_base_nodes
C_END
#endif
