/* Auto-generated + manual fix */
#ifndef IK_VTABLE_NODE_FABRIK_H
#define IK_VTABLE_NODE_FABRIK_H

#include "ik/config.h"
#include "ik/node.h"

C_BEGIN

#define IK_NODE_FABRIK_HEAD                                                   \
    IK_NODE_HEAD                                                              \
    union                                                                     \
    {                                                                         \
        struct                                                                \
        {                                                                     \
            ik_quat_t initial_rotation;                                       \
            ik_vec3_t initial_position;                                       \
        };                                                                    \
        ikreal_t initial_transform[7];                                        \
    };

struct ik_node_FABRIK_t
{
    IK_NODE_FABRIK_HEAD
};

/* vtable declarations */
IK_PRIVATE_API struct ik_node_t* ik_node_FABRIK_create(uint32_t guid);
IK_PRIVATE_API ikret_t ik_node_FABRIK_construct(struct ik_node_t* node, uint32_t guid);

#define IK_NODE_FABRIK_IMPL \
    ik_node_FABRIK_create, \
    ik_node_FABRIK_construct, \
    ik_node_base_destruct, \
    ik_node_base_destroy, \
    ik_node_base_create_child, \
    ik_node_base_add_child, \
    ik_node_base_unlink, \
    ik_node_base_find_child, \
    ik_node_base_duplicate, \
    ik_node_base_dump_to_dot

C_END

#endif
