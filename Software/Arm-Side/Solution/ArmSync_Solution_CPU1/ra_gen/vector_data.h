/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
        extern "C" {
        #endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (14)
#endif
/* ISR prototypes */
void ipc_isr(void);
void sci_b_uart_rxi_isr(void);
void sci_b_uart_txi_isr(void);
void sci_b_uart_tei_isr(void);
void sci_b_uart_eri_isr(void);
void agt_int_isr(void);

/* Vector table allocations */
#define VECTOR_NUMBER_IPC_IRQ0 ((IRQn_Type) 0) /* IPC IRQ0 (CPU Mutual Interrupt 0) */
#define IPC_IRQ0_IRQn          ((IRQn_Type) 0) /* IPC IRQ0 (CPU Mutual Interrupt 0) */
#define VECTOR_NUMBER_SCI1_RXI ((IRQn_Type) 1) /* SCI1 RXI (Receive data full) */
#define SCI1_RXI_IRQn          ((IRQn_Type) 1) /* SCI1 RXI (Receive data full) */
#define VECTOR_NUMBER_SCI1_TXI ((IRQn_Type) 2) /* SCI1 TXI (Transmit data empty) */
#define SCI1_TXI_IRQn          ((IRQn_Type) 2) /* SCI1 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI1_TEI ((IRQn_Type) 3) /* SCI1 TEI (Transmit end) */
#define SCI1_TEI_IRQn          ((IRQn_Type) 3) /* SCI1 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI1_ERI ((IRQn_Type) 4) /* SCI1 ERI (Receive error) */
#define SCI1_ERI_IRQn          ((IRQn_Type) 4) /* SCI1 ERI (Receive error) */
#define VECTOR_NUMBER_AGT1_INT ((IRQn_Type) 5) /* AGT1 INT (AGT interrupt) */
#define AGT1_INT_IRQn          ((IRQn_Type) 5) /* AGT1 INT (AGT interrupt) */
#define VECTOR_NUMBER_SCI5_RXI ((IRQn_Type) 6) /* SCI5 RXI (Receive data full) */
#define SCI5_RXI_IRQn          ((IRQn_Type) 6) /* SCI5 RXI (Receive data full) */
#define VECTOR_NUMBER_SCI5_TXI ((IRQn_Type) 7) /* SCI5 TXI (Transmit data empty) */
#define SCI5_TXI_IRQn          ((IRQn_Type) 7) /* SCI5 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI5_TEI ((IRQn_Type) 8) /* SCI5 TEI (Transmit end) */
#define SCI5_TEI_IRQn          ((IRQn_Type) 8) /* SCI5 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI5_ERI ((IRQn_Type) 9) /* SCI5 ERI (Receive error) */
#define SCI5_ERI_IRQn          ((IRQn_Type) 9) /* SCI5 ERI (Receive error) */
#define VECTOR_NUMBER_SCI8_RXI ((IRQn_Type) 10) /* SCI8 RXI (Receive data full) */
#define SCI8_RXI_IRQn          ((IRQn_Type) 10) /* SCI8 RXI (Receive data full) */
#define VECTOR_NUMBER_SCI8_TXI ((IRQn_Type) 11) /* SCI8 TXI (Transmit data empty) */
#define SCI8_TXI_IRQn          ((IRQn_Type) 11) /* SCI8 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI8_TEI ((IRQn_Type) 12) /* SCI8 TEI (Transmit end) */
#define SCI8_TEI_IRQn          ((IRQn_Type) 12) /* SCI8 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI8_ERI ((IRQn_Type) 13) /* SCI8 ERI (Receive error) */
#define SCI8_ERI_IRQn          ((IRQn_Type) 13) /* SCI8 ERI (Receive error) */
/* The number of entries required for the ICU vector table. */
#define BSP_ICU_VECTOR_NUM_ENTRIES (14)

#ifdef __cplusplus
        }
        #endif
#endif /* VECTOR_DATA_H */
