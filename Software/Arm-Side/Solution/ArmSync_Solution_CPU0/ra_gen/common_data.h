/* generated common header file - do not edit */
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "rm_ethosu_api.h"
#include "rm_ethosu.h"
#include "arm_math.h"
#include "arm_nnfunctions.h"
#include "r_ioport.h"
#include "bsp_pin_cfg.h"
FSP_HEADER
#include "ethosu_driver.h"
extern struct ethosu_driver g_ethosu0;
extern rm_ethosu_instance_ctrl_t g_rm_ethosu0_ctrl;
extern const rm_ethosu_cfg_t g_rm_ethosu0_cfg;
extern const rm_ethosu_instance_t g_rm_ethosu0;
#ifndef NULL
void NULL(rm_ethosu_callback_args_t *p_arg);
#endif
#define IOPORT_CFG_NAME g_bsp_pin_cfg
#define IOPORT_CFG_OPEN R_IOPORT_Open
#define IOPORT_CFG_CTRL g_ioport_ctrl

/* IOPORT Instance */
extern const ioport_instance_t g_ioport;

/* IOPORT control structure. */
extern ioport_instance_ctrl_t g_ioport_ctrl;
void g_common_init(void);
FSP_FOOTER
#endif /* COMMON_DATA_H_ */
