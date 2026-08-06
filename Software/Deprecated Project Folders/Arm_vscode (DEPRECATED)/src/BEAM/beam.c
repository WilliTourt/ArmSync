#include "beam.h"
#include <stdint.h>
#include "EmmV5/Emm_V5.h"
#include "math.h"
#include "claw/claw.h"
motor_t Upper_arm_roll = {6,2,20,1822,-1106};
motor_t Upper_arm_pitch = {1,1,30,2360,0};
motor_t Upper_arm_yaw = {2,2,10,900,-900};
motor_t Fore_arm_roll = {3,0,10,0,-1259};
motor_t Fore_arm_yaw = {4,2,1,90,-90};
motor_t Wrist_pitch = {5,2,10,900,-900};

uint32_t cal_clk(motor_t *motor, float angle_deg)
{
    
    uint32_t clk = 0;
    clk = (uint32_t)(angle_deg / 1.8f * 16.0f * motor->redu_rate);
    return clk;
    
}


void Pos_Control(motor_t *motor, float angle_deg)
{
    
    uint8_t temp_dir = 0;
    uint16_t temp_vel = 100;
    float temp_angle = fabs(angle_deg);
    switch(motor->addr)
    {
        case 1:
            temp_dir = 1;
            break;
        case 2:
            if(angle_deg > 0.0f)
            {
                temp_dir = 1;
            }
            else
            {
                temp_dir = 0;
            }
            break;
        case 3:
            if(angle_deg > 0.0f)
            {
                temp_dir = 0;
            }
            else
            {
                temp_dir = 1;
            }
            break;
        case 4:
            if(angle_deg > 0.0f)
            {
                temp_dir = 0;
            }
            else
            {
                temp_dir = 1;
            }
            break;
        case 5:

            
            if(angle_deg > 0.0f)
            {
                temp_dir = 0;
                if(angle_deg >=90.0f)
            {
                angle_deg = 90.0f;
            }
            }
            else
            {
                temp_dir = 1;
                if(angle_deg <= -90.0f)
            {
                angle_deg = -90.0f;
            }
            }
            break;
        case 6:
            temp_vel =150;
            if(angle_deg > 0.0f)
            {
                temp_dir = 0;
            }
            else
            {
                temp_dir = 1;
            }
            break;
        default:
            temp_dir = motor->dir;
    }

    Emm_V5_Pos_Control(motor->addr, temp_dir, temp_vel, 150, cal_clk(motor, temp_angle), 1, 0);
    

}

void Beam_Apply_Uart_Packet(beam_uart_packet_t const * p_packet)
{
    if (NULL == p_packet)
    {
        return;
    }
    static float last_upper_pitch = 0;
    static float last_upper_roll = 0;
    static float last_upper_yaw = 0;
    static float last_fore_roll = 0;
    static float last_fore_yaw = 0;
    static float last_hand_roll = 0;
    const float a = 0.8f;

    float current_upper_pitch = p_packet->upper.pitch;
    float current_upper_roll = p_packet->upper.roll;
    float current_upper_yaw = p_packet->upper.yaw;
    float current_fore_roll = p_packet->fore.roll;
    float current_fore_yaw = p_packet->fore.yaw;
    float current_hand_roll = p_packet->hand.roll;

    current_upper_pitch = current_upper_pitch * (1 - a) + last_upper_pitch * a;
    current_upper_roll  = current_upper_roll  * (1 - a) + last_upper_roll  * a;
    current_upper_yaw   = current_upper_yaw   * (1 - a) + last_upper_yaw   * a;
    current_fore_roll   = current_fore_roll   * (1 - a) + last_fore_roll   * a;
    current_fore_yaw    = current_fore_yaw    * (1 - a) + last_fore_yaw    * a;
    current_hand_roll   = current_hand_roll   * (1 - a) + last_hand_roll   * a;

    last_upper_pitch = current_upper_pitch;
    last_upper_roll  = current_upper_roll;
    last_upper_yaw   = current_upper_yaw;
    last_fore_roll   = current_fore_roll;   
    last_fore_yaw    = current_fore_yaw;
    last_hand_roll   = current_hand_roll;

    Pos_Control(&Upper_arm_pitch, current_upper_pitch);
    Pos_Control(&Upper_arm_roll,  current_upper_roll);
    Pos_Control(&Upper_arm_yaw,   current_upper_yaw);
    Pos_Control(&Fore_arm_roll,   current_fore_roll);
    Pos_Control(&Fore_arm_yaw,    current_fore_yaw);
    Pos_Control(&Wrist_pitch,     current_hand_roll);

    float claw_duty = ((float)(p_packet->claw_adc) / 4096.0f) * 10.0f + 3.0f;
    Servo_GPT_PWM_SetDuty((uint8_t)claw_duty);

}
