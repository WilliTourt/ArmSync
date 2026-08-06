#ifndef SAFETY_H
#define SAFETY_H
typedef enum { SAFETY_OK, SAFETY_LIMIT, SAFETY_OVERCURRENT, SAFETY_TIMEOUT, SAFETY_ESTOP } safety_status_t;
void Safety_Init(void);
safety_status_t Safety_Check(const float angles_deg[6], const float currents_ma[6], int ms_since_update);
safety_status_t Safety_GetStatus(void);
void Safety_Clear(void);
#endif
