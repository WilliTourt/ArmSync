/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_sci_uart.h"
            #include "r_uart_api.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#include "r_canfd.h"
#include "r_can_api.h"
#include "r_qspi.h"
            #include "r_spi_flash_api.h"
FSP_HEADER
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart1;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_uart_instance_ctrl_t     g_uart1_ctrl;
            extern const uart_cfg_t g_uart1_cfg;
            extern const sci_uart_extended_cfg_t g_uart1_cfg_extend;

            #ifndef g_uart1_callback
            void g_uart1_callback(uart_callback_args_t * p_args);
            #endif
/** Timer on GPT Instance. */
extern const timer_instance_t claw_ctrl;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t claw_ctrl_ctrl;
extern const timer_cfg_t claw_ctrl_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** CANFD on CANFD Instance. */
extern const can_instance_t g_canfd0;
/** Access the CANFD instance using these structures when calling API functions directly (::p_api is not used). */
extern canfd_instance_ctrl_t g_canfd0_ctrl;
extern const can_cfg_t g_canfd0_cfg;
extern const canfd_extended_cfg_t g_canfd0_cfg_extend;

#ifndef canfd0_callback
void canfd0_callback(can_callback_args_t * p_args);
#endif

/* Global configuration (referenced by all instances) */
extern canfd_global_cfg_t g_canfd_global_cfg;
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart0;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_uart_instance_ctrl_t     g_uart0_ctrl;
            extern const uart_cfg_t g_uart0_cfg;
            extern const sci_uart_extended_cfg_t g_uart0_cfg_extend;

            #ifndef g_uart0_callback
            void g_uart0_callback(uart_callback_args_t * p_args);
            #endif
extern const spi_flash_instance_t g_qspi0_flash;
            extern qspi_instance_ctrl_t g_qspi0_flash_ctrl;
            extern const spi_flash_cfg_t g_qspi0_flash_cfg;
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
