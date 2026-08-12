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
            [6] = sci_b_uart_rxi_isr, /* SCI5 RXI (Receive data full) */
            [7] = sci_b_uart_txi_isr, /* SCI5 TXI (Transmit data empty) */
            [8] = sci_b_uart_tei_isr, /* SCI5 TEI (Transmit end) */
            [9] = sci_b_uart_eri_isr, /* SCI5 ERI (Receive error) */
            [10] = sci_b_uart_rxi_isr, /* SCI8 RXI (Receive data full) */
            [11] = sci_b_uart_txi_isr, /* SCI8 TXI (Transmit data empty) */
            [12] = sci_b_uart_tei_isr, /* SCI8 TEI (Transmit end) */
            [13] = sci_b_uart_eri_isr, /* SCI8 ERI (Receive error) */
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
            [6] = BSP_PRV_VECT_ENUM(EVENT_SCI5_RXI,GROUP6), /* SCI5 RXI (Receive data full) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_SCI5_TXI,GROUP7), /* SCI5 TXI (Transmit data empty) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_SCI5_TEI,GROUP0), /* SCI5 TEI (Transmit end) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_SCI5_ERI,GROUP1), /* SCI5 ERI (Receive error) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_SCI8_RXI,GROUP2), /* SCI8 RXI (Receive data full) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_SCI8_TXI,GROUP3), /* SCI8 TXI (Transmit data empty) */
            [12] = BSP_PRV_VECT_ENUM(EVENT_SCI8_TEI,GROUP4), /* SCI8 TEI (Transmit end) */
            [13] = BSP_PRV_VECT_ENUM(EVENT_SCI8_ERI,GROUP5), /* SCI8 ERI (Receive error) */
        };
        #endif
        #endif
