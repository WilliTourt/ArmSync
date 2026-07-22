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
            [5] = canfd_error_isr, /* CAN1 CHERR (Channel  error) */
            [6] = canfd_channel_tx_isr, /* CAN1 TX (Transmit interrupt) */
            [7] = canfd_common_fifo_rx_isr, /* CAN1 COMFRX (Common FIFO receive interrupt) */
            [8] = canfd_error_isr, /* CAN GLERR (Global error) */
            [9] = canfd_rx_fifo_isr, /* CAN RXF (Global receive FIFO interrupt) */
            [10] = sci_b_uart_rxi_isr, /* SCI9 RXI (Receive data full) */
            [11] = sci_b_uart_txi_isr, /* SCI9 TXI (Transmit data empty) */
            [12] = sci_b_uart_tei_isr, /* SCI9 TEI (Transmit end) */
            [13] = sci_b_uart_eri_isr, /* SCI9 ERI (Receive error) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_AGT0_INT,GROUP0), /* AGT0 INT (AGT interrupt) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_SCI0_RXI,GROUP1), /* SCI0 RXI (Receive data full) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TXI,GROUP2), /* SCI0 TXI (Transmit data empty) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TEI,GROUP3), /* SCI0 TEI (Transmit end) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_SCI0_ERI,GROUP4), /* SCI0 ERI (Receive error) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_CAN1_CHERR,GROUP5), /* CAN1 CHERR (Channel  error) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_CAN1_TX,GROUP6), /* CAN1 TX (Transmit interrupt) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_CAN1_COMFRX,GROUP7), /* CAN1 COMFRX (Common FIFO receive interrupt) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_CAN_GLERR,GROUP0), /* CAN GLERR (Global error) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_CAN_RXF,GROUP1), /* CAN RXF (Global receive FIFO interrupt) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_SCI9_RXI,GROUP2), /* SCI9 RXI (Receive data full) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_SCI9_TXI,GROUP3), /* SCI9 TXI (Transmit data empty) */
            [12] = BSP_PRV_VECT_ENUM(EVENT_SCI9_TEI,GROUP4), /* SCI9 TEI (Transmit end) */
            [13] = BSP_PRV_VECT_ENUM(EVENT_SCI9_ERI,GROUP5), /* SCI9 ERI (Receive error) */
        };
        #endif
        #endif
