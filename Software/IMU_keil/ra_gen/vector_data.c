/* generated vector source file - do not edit */
        #include "bsp_api.h"
        /* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
        #if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = sci_uart_rxi_isr, /* SCI1 RXI (Receive data full) */
            [1] = sci_uart_txi_isr, /* SCI1 TXI (Transmit data empty) */
            [2] = sci_uart_tei_isr, /* SCI1 TEI (Transmit end) */
            [3] = sci_uart_eri_isr, /* SCI1 ERI (Receive error) */
            [4] = sci_uart_rxi_isr, /* SCI2 RXI (Receive data full) */
            [5] = sci_uart_txi_isr, /* SCI2 TXI (Transmit data empty) */
            [6] = sci_uart_tei_isr, /* SCI2 TEI (Transmit end) */
            [7] = sci_uart_eri_isr, /* SCI2 ERI (Receive error) */
            [8] = spi_rxi_isr, /* SPI1 RXI (Receive buffer full) */
            [9] = spi_txi_isr, /* SPI1 TXI (Transmit buffer empty) */
            [10] = spi_tei_isr, /* SPI1 TEI (Transmission complete event) */
            [11] = spi_eri_isr, /* SPI1 ERI (Error) */
            [12] = iic_master_rxi_isr, /* IIC1 RXI (Receive data full) */
            [13] = iic_master_txi_isr, /* IIC1 TXI (Transmit data empty) */
            [14] = iic_master_tei_isr, /* IIC1 TEI (Transmit end) */
            [15] = iic_master_eri_isr, /* IIC1 ERI (Transfer error) */
            [16] = agt_int_isr, /* AGT1 INT (AGT interrupt) */
            [17] = r_icu_isr, /* ICU IRQ1 (External pin interrupt 1) */
            [18] = agt_int_isr, /* AGT0 INT (AGT interrupt) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_SCI1_RXI,GROUP0), /* SCI1 RXI (Receive data full) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_SCI1_TXI,GROUP1), /* SCI1 TXI (Transmit data empty) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_SCI1_TEI,GROUP2), /* SCI1 TEI (Transmit end) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_SCI1_ERI,GROUP3), /* SCI1 ERI (Receive error) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_SCI2_RXI,GROUP4), /* SCI2 RXI (Receive data full) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TXI,GROUP5), /* SCI2 TXI (Transmit data empty) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TEI,GROUP6), /* SCI2 TEI (Transmit end) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_SCI2_ERI,GROUP7), /* SCI2 ERI (Receive error) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_SPI1_RXI,GROUP0), /* SPI1 RXI (Receive buffer full) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_SPI1_TXI,GROUP1), /* SPI1 TXI (Transmit buffer empty) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_SPI1_TEI,GROUP2), /* SPI1 TEI (Transmission complete event) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_SPI1_ERI,GROUP3), /* SPI1 ERI (Error) */
            [12] = BSP_PRV_VECT_ENUM(EVENT_IIC1_RXI,GROUP4), /* IIC1 RXI (Receive data full) */
            [13] = BSP_PRV_VECT_ENUM(EVENT_IIC1_TXI,GROUP5), /* IIC1 TXI (Transmit data empty) */
            [14] = BSP_PRV_VECT_ENUM(EVENT_IIC1_TEI,GROUP6), /* IIC1 TEI (Transmit end) */
            [15] = BSP_PRV_VECT_ENUM(EVENT_IIC1_ERI,GROUP7), /* IIC1 ERI (Transfer error) */
            [16] = BSP_PRV_VECT_ENUM(EVENT_AGT1_INT,GROUP0), /* AGT1 INT (AGT interrupt) */
            [17] = BSP_PRV_VECT_ENUM(EVENT_ICU_IRQ1,GROUP1), /* ICU IRQ1 (External pin interrupt 1) */
            [18] = BSP_PRV_VECT_ENUM(EVENT_AGT0_INT,GROUP2), /* AGT0 INT (AGT interrupt) */
        };
        #endif
        #endif