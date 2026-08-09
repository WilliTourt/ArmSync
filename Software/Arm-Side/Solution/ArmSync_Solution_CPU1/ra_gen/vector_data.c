/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = ipc_isr, /* IPC IRQ0 (CPU Mutual Interrupt 0) */
            [1] = sci_b_uart_rxi_isr, /* SCI1 RXI (Receive data full) */
            [2] = sci_b_uart_txi_isr, /* SCI1 TXI (Transmit data empty) */
            [3] = sci_b_uart_tei_isr, /* SCI1 TEI (Transmit end) */
            [4] = sci_b_uart_eri_isr, /* SCI1 ERI (Receive error) */
            [5] = agt_int_isr, /* AGT1 INT (AGT interrupt) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_IPC_IRQ0,GROUP0), /* IPC IRQ0 (CPU Mutual Interrupt 0) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_SCI1_RXI,GROUP1), /* SCI1 RXI (Receive data full) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_SCI1_TXI,GROUP2), /* SCI1 TXI (Transmit data empty) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_SCI1_TEI,GROUP3), /* SCI1 TEI (Transmit end) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_SCI1_ERI,GROUP4), /* SCI1 ERI (Receive error) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_AGT1_INT,GROUP5), /* AGT1 INT (AGT interrupt) */
        };
        #endif
        #endif
