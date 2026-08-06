/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
        extern "C" {
        #endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (5)
#endif
/* ISR prototypes */
void canfd_error_isr(void);
void canfd_channel_tx_isr(void);
void canfd_common_fifo_rx_isr(void);
void canfd_rx_fifo_isr(void);

/* Vector table allocations */
#define VECTOR_NUMBER_CAN1_CHERR ((IRQn_Type) 0) /* CAN1 CHERR (Channel  error) */
#define CAN1_CHERR_IRQn          ((IRQn_Type) 0) /* CAN1 CHERR (Channel  error) */
#define VECTOR_NUMBER_CAN1_TX ((IRQn_Type) 1) /* CAN1 TX (Transmit interrupt) */
#define CAN1_TX_IRQn          ((IRQn_Type) 1) /* CAN1 TX (Transmit interrupt) */
#define VECTOR_NUMBER_CAN1_COMFRX ((IRQn_Type) 2) /* CAN1 COMFRX (Common FIFO receive interrupt) */
#define CAN1_COMFRX_IRQn          ((IRQn_Type) 2) /* CAN1 COMFRX (Common FIFO receive interrupt) */
#define VECTOR_NUMBER_CAN_GLERR ((IRQn_Type) 3) /* CAN GLERR (Global error) */
#define CAN_GLERR_IRQn          ((IRQn_Type) 3) /* CAN GLERR (Global error) */
#define VECTOR_NUMBER_CAN_RXF ((IRQn_Type) 4) /* CAN RXF (Global receive FIFO interrupt) */
#define CAN_RXF_IRQn          ((IRQn_Type) 4) /* CAN RXF (Global receive FIFO interrupt) */
/* The number of entries required for the ICU vector table. */
#define BSP_ICU_VECTOR_NUM_ENTRIES (5)

#ifdef __cplusplus
        }
        #endif
#endif /* VECTOR_DATA_H */
