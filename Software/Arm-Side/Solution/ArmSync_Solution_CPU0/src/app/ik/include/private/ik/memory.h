#ifndef MEMORY_H
#define MEMORY_H

#include "ik/config.h"
#include <stddef.h>   // size_t

/* FreeRTOS heap allocation */
#ifdef __cplusplus
extern "C" {
#endif
void* pvPortMalloc(size_t size);
void  vPortFree(void* ptr);
#ifdef __cplusplus
}
#endif

#define MALLOC  pvPortMalloc
#define FREE    vPortFree

C_BEGIN

IK_PRIVATE_API void ik_memory_init(void);
IK_PRIVATE_API uintptr_t ik_memory_deinit(void);

C_END

#endif
