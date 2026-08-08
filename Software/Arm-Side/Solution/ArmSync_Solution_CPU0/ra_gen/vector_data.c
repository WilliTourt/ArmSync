/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = agt_int_isr, /* AGT0 INT (AGT interrupt) */
            [1] = sci_b_uart_rxi_isr, /* SCI0 RXI (Receive data full) */
            [2] = sci_b_uart_txi_isr, /* SCI0 TXI (Transmit data empty) */
            [3] = sci_b_uart_tei_isr, /* SCI0 TEI (Transmit end) */
            [4] = sci_b_uart_eri_isr, /* SCI0 ERI (Receive error) */
            [5] = sci_b_uart_rxi_isr, /* SCI2 RXI (Receive data full) */
            [6] = sci_b_uart_txi_isr, /* SCI2 TXI (Transmit data empty) */
            [7] = sci_b_uart_tei_isr, /* SCI2 TEI (Transmit end) */
            [8] = sci_b_uart_eri_isr, /* SCI2 ERI (Receive error) */
            [9] = sci_b_uart_rxi_isr, /* SCI9 RXI (Receive data full) */
            [10] = sci_b_uart_txi_isr, /* SCI9 TXI (Transmit data empty) */
            [11] = sci_b_uart_tei_isr, /* SCI9 TEI (Transmit end) */
            [12] = sci_b_uart_eri_isr, /* SCI9 ERI (Receive error) */
            [13] = sci_b_uart_rxi_isr, /* SCI3 RXI (Receive data full) */
            [14] = sci_b_uart_txi_isr, /* SCI3 TXI (Transmit data empty) */
            [15] = sci_b_uart_tei_isr, /* SCI3 TEI (Transmit end) */
            [16] = sci_b_uart_eri_isr, /* SCI3 ERI (Receive error) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_AGT0_INT,GROUP0), /* AGT0 INT (AGT interrupt) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_SCI0_RXI,GROUP1), /* SCI0 RXI (Receive data full) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TXI,GROUP2), /* SCI0 TXI (Transmit data empty) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TEI,GROUP3), /* SCI0 TEI (Transmit end) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_SCI0_ERI,GROUP4), /* SCI0 ERI (Receive error) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_SCI2_RXI,GROUP5), /* SCI2 RXI (Receive data full) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TXI,GROUP6), /* SCI2 TXI (Transmit data empty) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TEI,GROUP7), /* SCI2 TEI (Transmit end) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_SCI2_ERI,GROUP0), /* SCI2 ERI (Receive error) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_SCI9_RXI,GROUP1), /* SCI9 RXI (Receive data full) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_SCI9_TXI,GROUP2), /* SCI9 TXI (Transmit data empty) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_SCI9_TEI,GROUP3), /* SCI9 TEI (Transmit end) */
            [12] = BSP_PRV_VECT_ENUM(EVENT_SCI9_ERI,GROUP4), /* SCI9 ERI (Receive error) */
            [13] = BSP_PRV_VECT_ENUM(EVENT_SCI3_RXI,GROUP5), /* SCI3 RXI (Receive data full) */
            [14] = BSP_PRV_VECT_ENUM(EVENT_SCI3_TXI,GROUP6), /* SCI3 TXI (Transmit data empty) */
            [15] = BSP_PRV_VECT_ENUM(EVENT_SCI3_TEI,GROUP7), /* SCI3 TEI (Transmit end) */
            [16] = BSP_PRV_VECT_ENUM(EVENT_SCI3_ERI,GROUP0), /* SCI3 ERI (Receive error) */
        };
        #endif
        #endif
