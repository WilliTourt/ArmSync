#include "claw.h"
#include "C:\Users\wtfzfj\FSPSmartConfigurator\arm\ra_gen\hal_data.h"

#include "ra/fsp/inc/api/r_timer_api.h"
#include <stdint.h>
uint8_t Temp_Duty = 0 ;
void Servo_GPT_PWM_Init(void)
{
    R_GPT_Open(&claw_ctrl_ctrl, &claw_ctrl_cfg);
    R_GPT_Start(&claw_ctrl_ctrl);
}

/**
 * @brief 设置GPT PWM占空比
 * @param duty 占空比百分比，范围0-100%
 */
void Servo_GPT_PWM_SetDuty(uint8_t duty)
{
    timer_info_t info;
    uint32_t current_period_counts;
    uint32_t duty_cycle_counts;

    if (duty > 100) duty = 100;

    R_GPT_InfoGet(&claw_ctrl_ctrl, &info);

    current_period_counts = info.period_counts;

    duty_cycle_counts = (uint32_t)(((uint64_t) current_period_counts * duty) / 100);

    R_GPT_DutyCycleSet(&claw_ctrl_ctrl, duty_cycle_counts, GPT_IO_PIN_GTIOCA);
}


void Origin_Calibration(void)
{
    for (int i=0;i<180;i++)
    {
        Servo_Set_angle((float)i);
        R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    }
    
}