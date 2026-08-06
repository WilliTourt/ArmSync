#ifndef IPC_ISR_H
#define IPC_ISR_H
#include <stdint.h>
#include "../shared/ipc_frame.h"
void IPC_ISR_Init(void);
void IPC_ISR_Handler(void);
int ipc_frame_available(void);
const ipc_frame_t* ipc_get_frame(void);
void ipc_ack_frame(void);
#endif
