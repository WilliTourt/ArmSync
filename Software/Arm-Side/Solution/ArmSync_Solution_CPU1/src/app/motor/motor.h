#ifndef MOTOR_H
#define MOTOR_H
#include <stdint.h>
void Motor_Init(void);
void Motor_SetAngles(const float angles_deg[6]);
void Motor_Tick_1ms(void);
int Motor_GetFeedback(float angles_deg[6], float currents_ma[6]);
#endif
