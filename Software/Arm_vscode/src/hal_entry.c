#include "hal_data.h"
#include "EmmV5/Emm_V5.h"
#include "CAN/can.h"
#include "Key/key.h"
#include "commu/commu.h"
#include "r_can_api.h"
#include "r_canfd.h"
#include "ra/fsp/src/bsp/mcu/all/bsp_delay.h"
#include "ra/fsp/src/bsp/mcu/all/bsp_io.h"
#include "ra/fsp/src/bsp/mcu/all/bsp_mcu_api.h"
#include "stdio.h"
#include "QSPI/bsp_qspi_flash.h"
#include "CLAW/claw.h"
#include "BEAM/beam.h"

#if (1 == BSP_MULTICORE_PROJECT) && BSP_TZ_SECURE_BUILD
bsp_ipc_semaphore_handle_t g_core_start_semaphore =
{
    .semaphore_num = 0
};
#endif

can_frame_t frame = {0};
uint8_t led_status = 0;
extern volatile bool key1_sw2_press;
extern uint32_t counter;
uint32_t ID = 0x123;
extern motor_t Upper_arm_roll ;
extern motor_t Upper_arm_pitch;
extern motor_t Upper_arm_yaw ;
extern motor_t Fore_arm_roll ;
extern motor_t Fore_arm_yaw ;
extern motor_t Wrist_pitch ;
beam_uart_packet_t  beam_packet = {0};

#define KEY_COUNT_FLASH_ADDR    0x1000

static uint32_t g_key_press_count = 0;

// static bool Read_Five_Motor_Pos(uint32_t pos_raw[5], float pos_deg[5])
// {
//     static const uint8_t motor_addr[5] = {1U, 2U, 3U, 5U, 6U};

//     if ((NULL == pos_raw) || (NULL == pos_deg))
//     {
//         return false;
//     }

//     for (uint8_t i = 0; i < 5U; i++)
//     {
//         can.rxFrameFlag       = false;
//         can.motorCurPosUpdated = false;

//         Emm_V5_Read_Sys_Params(motor_addr[i], S_CPOS);

//         bool received = false;
//         for (uint32_t wait_ms = 0; wait_ms < 50U; wait_ms++)
//         {
//             if (can.rxFrameFlag)
//             {
//                 uint8_t addr = (uint8_t) (can.rxExtId >> 8);

//                 if ((motor_addr[i] == addr) &&
//                     (7U == can.rxDlc) &&
//                     (0x36U == can.rxData[0]))
//                 {
//                     uint32_t pos = ((uint32_t) can.rxData[2] << 24) |
//                                    ((uint32_t) can.rxData[3] << 16) |
//                                    ((uint32_t) can.rxData[4] << 8)  |
//                                    ((uint32_t) can.rxData[5]);

//                     float angle = (float) pos * 360.0f / 65536.0f;
//                     if (0U != can.rxData[1])
//                     {
//                         angle = -angle;
//                     }

//                     pos_raw[i] = pos;
//                     pos_deg[i] = angle;
//                     received   = true;
//                 }

//                 can.rxFrameFlag = false;

//                 if (received)
//                 {
//                     break;
//                 }
//             }

//             R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
//         }

//         if (!received)
//         {
//             return false;
//         }
//     }

//     return true;        
// }

void hal_entry(void)
{



    can_Init();
    Key_IRQ_Init();
    g_uart0_Init();
    g_uart1_Init();

    Servo_GPT_PWM_Init();
    // AGT_Timing_Init();
    // QSPI_Flash_Init();
    // R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MILLISECONDS);
    // uint8_t read_buf[4];
    // QSPI_Flash_BufferRead(read_buf, KEY_COUNT_FLASH_ADDR, 4);
    // if ((0xFFU == read_buf[0]) && (0xFFU == read_buf[1]) && (0xFFU == read_buf[2]) && (0xFFU == read_buf[3]))
    // {
        // g_key_press_count = 0;
        // printf("No valid key count found, initialize to 0\n");
    // }
    // else
    // {
        // g_key_press_count = ((uint32_t) read_buf[0] << 24) |
                            // ((uint32_t) read_buf[1] << 16) |
                            // ((uint32_t) read_buf[2] << 8)  |
                            // ((uint32_t) read_buf[3]);
        // printf("Key press count from Flash: %lu\n", g_key_press_count);
    // }
    // if (Read_Five_Motor_Pos(motor_pos_raw, motor_pos_deg))
    // {
        // printf("Motor 1: pos_raw=%lu pos=%.2f deg\n", motor_pos_raw[0], (double) motor_pos_deg[0]);
        // printf("Motor 2: pos_raw=%lu pos=%.2f deg\n", motor_pos_raw[1], (double) motor_pos_deg[1]);
        // printf("Motor 3: pos_raw=%lu pos=%.2f deg\n", motor_pos_raw[2], (double) motor_pos_deg[2]);
        // printf("Motor 5: pos_raw=%lu pos=%.2f deg\n", motor_pos_raw[3], (double) motor_pos_deg[3]);
        // printf("Motor 6: pos_raw=%lu pos=%.2f deg\n", motor_pos_raw[4], (double) motor_pos_deg[4]);
    // }
    // else
    // {
        // printf("Read five motor positions failed.\n");
    // }

    R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MILLISECONDS);
    
    //Origin_Calibration();
    //Emm_V5_Pos_Control(6,0,50,150,800,1,0);
    //Pos_Control(&Upper_arm_roll, -30.0f);
    // Pos_Control(&Upper_arm_pitch, 45.0f);
    // Pos_Control(&Fore_arm_roll, 30.0f);
    
    printf("OK");
    
    while (1)
    {
        if (uart0_get_latest_packet(&beam_packet))
        {
            Beam_Apply_Uart_Packet(&beam_packet);
                // printf("Received UART packet: upper_pitch=%.2f, upper_roll=%.2f, upper_yaw=%.2f, fore_pitch=%.2f, fore_roll=%.2f, fore_yaw=%.2f, hand_pitch=%.2f, hand_roll=%.2f, hand_yaw=%.2f, claw_adc=%u\n",
                        // (double) beam_packet.upper.pitch,
                        // (double) beam_packet.upper.roll,
                        // (double) beam_packet.upper.yaw,
                        // (double) beam_packet.fore.pitch,
                        // (double) beam_packet.fore.roll,
                        // (double) beam_packet.fore.yaw,
                        // (double) beam_packet.hand.pitch,
                        // (double) beam_packet.hand.roll,
                        // (double) beam_packet.hand.yaw,
                        // beam_packet.claw_adc);
                        
        }
        if (key1_sw2_press)
        {

            Pos_Control(&Upper_arm_roll, 0.0f);
            Pos_Control(&Upper_arm_pitch, 0.0f);
            Pos_Control(&Upper_arm_yaw, 0.0f);
            Pos_Control(&Fore_arm_roll, 0.0f);
            Pos_Control(&Fore_arm_yaw, 0.0f);
            Pos_Control(&Wrist_pitch, 0.0f);
            key1_sw2_press = false;
        }
    }

#if (0 == _RA_CORE) && (1 == BSP_MULTICORE_PROJECT) && !BSP_TZ_NONSECURE_BUILD
#if BSP_TZ_SECURE_BUILD
    R_BSP_IpcSemaphoreTake(&g_core_start_semaphore);
#endif
    R_BSP_SecondaryCoreStart();
#if BSP_TZ_SECURE_BUILD
    while (FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&g_core_start_semaphore))
    {
        ;
    }
#endif
#endif

#if (1 == _RA_CORE) && (1 == BSP_MULTICORE_PROJECT) && BSP_TZ_SECURE_BUILD
    R_BSP_IpcSemaphoreGive(&g_core_start_semaphore);
#endif

#if BSP_TZ_SECURE_BUILD
    R_BSP_NonSecureEnter();
#endif
}
