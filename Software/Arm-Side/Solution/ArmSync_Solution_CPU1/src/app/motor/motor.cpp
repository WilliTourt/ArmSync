#include "motor.h"
void Motor_Init(void) { /* TODO: CAN FD + 6路电机初始化 */ }
void Motor_SetAngles(const float angles_deg[6]) { /* TODO: 梯形加减速 -> CAN FD */ }
void Motor_Tick_1ms(void) { /* TODO: 1ms定时器更新速度曲线 */ }
int Motor_GetFeedback(float angles_deg[6], float currents_ma[6]) { return 0; }
