
#ifndef __BEAM_H__
#define __BEAM_H__
#include "hal_data.h"
#include <stdint.h>
typedef struct
{
    uint8_t addr;
    uint8_t dir;       //0为CW，1为CCW，2为两边都可以
    uint8_t redu_rate; //减速比
    uint32_t max_angle; //最大转角，单位：度
    int32_t min_angle; //最小转角，单位：度
    
} motor_t;

typedef struct
{
    float pitch;
    float roll;
    float yaw;
} beam_euler_angle_t;

typedef struct
{
    beam_euler_angle_t upper;
    beam_euler_angle_t fore;
    beam_euler_angle_t hand;
    uint16_t claw_adc;
} beam_uart_packet_t;
extern motor_t Upper_arm_roll ;
extern motor_t Upper_arm_pitch;
extern motor_t Upper_arm_yaw ;
extern motor_t Fore_arm_roll ;
extern motor_t Fore_arm_yaw ;
extern motor_t Wrist_pitch ;
uint32_t cal_clk(motor_t *motor, float angle_deg);
void Pos_Control(motor_t *motor, float angle_deg);
void Beam_Apply_Uart_Packet(beam_uart_packet_t const * p_packet);

#endif /* __BEAM_H__ */
