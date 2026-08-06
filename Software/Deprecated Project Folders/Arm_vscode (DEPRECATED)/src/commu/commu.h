#ifndef COMMU_H
#define COMMU_H
#include "STDINT.H"
#include <stdbool.h>
#include "BEAM/beam.h"

extern uint32_t counter;
void g_uart0_Init(void);
void g_uart1_Init(void);
void uart0_process_received_packet(void);
bool uart0_get_latest_packet(beam_uart_packet_t * p_packet);
void AGT_Timing_Init(void);
#endif // COMMU_H
