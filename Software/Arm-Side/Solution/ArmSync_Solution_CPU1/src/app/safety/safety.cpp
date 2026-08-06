#include "safety.h"
static safety_status_t g_status = SAFETY_OK;
void Safety_Init(void) { /* TODO */ }
safety_status_t Safety_Check(const float angles_deg[6], const float currents_ma[6], int ms_since_update) { return g_status; }
safety_status_t Safety_GetStatus(void) { return g_status; }
void Safety_Clear(void) { g_status = SAFETY_OK; }
