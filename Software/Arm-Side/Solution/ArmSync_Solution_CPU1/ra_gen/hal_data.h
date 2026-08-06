/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_canfd.h"
#include "r_can_api.h"
FSP_HEADER
/** CANFD on CANFD Instance. */
extern const can_instance_t g_canfd1;
/** Access the CANFD instance using these structures when calling API functions directly (::p_api is not used). */
extern canfd_instance_ctrl_t g_canfd1_ctrl;
extern const can_cfg_t g_canfd1_cfg;
extern const canfd_extended_cfg_t g_canfd1_cfg_extend;

#ifndef CANFD1_Callback
void CANFD1_Callback(can_callback_args_t *p_args);
#endif

/* Global configuration (referenced by all instances) */
extern canfd_global_cfg_t g_canfd_global_cfg;
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
