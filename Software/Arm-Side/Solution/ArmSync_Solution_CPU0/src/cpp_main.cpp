#include "cpp_main.h"
#include "tasks.h"
#include "queues.h"
#include "ElegantDebug.h"

FSP_HEADER

// void SysTick_Handler(void);

void vSysTick(timer_callback_args_t *p_args) {
    (void)p_args;
    // SysTick_Handler();

    // ElegantDebug tick
    ElegantDebug::tick();
}


/* Static allocation stubs — required when configSUPPORT_STATIC_ALLOCATION = 1 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize);
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize);

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize) {
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

FSP_FOOTER



ElegantDebug dbg(&g_uart9, false, true);

void cpp_main() {

    fsp_err_t err = FSP_SUCCESS;
    err = R_AGT_Open(&agt_SysTick_ctrl, &agt_SysTick_cfg);
    assert(err == FSP_SUCCESS);

    err = R_AGT_Start(&agt_SysTick_ctrl);
    assert(err == FSP_SUCCESS);
    err = R_AGT_Enable(&agt_SysTick_ctrl);
    assert(err == FSP_SUCCESS);

    err = R_SCI_B_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
    assert(err == FSP_SUCCESS);
    err = R_SCI_B_UART_Open(&g_uart2_ctrl, &g_uart2_cfg);
    assert(err == FSP_SUCCESS);
    err = R_SCI_B_UART_Open(&g_uart9_ctrl, &g_uart9_cfg);
    assert(err == FSP_SUCCESS);

    R_IOPORT_PinWrite(&g_ioport_ctrl, LED_USER, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, LED_USER, BSP_IO_LEVEL_LOW);

    dbg.ok("UART0 UART9 Opened successfully\n");
    dbg.ok("SysTick initialized. Starting FreeRTOS scheduler...\n");

    FreeRTOS::Kernel::startScheduler();

    while (1);
}



/* UART0 Receive Callback for controller */
extern "C" void UART0_Callback(uart_callback_args_t *p_args) {
    UartRecvTask::uart0Callback(p_args);
}

/* UART2 Receive Callback for jetson nano */
extern "C" void UART2_Callback(uart_callback_args_t *p_args) {
    UartRecvTask::uart2Callback(p_args);
}

/* JLink OB */
extern "C" void UART9_Callback(uart_callback_args_t *p_args) {
    FSP_PARAMETER_NOT_USED(*p_args);
}
