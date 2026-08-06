#include "ipc_isr.h"
#include <cstring>
#define IPC_SHM_BASE 0x20080000
static ipc_frame_t g_rx_frame;
static volatile int g_new_frame = 0;
void IPC_ISR_Init(void) { /* TODO */ }
void IPC_ISR_Handler(void) {
    volatile ipc_frame_t *shm = (volatile ipc_frame_t *)IPC_SHM_BASE;
    if (ipc_frame_valid((const ipc_frame_t *)shm)) {
        std::memcpy(&g_rx_frame, (const void *)shm, sizeof(ipc_frame_t));
        g_new_frame = 1;
    }
}
int ipc_frame_available(void) { return g_new_frame; }
const ipc_frame_t* ipc_get_frame(void) { return g_new_frame ? &g_rx_frame : nullptr; }
void ipc_ack_frame(void) { g_new_frame = 0; }
