/* generated vector source file - do not edit */
        #include "bsp_api.h"
        /* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
        #if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = r_icu_isr, /* ICU IRQ6 (External pin interrupt 6) */
            [1] = sci_uart_rxi_isr, /* SCI2 RXI (Receive data full) */
            [2] = sci_uart_txi_isr, /* SCI2 TXI (Transmit data empty) */
            [3] = sci_uart_tei_isr, /* SCI2 TEI (Transmit end) */
            [4] = sci_uart_eri_isr, /* SCI2 ERI (Receive error) */
            [5] = canfd_error_isr, /* CAN0 CHERR (Channel  error) */
            [6] = canfd_channel_tx_isr, /* CAN0 TX (Transmit interrupt) */
            [7] = canfd_common_fifo_rx_isr, /* CAN0 COMFRX (Common FIFO receive interrupt) */
            [8] = canfd_error_isr, /* CAN GLERR (Global error) */
            [9] = canfd_rx_fifo_isr, /* CAN RXF (Global receive FIFO interrupt) */
            [10] = sci_uart_rxi_isr, /* SCI3 RXI (Receive data full) */
            [11] = sci_uart_txi_isr, /* SCI3 TXI (Transmit data empty) */
            [12] = sci_uart_tei_isr, /* SCI3 TEI (Transmit end) */
            [13] = sci_uart_eri_isr, /* SCI3 ERI (Receive error) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_ICU_IRQ6,GROUP0), /* ICU IRQ6 (External pin interrupt 6) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_SCI2_RXI,GROUP1), /* SCI2 RXI (Receive data full) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TXI,GROUP2), /* SCI2 TXI (Transmit data empty) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TEI,GROUP3), /* SCI2 TEI (Transmit end) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_SCI2_ERI,GROUP4), /* SCI2 ERI (Receive error) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_CAN0_CHERR,GROUP5), /* CAN0 CHERR (Channel  error) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_CAN0_TX,GROUP6), /* CAN0 TX (Transmit interrupt) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_CAN0_COMFRX,GROUP7), /* CAN0 COMFRX (Common FIFO receive interrupt) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_CAN_GLERR,GROUP0), /* CAN GLERR (Global error) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_CAN_RXF,GROUP1), /* CAN RXF (Global receive FIFO interrupt) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_SCI3_RXI,GROUP2), /* SCI3 RXI (Receive data full) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_SCI3_TXI,GROUP3), /* SCI3 TXI (Transmit data empty) */
            [12] = BSP_PRV_VECT_ENUM(EVENT_SCI3_TEI,GROUP4), /* SCI3 TEI (Transmit end) */
            [13] = BSP_PRV_VECT_ENUM(EVENT_SCI3_ERI,GROUP5), /* SCI3 ERI (Receive error) */
        };
        #endif
        #endif