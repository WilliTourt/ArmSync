#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "bsp_api.h"

FSP_HEADER

/* --- Basics --- */
#define configUSE_PREEMPTION                     1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION  0
#define configUSE_TICKLESS_IDLE                  0
#define configCPU_CLOCK_HZ                       (SystemCoreClock)
#define configTICK_RATE_HZ                       ((TickType_t)1000)
#define configMAX_PRIORITIES                     (56)
#define configMINIMAL_STACK_SIZE                 ((uint16_t)128)
#define configMAX_TASK_NAME_LEN                  (32)
#define configUSE_16_BIT_TICKS                   0
#define configUSE_TASK_NOTIFICATIONS             1
#define INCLUDE_xTaskGetHandle                   1

/* --- Memory --- */
#define configSUPPORT_STATIC_ALLOCATION          1
#define configSUPPORT_DYNAMIC_ALLOCATION         1
#define configTOTAL_HEAP_SIZE                    ((size_t)61440)
#define configAPPLICATION_ALLOCATED_HEAP         0

/* --- Hooks --- */
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0
#define configUSE_MALLOC_FAILED_HOOK             0

/* --- Sync primitives --- */
#define configUSE_MUTEXES                        1
#define configUSE_RECURSIVE_MUTEXES              1
#define configUSE_COUNTING_SEMAPHORES            1

/* --- Trace & Debug --- */
#define configUSE_TRACE_FACILITY                 1
#define configRECORD_STACK_HIGH_ADDRESS          1
#define configGENERATE_RUN_TIME_STATS            0
#define configUSE_STATS_FORMATTING_FUNCTIONS     0

/* --- Software timers --- */
#define configUSE_TIMERS                         1
#define configTIMER_TASK_PRIORITY                (2)
#define configTIMER_QUEUE_LENGTH                 10
#define configTIMER_TASK_STACK_DEPTH             256

/* --- Co-routines (not used) --- */
#define configUSE_CO_ROUTINES                    0
#define configMAX_CO_ROUTINE_PRIORITIES          (2)

/* --- Includes/excludes --- */
#define INCLUDE_vTaskPrioritySet             1
#define INCLUDE_uxTaskPriorityGet            1
#define INCLUDE_vTaskDelete                  1
#define INCLUDE_vTaskCleanUpResources        0
#define INCLUDE_vTaskSuspend                 1
#define INCLUDE_vTaskDelayUntil              1
#define INCLUDE_vTaskDelay                   1
#define INCLUDE_xTaskGetSchedulerState       1
#define INCLUDE_xTimerPendFunctionCall       1
#define INCLUDE_xQueueGetMutexHolder         1
#define INCLUDE_uxTaskGetStackHighWaterMark  1
#define INCLUDE_xTaskGetCurrentTaskHandle    1
#define INCLUDE_eTaskGetState                1
#define configQUEUE_REGISTRY_SIZE             8

/* --- NVIC priorities (RA8P1: 4-bit, 0-15) --- */
#define configPRIO_BITS                         __NVIC_PRIO_BITS
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY  ((1 << __NVIC_PRIO_BITS) - 1)
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 1
#define configKERNEL_INTERRUPT_PRIORITY          (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY     (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* --- Assert --- */
#define configASSERT(x)  if((x)==0) { taskDISABLE_INTERRUPTS(); for(;;); }

/* --- ISR handlers --- */
#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler

/* --- Message buffer --- */
#define configMESSAGE_BUFFER_LENGTH_TYPE  size_t

FSP_FOOTER

#endif /* FREERTOS_CONFIG_H */
