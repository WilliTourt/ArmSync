#ifndef __CLAW_H
#define __CLAW_H

#include "stdint.h"
void Servo_GPT_PWM_Init(void);

void Servo_GPT_PWM_SetDuty(uint8_t duty);



void Origin_Calibration(void);
#endif