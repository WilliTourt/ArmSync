/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_sci_b_uart.h"
#include "r_uart_api.h"
#include "r_agt.h"
#include "r_timer_api.h"
FSP_HEADER
/** UART on SCI Instance. */
extern const uart_instance_t g_uart3;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_b_uart_instance_ctrl_t g_uart3_ctrl;
extern const uart_cfg_t g_uart3_cfg;
extern const sci_b_uart_extended_cfg_t g_uart3_cfg_extend;

#ifndef UART3_Callback
void UART3_Callback(uart_callback_args_t *p_args);
#endif
/** UART on SCI Instance. */
extern const uart_instance_t g_uart9;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_b_uart_instance_ctrl_t g_uart9_ctrl;
extern const uart_cfg_t g_uart9_cfg;
extern const sci_b_uart_extended_cfg_t g_uart9_cfg_extend;

#ifndef UART9_Callback
void UART9_Callback(uart_callback_args_t *p_args);
#endif
/** UART on SCI Instance. */
extern const uart_instance_t g_uart2;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_b_uart_instance_ctrl_t g_uart2_ctrl;
extern const uart_cfg_t g_uart2_cfg;
extern const sci_b_uart_extended_cfg_t g_uart2_cfg_extend;

#ifndef UART2_Callback
void UART2_Callback(uart_callback_args_t *p_args);
#endif
/** UART on SCI Instance. */
extern const uart_instance_t g_uart0;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_b_uart_instance_ctrl_t g_uart0_ctrl;
extern const uart_cfg_t g_uart0_cfg;
extern const sci_b_uart_extended_cfg_t g_uart0_cfg_extend;

#ifndef UART0_Callback
void UART0_Callback(uart_callback_args_t *p_args);
#endif
/** AGT Timer Instance */
extern const timer_instance_t agt_SysTick;

/** Access the AGT instance using these structures when calling API functions directly (::p_api is not used). */
extern agt_instance_ctrl_t agt_SysTick_ctrl;
extern const timer_cfg_t agt_SysTick_cfg;

#ifndef vSysTick
void vSysTick(timer_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
