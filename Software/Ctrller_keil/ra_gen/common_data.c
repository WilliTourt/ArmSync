/* generated common source file - do not edit */
#include "common_data.h"
icu_instance_ctrl_t g_external_irq2_ctrl;

/** External IRQ extended configuration for ICU HAL driver */
const icu_extended_cfg_t g_external_irq2_ext_cfg =
{
    .filter_src         = EXTERNAL_IRQ_DIGITAL_FILTER_PCLK_DIV,
};

const external_irq_cfg_t g_external_irq2_cfg =
{
    .channel             = 2,
    .trigger             = EXTERNAL_IRQ_TRIG_RISING,
    .filter_enable       = false,
    .clock_source_div            = EXTERNAL_IRQ_CLOCK_SOURCE_DIV_64,
    .p_callback          = NULL,
    /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
    .p_context           = (void *) &NULL,
#endif
    .p_extend            = (void *)&g_external_irq2_ext_cfg,
    .ipl                 = (1),
#if defined(VECTOR_NUMBER_ICU_IRQ2)
    .irq                 = VECTOR_NUMBER_ICU_IRQ2,
#else
    .irq                 = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq2 =
{
    .p_ctrl        = &g_external_irq2_ctrl,
    .p_cfg         = &g_external_irq2_cfg,
    .p_api         = &g_external_irq_on_icu
};
icu_instance_ctrl_t g_external_irq9_ctrl;

/** External IRQ extended configuration for ICU HAL driver */
const icu_extended_cfg_t g_external_irq9_ext_cfg =
{
    .filter_src         = EXTERNAL_IRQ_DIGITAL_FILTER_PCLK_DIV,
};

const external_irq_cfg_t g_external_irq9_cfg =
{
    .channel             = 9,
    .trigger             = EXTERNAL_IRQ_TRIG_BOTH_EDGE,
    .filter_enable       = false,
    .clock_source_div            = EXTERNAL_IRQ_CLOCK_SOURCE_DIV_64,
    .p_callback          = g_external_irq_callback,
    /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
    .p_context           = (void *) &NULL,
#endif
    .p_extend            = (void *)&g_external_irq9_ext_cfg,
    .ipl                 = (0),
#if defined(VECTOR_NUMBER_ICU_IRQ9)
    .irq                 = VECTOR_NUMBER_ICU_IRQ9,
#else
    .irq                 = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq9 =
{
    .p_ctrl        = &g_external_irq9_ctrl,
    .p_cfg         = &g_external_irq9_cfg,
    .p_api         = &g_external_irq_on_icu
};
icu_instance_ctrl_t g_external_irq1_ctrl;

/** External IRQ extended configuration for ICU HAL driver */
const icu_extended_cfg_t g_external_irq1_ext_cfg =
{
    .filter_src         = EXTERNAL_IRQ_DIGITAL_FILTER_PCLK_DIV,
};

const external_irq_cfg_t g_external_irq1_cfg =
{
    .channel             = 1,
    .trigger             = EXTERNAL_IRQ_TRIG_RISING,
    .filter_enable       = false,
    .clock_source_div            = EXTERNAL_IRQ_CLOCK_SOURCE_DIV_64,
    .p_callback          = g_external_irq_callback,
    /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
    .p_context           = (void *) &NULL,
#endif
    .p_extend            = (void *)&g_external_irq1_ext_cfg,
    .ipl                 = (3),
#if defined(VECTOR_NUMBER_ICU_IRQ1)
    .irq                 = VECTOR_NUMBER_ICU_IRQ1,
#else
    .irq                 = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq1 =
{
    .p_ctrl        = &g_external_irq1_ctrl,
    .p_cfg         = &g_external_irq1_cfg,
    .p_api         = &g_external_irq_on_icu
};
icu_instance_ctrl_t g_external_irq0_ctrl;

/** External IRQ extended configuration for ICU HAL driver */
const icu_extended_cfg_t g_external_irq0_ext_cfg =
{
    .filter_src         = EXTERNAL_IRQ_DIGITAL_FILTER_PCLK_DIV,
};

const external_irq_cfg_t g_external_irq0_cfg =
{
    .channel             = 0,
    .trigger             = EXTERNAL_IRQ_TRIG_FALLING,
    .filter_enable       = false,
    .clock_source_div            = EXTERNAL_IRQ_CLOCK_SOURCE_DIV_64,
    .p_callback          = g_external_irq_callback,
    /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
    .p_context           = (void *) &NULL,
#endif
    .p_extend            = (void *)&g_external_irq0_ext_cfg,
    .ipl                 = (0),
#if defined(VECTOR_NUMBER_ICU_IRQ0)
    .irq                 = VECTOR_NUMBER_ICU_IRQ0,
#else
    .irq                 = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq0 =
{
    .p_ctrl        = &g_external_irq0_ctrl,
    .p_cfg         = &g_external_irq0_cfg,
    .p_api         = &g_external_irq_on_icu
};
ioport_instance_ctrl_t g_ioport_ctrl;
const ioport_instance_t g_ioport =
        {
            .p_api = &g_ioport_on_ioport,
            .p_ctrl = &g_ioport_ctrl,
            .p_cfg = &g_bsp_pin_cfg,
        };
void g_common_init(void) {
}
