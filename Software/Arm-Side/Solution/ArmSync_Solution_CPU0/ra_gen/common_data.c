/* generated common source file - do not edit */
#include "common_data.h"
#include "ethosu_driver.h"
struct ethosu_driver g_ethosu0;
rm_ethosu_extended_cfg_t g_rm_ethosu0_ext_cfg = { .p_dev = &g_ethosu0, };

rm_ethosu_instance_ctrl_t g_rm_ethosu0_ctrl = { .p_ext_cfg =
		&g_rm_ethosu0_ext_cfg, };

const rm_ethosu_cfg_t g_rm_ethosu0_cfg = { .secure_enable = 1,
		.privilege_enable = 1,
#if defined(VECTOR_NUMBER_NPU_IRQ)
            .irq             = VECTOR_NUMBER_NPU_IRQ,
#else
		.irq = FSP_INVALID_VECTOR,
#endif
		.ipl = (12), .p_callback = NULL, .p_context = NULL, };

const rm_ethosu_instance_t g_rm_ethosu0 = { .p_ctrl = &g_rm_ethosu0_ctrl,
		.p_cfg = &g_rm_ethosu0_cfg, .p_api = &g_rm_ethosu_on_npu, };
ioport_instance_ctrl_t g_ioport_ctrl;
const ioport_instance_t g_ioport = { .p_api = &g_ioport_on_ioport, .p_ctrl =
		&g_ioport_ctrl, .p_cfg = &g_bsp_pin_cfg, };
void g_common_init(void) {
}
