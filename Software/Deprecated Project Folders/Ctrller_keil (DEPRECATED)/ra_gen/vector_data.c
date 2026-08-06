/* generated vector source file - do not edit */
        #include "bsp_api.h"
        /* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
        #if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = r_icu_isr, /* ICU IRQ0 (External pin interrupt 0) */
            [1] = r_icu_isr, /* ICU IRQ1 (External pin interrupt 1) */
            [2] = adc_scan_end_isr, /* ADC0 SCAN END (End of A/D scanning operation) */
            [3] = sci_uart_rxi_isr, /* SCI1 RXI (Receive data full) */
            [4] = sci_uart_txi_isr, /* SCI1 TXI (Transmit data empty) */
            [5] = sci_uart_tei_isr, /* SCI1 TEI (Transmit end) */
            [6] = sci_uart_eri_isr, /* SCI1 ERI (Receive error) */
            [7] = sci_uart_rxi_isr, /* SCI2 RXI (Receive data full) */
            [8] = sci_uart_txi_isr, /* SCI2 TXI (Transmit data empty) */
            [9] = sci_uart_tei_isr, /* SCI2 TEI (Transmit end) */
            [10] = sci_uart_eri_isr, /* SCI2 ERI (Receive error) */
            [11] = agt_int_isr, /* AGT0 INT (AGT interrupt) */
            [12] = r_icu_isr, /* ICU IRQ9 (External pin interrupt 9) */
            [13] = spi_rxi_isr, /* SPI1 RXI (Receive buffer full) */
            [14] = spi_txi_isr, /* SPI1 TXI (Transmit buffer empty) */
            [15] = spi_tei_isr, /* SPI1 TEI (Transmission complete event) */
            [16] = spi_eri_isr, /* SPI1 ERI (Error) */
            [17] = iic_master_rxi_isr, /* IIC1 RXI (Receive data full) */
            [18] = iic_master_txi_isr, /* IIC1 TXI (Transmit data empty) */
            [19] = iic_master_tei_isr, /* IIC1 TEI (Transmit end) */
            [20] = iic_master_eri_isr, /* IIC1 ERI (Transfer error) */
            [21] = agt_int_isr, /* AGT1 INT (AGT interrupt) */
            [22] = r_icu_isr, /* ICU IRQ2 (External pin interrupt 2) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_ICU_IRQ0,GROUP0), /* ICU IRQ0 (External pin interrupt 0) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_ICU_IRQ1,GROUP1), /* ICU IRQ1 (External pin interrupt 1) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_ADC0_SCAN_END,GROUP2), /* ADC0 SCAN END (End of A/D scanning operation) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_SCI1_RXI,GROUP3), /* SCI1 RXI (Receive data full) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_SCI1_TXI,GROUP4), /* SCI1 TXI (Transmit data empty) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_SCI1_TEI,GROUP5), /* SCI1 TEI (Transmit end) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_SCI1_ERI,GROUP6), /* SCI1 ERI (Receive error) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_SCI2_RXI,GROUP7), /* SCI2 RXI (Receive data full) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TXI,GROUP0), /* SCI2 TXI (Transmit data empty) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TEI,GROUP1), /* SCI2 TEI (Transmit end) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_SCI2_ERI,GROUP2), /* SCI2 ERI (Receive error) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_AGT0_INT,GROUP3), /* AGT0 INT (AGT interrupt) */
            [12] = BSP_PRV_VECT_ENUM(EVENT_ICU_IRQ9,GROUP4), /* ICU IRQ9 (External pin interrupt 9) */
            [13] = BSP_PRV_VECT_ENUM(EVENT_SPI1_RXI,GROUP5), /* SPI1 RXI (Receive buffer full) */
            [14] = BSP_PRV_VECT_ENUM(EVENT_SPI1_TXI,GROUP6), /* SPI1 TXI (Transmit buffer empty) */
            [15] = BSP_PRV_VECT_ENUM(EVENT_SPI1_TEI,GROUP7), /* SPI1 TEI (Transmission complete event) */
            [16] = BSP_PRV_VECT_ENUM(EVENT_SPI1_ERI,GROUP0), /* SPI1 ERI (Error) */
            [17] = BSP_PRV_VECT_ENUM(EVENT_IIC1_RXI,GROUP1), /* IIC1 RXI (Receive data full) */
            [18] = BSP_PRV_VECT_ENUM(EVENT_IIC1_TXI,GROUP2), /* IIC1 TXI (Transmit data empty) */
            [19] = BSP_PRV_VECT_ENUM(EVENT_IIC1_TEI,GROUP3), /* IIC1 TEI (Transmit end) */
            [20] = BSP_PRV_VECT_ENUM(EVENT_IIC1_ERI,GROUP4), /* IIC1 ERI (Transfer error) */
            [21] = BSP_PRV_VECT_ENUM(EVENT_AGT1_INT,GROUP5), /* AGT1 INT (AGT interrupt) */
            [22] = BSP_PRV_VECT_ENUM(EVENT_ICU_IRQ2,GROUP6), /* ICU IRQ2 (External pin interrupt 2) */
        };
        #endif
        #endif