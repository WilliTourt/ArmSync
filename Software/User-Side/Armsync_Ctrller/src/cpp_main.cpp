#include "cpp_main.h"

#include <cstdio>

#include "callbacks.h"
#include "adc.h"
#include "periph.h"
#include "utils.h"

#define UART1_RECEIVE_LEN 43
#define UART2_RECEIVE_LEN 5

AdcSlider sliderGrip(ADC_CHANNEL_0);
AdcSlider sliderPitch(ADC_CHANNEL_9);

volatile uint32_t sysTick = 0;
volatile bool main_timer_call = false;
volatile bool uart1_receive_complete_flag = false;
volatile bool uart2_receive_complete_flag = false;
volatile bool uart1_send_complete_flag = false;
volatile bool uart2_send_complete_flag = false;
volatile bool uart2_send_pending = false;  // UART2正在发送中
volatile bool isKeyActive = false;
volatile bool is_calibrated = false;

// 小臂发来的欧拉角（大端序 float）
struct ArmData {
    float fore_quaternion[4];
    float elbow_x, elbow_y, elbow_z;
    float wrist_x, wrist_y, wrist_z;
};
volatile ArmData armData = {0, 0, 0, 0, 0, 0};

static float sliderGripPercent = 0.0;
static float sliderPitchPercent = 0.0;
static char tx_buf[128];  // sprintf buffer, static to avoid stack overflow

void cpp_main() {
    AdcBase::init();
    Periph::init();

    R_IOPORT_PinWrite(&g_ioport_ctrl, LED1, BSP_IO_LEVEL_LOW);
    R_IOPORT_PinWrite(&g_ioport_ctrl, LED2, BSP_IO_LEVEL_LOW);
    Periph::setBuzzerDutyCycle(50);
    Periph::setBuzzerFreq(1350);
    R_BSP_SoftwareDelay(200, BSP_DELAY_UNITS_MILLISECONDS);
    Periph::setBuzzerFreq(2700);
    R_BSP_SoftwareDelay(200, BSP_DELAY_UNITS_MILLISECONDS);
    // R_IOPORT_PinWrite(&g_ioport_ctrl, LED1, BSP_IO_LEVEL_HIGH);
    Periph::setBuzzerDutyCycle(0);
    R_IOPORT_PinWrite(&g_ioport_ctrl, LED2, BSP_IO_LEVEL_HIGH);

    uint8_t receivePacket1[UART1_RECEIVE_LEN];
    uint8_t receivePacket2[UART2_RECEIVE_LEN];

    R_SCI_UART_Read(&g_uart1_ctrl, receivePacket1, UART1_RECEIVE_LEN);
    R_SCI_UART_Read(&g_uart2_ctrl, receivePacket2, UART2_RECEIVE_LEN);

    while (1) {

        if (main_timer_call) {
            sliderGripPercent = sliderGrip.getPercent();
            sliderPitchPercent = sliderPitch.getPercent();
            main_timer_call = false;
        }

        static uint32_t now = 0;
        if (sysTick - now >= 500) {
            now = sysTick;
            bsp_io_level_t led2State;
            R_IOPORT_PinRead(&g_ioport_ctrl, LED2, &led2State);
            R_IOPORT_PinWrite(&g_ioport_ctrl, LED2, (bsp_io_level_t)(BSP_IO_LEVEL_HIGH - led2State));
        }

        // UART2 发送完成处理
        if (uart2_send_complete_flag) {
            uart2_send_complete_flag = false;
            uart2_send_pending = false;
        }

        // UART1: fixed-length 27-byte packets from arm IMU
        if (uart1_receive_complete_flag) {
            uart1_receive_complete_flag = false;

            if (receivePacket1[0] == 0xAF && receivePacket1[1] == 0x01 && receivePacket1[42] == 0xFA) {
                armData.fore_quaternion[0] = Utils::bytes2F(receivePacket1 + 2);
                armData.fore_quaternion[1] = Utils::bytes2F(receivePacket1 + 6);
                armData.fore_quaternion[2] = Utils::bytes2F(receivePacket1 + 10);
                armData.fore_quaternion[3] = Utils::bytes2F(receivePacket1 + 14);
                armData.elbow_x            = Utils::bytes2F(receivePacket1 + 18);
                armData.elbow_y            = Utils::bytes2F(receivePacket1 + 22);
                armData.elbow_z            = Utils::bytes2F(receivePacket1 + 26);
                armData.wrist_x            = Utils::bytes2F(receivePacket1 + 30);
                armData.wrist_y            = Utils::bytes2F(receivePacket1 + 34);
                armData.wrist_z            = Utils::bytes2F(receivePacket1 + 38);

                if (!is_calibrated) {
                    for (uint8_t i = 0; i < 3; i++) {
                        Periph::setBuzzerDutyCycle(50);
                        R_BSP_SoftwareDelay(66, BSP_DELAY_UNITS_MILLISECONDS);
                        Periph::setBuzzerDutyCycle(0);
                        R_BSP_SoftwareDelay(33, BSP_DELAY_UNITS_MILLISECONDS);                   
                    }
                    is_calibrated = true;
                }
                
                int len = sprintf(tx_buf, "%.4f, %.4f, %.4f, %.4f, 
                                           %.4f, %.4f, %.4f, 
                                           %.4f, %.4f, %.4f\n",
                    armData.fore_quaternion[0], armData.fore_quaternion[1], armData.fore_quaternion[2], armData.fore_quaternion[3],
                    armData.elbow_x, armData.elbow_y, armData.elbow_z,
                    armData.wrist_x, armData.wrist_y, armData.wrist_z);
                if (!uart2_send_pending) {
                    uart2_send_pending = true;
                    R_SCI_UART_Write(&g_uart2_ctrl, (uint8_t*)tx_buf, len);
                }

                static uint8_t tx_led_count = 0;
                static bool tx_led = false;
                tx_led_count++;
                if(tx_led_count >= 2) {
                    tx_led_count = 0;
                    tx_led = !tx_led;
                    R_IOPORT_PinWrite(&g_ioport_ctrl, LED2, tx_led ? BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW);
                }
            }
            R_SCI_UART_Read(&g_uart1_ctrl, receivePacket1, UART1_RECEIVE_LEN);
        }

        // // UART2: Type-C green board data
        // if (uart2_receive_complete_flag) {
        //     uart2_receive_complete_flag = false;

        //     if (receivePacket2[0] == 0xAF && receivePacket2[1] == 0x04 && receivePacket2[4] == 0xFA) {
        //         // TODO: 处理TYPEC绿板子数据
        //     }
        //     R_SCI_UART_Read(&g_uart2_ctrl, receivePacket2, UART2_RECEIVE_LEN);
        // }

        // Key: send calibration command
        if (isKeyActive) {
            isKeyActive = false;

            uint8_t calib_cmd[] = {0xAF, 0x02, 0xFA};
            R_SCI_UART_Write(&g_uart1_ctrl, calib_cmd, 3);
            while (!uart1_send_complete_flag);
            uart1_send_complete_flag = false;
        }
    }
}
