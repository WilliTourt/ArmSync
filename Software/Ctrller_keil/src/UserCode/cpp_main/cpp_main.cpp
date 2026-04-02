#include "src\UserCode\cpp_main\cpp_main.h"

#include "UserCode\ICM42688\ICM42688.h"
#include "UserCode\QMC5883P\qmc5883p.h"
// #include "debug.h"
#include "UserCode\ICM42688\Madgwick.h"
#include "common_data.h"
#include "fsp_common_api.h"
#include "hal_data.h"
#include "r_adc.h"
#include "r_adc_api.h"
#include "r_agt.h"
#include "r_icu.h"
#include "r_sci_uart.h"
#include "r_timer_api.h"
#include "string.h"
#include "r_iic_master.h"
#include "r_ioport_cfg.h"               // Renesas::Flex Software:Build Configuration


extern "C"{
#include "stdio.h"
#include "UserCode\ICM42688\ESKF.h"
}

#define UART1_RECEIVE_LEN 51
#define UART2_RECEIVE_LEN 5

// sysTick
volatile uint32_t sysTick = 0;

// 传感器相关
ICM42688 icm42688(ICM42688::ICM42688_MountingOrientation::MOUNT_Y_DOWN);
QMC5883P qmc5883p(QMC5883P::QMC5883P_Mode::CONTINUOUS, QMC5883P::QMC5883P_Spd::ODR_100HZ, QMC5883P::QMC5883P_Rng::RNG_2G);
EULER euler(25.98f);

volatile ICM42688::ICM42688_StatusTypeDef res_icm42688;
volatile ICM42688::General::StatusTypeDef res_general_icm42688;
volatile ICM42688::INT::StatusTypeDef res_int_icm42688;
volatile QMC5883P::QMC5883P_Status res_qmc5883p;

ESKF_State eskf_state;

float gyro_bias_x = 0;
float gyro_bias_y = 0;
float gyro_bias_z = 0;

void ICM42688_dataRady(void* context)
{
    qmc5883p.update();
    icm42688.general.read_data();
}

// 串口发送完成标志
volatile bool uart1_send_complete_flag = false;
volatile bool uart2_send_complete_flag = false;
volatile bool uart1_receive_complete_flag = false;
volatile bool uart2_receive_complete_flag = false;

// ADC
volatile bool scan_complete_flag = false;  
volatile uint16_t ADC_9_value;
volatile float ADC_PWM_truly_value = 0.6f;

// 主循环定时器
volatile bool main_timer_call = false;

// 数据包空间
uint8_t sendbuf[64] = {0};
uint8_t sendbuf_key1[3] = {0xAF, 0x02, 0xFA};
uint8_t sendbuf_key2[3] = {0xAF, 0x03, 0xFA};
uint8_t receivePacket1[64] = {0};
uint8_t receivePacket2[32] = {0};

// debug存储空间
uint8_t debug_buf[128] = {0};

// UI状态                                                       
volatile bool isKey1Active = false;              // KEY1激活状态
volatile bool isKey2Active = false;              // KEY2激活状态
volatile bool isTriggerActive = false;

typedef struct {
    float w, x, y, z;
} Quaternion;

typedef struct {
    float yaw, pitch, roll;  // 单位：角度
} EulerAngle;

Quaternion q_zero;      // 零点
Quaternion q_zero_conj; // 零点的逆
Quaternion q_hand;
Quaternion q_hand_delta;
Quaternion q_fore;
Quaternion q_fore_rel;
Quaternion q_fore_conj;
Quaternion q_hand_rel;
Quaternion q_upper_rel;

EulerAngle upper_angle;
EulerAngle fore_angle;
EulerAngle hand_angle;

volatile bool is_calibrated  = false;

 // 四元数共轭（逆）q* = (w, -x, -y, -z)
void quat_conjugate(const Quaternion *q, Quaternion *result) 
{
    result->w =  q->w;
    result->x = -q->x;
    result->y = -q->y;
    result->z = -q->z;
}

// 四元数乘法 q1 ⊗ q2
void quat_multiply(const Quaternion *q1, const Quaternion *q2, Quaternion *result) 
{
    result->w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y - q1->z * q2->z;
    result->x = q1->w * q2->x + q1->x * q2->w + q1->y * q2->z - q1->z * q2->y;
    result->y = q1->w * q2->y - q1->x * q2->z + q1->y * q2->w + q1->z * q2->x;
    result->z = q1->w * q2->z + q1->x * q2->y - q1->y * q2->x + q1->z * q2->w;
}

// 四元数转欧拉角 (ZYX顺序: yaw-pitch-roll)
void quat_to_euler(const Quaternion *q, EulerAngle *euler) 
{
    // yaw (Z轴旋转)
    float sinr_cosp = 2.0f * (q->w * q->z + q->x * q->y);
    float cosr_cosp = 1.0f - 2.0f * (q->y * q->y + q->z * q->z);
    euler->yaw = atan2f(sinr_cosp, cosr_cosp) * 180 / PI;
    
    // pitch (Y轴旋转)
    float sinp = 2.0f * (q->w * q->y - q->z * q->x);
    if (fabsf(sinp) >= 1.0f)
        euler->pitch = copysignf(PI / 2.0f, sinp) * 180 / PI;
    else
        euler->pitch = asinf(sinp) * 180 / PI;
    
    // roll (X轴旋转)
    float siny_cosp = 2.0f * (q->w * q->x + q->y * q->z);
    float cosy_cosp = 1.0f - 2.0f * (q->x * q->x + q->y * q->y);
    euler->roll = atan2f(siny_cosp, cosy_cosp) * 180 / PI;
}

void arm_calibrate(void)
{
    q_zero.w = eskf_state.qw_f;
    q_zero.x = eskf_state.qx_f;
    q_zero.y = eskf_state.qy_f;
    q_zero.z = eskf_state.qz_f;
    
    is_calibrated = true;
}

void ADC_Read(void)
{
    R_ADC_ScanStart(g_adc0.p_ctrl);
    while(scan_complete_flag == false);
    scan_complete_flag = false;

    R_ADC_Read(g_adc0.p_ctrl, ADC_CHANNEL_9, (uint16_t *)&ADC_9_value);
}

void PWM_Set(uint8_t duty)  // 0-100
{
    if(duty < 0)
        duty = 0;
    if(duty > 100)
        duty = 100;
    timer_info_t info;
    
    R_GPT_InfoGet(g_timer_gpt2.p_ctrl, &info);
    R_GPT_DutyCycleSet(g_timer_gpt2.p_ctrl, (uint32_t)(((uint64_t) info.period_counts * duty) / 100), GPT_IO_PIN_GTIOCA);
}

void float_trans_int(float data, uint8_t* out)
{
    union {
        float f;
        uint8_t bytes[4];
    } converter;
    
    converter.f = data;
    
    // STM32是小端架构，需要转换为大端序
    out[0] = converter.bytes[3];  // 最高字节
    out[1] = converter.bytes[2];
    out[2] = converter.bytes[1];
    out[3] = converter.bytes[0];  // 最低字节
}

float big_endian_bytes_to_float(const uint8_t* buffer)
{
    union {
        float f;
        uint8_t bytes[4];
    } converter;

    converter.bytes[3] = buffer[0]; 
    converter.bytes[2] = buffer[1];
    converter.bytes[1] = buffer[2];
    converter.bytes[0] = buffer[3]; 

    return converter.f;
}
    
void initializeSensors(void)
{
    fsp_err_t err = FSP_SUCCESS;
    
    {
        R_SPI_Open(g_spi1.p_ctrl, g_spi1.p_cfg);
        assert(err == FSP_SUCCESS);
    }
    
    {
        R_IIC_MASTER_Open(g_i2c_master1.p_ctrl, g_i2c_master1.p_cfg);
        assert(err == FSP_SUCCESS);
    }
    
    // 开启外部中断->ICM42688
    {
        err = R_ICU_ExternalIrqOpen(g_external_irq1.p_ctrl, g_external_irq1.p_cfg);
        assert(err == FSP_SUCCESS);
        err = R_ICU_ExternalIrqEnable(g_external_irq1.p_ctrl);
        assert(err == FSP_SUCCESS);
    }
    
    res_qmc5883p = qmc5883p.begin();
    
    res_icm42688 = icm42688.begin();
    
    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

    res_general_icm42688 = icm42688.general.enable_acc(ICM42688::General::ACCEL_ODR::ACCEL_ODR_200HZ, 
                               ICM42688::General::ACCEL_FS::ACCEL_FS_2G, 
                               ICM42688::General::FILTER_LEVEL::MEDIUM);

    res_general_icm42688 = icm42688.general.enable_gyro(ICM42688::General::GYRO_ODR::GYRO_ODR_200HZ,
                                ICM42688::General::GYRO_FS::GYRO_FS_125DPS,
                                ICM42688::General::FILTER_LEVEL::MEDIUM);

    R_BSP_SoftwareDelay(200, BSP_DELAY_UNITS_MILLISECONDS);

    res_general_icm42688 = icm42688.general.set_acc_gyro_offset(500);

    res_int_icm42688 = icm42688.Int.set_int1_pin_cfg(ICM42688::INT::LEVEL::LEVEL_ACTIVE_HIGH,
                                                ICM42688::INT::DRIVE::DRIVE_PUSH_PULL,
                                                ICM42688::INT::MODE::LATCHED);

    res_int_icm42688 = icm42688.Int.register_event(ICM42688::INT::IntEvent::DATA_RDY_INT,
                                                ICM42688::INT::INT_NUM::INT1,
                                                ICM42688_dataRady,
                                                NULL,
                                                ICM42688::INT::EVENT_PRIORITY::PRIORITY_HIGH);

    R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
}

void initializePeripherals(void)
{
    fsp_err_t err = FSP_SUCCESS;
    
    // 开启串口1
    {
        err = R_SCI_UART_Open(g_uart1.p_ctrl, g_uart1.p_cfg);
        assert(err == FSP_SUCCESS);
    }
    // 开启串口2
    {
        err = R_SCI_UART_Open(g_uart2.p_ctrl, g_uart2.p_cfg);
        assert(err == FSP_SUCCESS);
    }
    // 开启外部中断->KEY1
    {
        err = R_ICU_ExternalIrqOpen(g_external_irq0.p_ctrl, g_external_irq0.p_cfg);
        assert(err == FSP_SUCCESS);
        err = R_ICU_ExternalIrqEnable(g_external_irq0.p_ctrl);
        assert(err == FSP_SUCCESS);
    }
    // 开启外部中断->KEY2
    {
        err = R_ICU_ExternalIrqOpen(g_external_irq9.p_ctrl, g_external_irq9.p_cfg);
        assert(err == FSP_SUCCESS);
        err = R_ICU_ExternalIrqEnable(g_external_irq9.p_ctrl);
        assert(err == FSP_SUCCESS);
    }
    // 开启ADC
    {
        err = R_ADC_Open(g_adc0.p_ctrl, g_adc0.p_cfg);
        assert(err == FSP_SUCCESS);
        err = R_ADC_ScanCfg(g_adc0.p_ctrl, g_adc0.p_channel_cfg);
        assert(err == FSP_SUCCESS);
    }
    // 输出PWM开启
    {
        R_GPT_Open(g_timer_gpt2.p_ctrl, g_timer_gpt2.p_cfg);
        R_GPT_Start(g_timer_gpt2.p_ctrl);
    }
    PWM_Set(0);
}

void initializeTimers(void)
{
    fsp_err_t err = FSP_SUCCESS;
    
    // 开启定时器main
    {
        err = R_AGT_Open(g_timer_main.p_ctrl, g_timer_main.p_cfg);
        assert(err == FSP_SUCCESS);
        err = R_AGT_Start(g_timer_main.p_ctrl);
        assert(err == FSP_SUCCESS);
        err = R_AGT_Enable(g_timer_main.p_ctrl);
        assert(err == FSP_SUCCESS);
    }
    // 开启定时器systick
    {
        err = R_AGT_Open(g_systick.p_ctrl, g_systick.p_cfg);
        assert(err == FSP_SUCCESS);
        err = R_AGT_Start(g_systick.p_ctrl);
        assert(err == FSP_SUCCESS);
        err = R_AGT_Enable(g_systick.p_ctrl);
        assert(err == FSP_SUCCESS);
    }
}


// ADC中断回调函数
void g_adc_callback(adc_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    scan_complete_flag = true;
}

// 串口1中断回调函数      DEBUG
void g_uart1_callback(uart_callback_args_t *p_args)
{           
    switch(p_args->event)
    {
        case UART_EVENT_RX_COMPLETE:
        {
            uart1_receive_complete_flag = true;
            break;
        }
        case UART_EVENT_RX_CHAR:
        {
            
            break;
        }
        case UART_EVENT_TX_COMPLETE:
        {
            uart1_send_complete_flag = true;
            break;
        }
        default:
            break;
    }
}

// 串口2中断回调函数      HC06
void g_uart2_callback(uart_callback_args_t *p_args)
{
    switch(p_args->event) 
    {
        case UART_EVENT_RX_COMPLETE:
        {
            uart2_receive_complete_flag = true;
            break;
        }
        case UART_EVENT_RX_CHAR:
        {
            
            break;
        }
        case UART_EVENT_TX_COMPLETE:
        {
            uart2_send_complete_flag = true;
            break;
        }
        default:
            break;
    }
}

// 定时器中断回调函数main
void g_timer_main_callback(timer_callback_args_t *p_args)
{
    if(p_args->event == TIMER_EVENT_CYCLE_END)
    {
        ADC_Read();
        PWM_Set((uint8_t)((((float)ADC_9_value/ 4096) * ADC_PWM_truly_value + 1 - ADC_PWM_truly_value)));
    
        eskf_update(&eskf_state, icm42688.general.get_gx() - gyro_bias_x, (icm42688.general.get_gy() - gyro_bias_y), (icm42688.general.get_gz() - gyro_bias_z) * 8.87f,
                                icm42688.general.get_ax(), icm42688.general.get_ay(), icm42688.general.get_az(),
                                qmc5883p.getX(), qmc5883p.getY(), qmc5883p.getZ());
        
//        eskf_update(&eskf_state, icm42688.general.get_gx() - gyro_bias_x, (icm42688.general.get_gz() - gyro_bias_z), (icm42688.general.get_gy() - gyro_bias_y) * 8.87f,
//                                icm42688.general.get_ax(), -icm42688.general.get_az(), icm42688.general.get_ay(),
//                                qmc5883p.getX(), qmc5883p.getY(), qmc5883p.getZ());
    
        main_timer_call = true;
    }
}

// 定时器中断回调函数systick
void g_systick_callback(timer_callback_args_t *p_args)      // us
{
    if(p_args->event == TIMER_EVENT_CYCLE_END)
    {
        sysTick++;
    }
}

// 外部中断回调函数
void g_external_irq_callback(external_irq_callback_args_t *p_args)
{
    switch(p_args->channel)
    {
        case 0:
        {
            // KEY1
            isKey1Active = true;
            break;
        }
        case 1:
        {
            // ICM42688
            icm42688.Int.process_interrupts(ICM42688::INT::INT_NUM::INT1);
            break;
        }
        case 2:
        {
            // Trigger
            isTriggerActive = true;
            break;
        }
        case 9:
        {
            // KEY2
            isKey2Active = true;
            
            break;
        }
        default:
            break;
    }
}

void cpp_main(void) 
{
    initializePeripherals();
    initializeSensors();
    initializeTimers();
    
    eskf_init(&eskf_state);
    
    R_IOPORT_PinWrite(&g_ioport_ctrl, LED2, BSP_IO_LEVEL_HIGH);
    R_IOPORT_PinWrite(&g_ioport_ctrl, LED1, BSP_IO_LEVEL_HIGH);
    PWM_Set(90);
    R_BSP_SoftwareDelay(300, BSP_DELAY_UNITS_MILLISECONDS);
    PWM_Set(0);
    R_BSP_SoftwareDelay(300, BSP_DELAY_UNITS_MILLISECONDS);
    PWM_Set(90);
    R_BSP_SoftwareDelay(300, BSP_DELAY_UNITS_MILLISECONDS);
    PWM_Set(0);
    
    R_SCI_UART_Read(&g_uart1_ctrl, receivePacket1, UART1_RECEIVE_LEN);
    R_SCI_UART_Read(&g_uart2_ctrl, receivePacket2, UART2_RECEIVE_LEN);

    while(1)
    {   
        if(main_timer_call == true)
        {
            main_timer_call = false;
            
            q_hand.w = eskf_state.qw_f;
            q_hand.x = eskf_state.qx_f;
            q_hand.y = eskf_state.qy_f;
            q_hand.z = eskf_state.qz_f;
            
            if(uart1_receive_complete_flag == true)
            {
                uart1_receive_complete_flag = false;
                
//                if(receivePacket1[0] == 0xAF && receivePacket1[1] == 0x01 && receivePacket1[26] == 0xFA)
                if((receivePacket1[0] == 0xAF && receivePacket1[1] == 0x01 && receivePacket1[50] == 0xFA) && (is_calibrated == true))
                {
                     // #define DEBUG_MODE

                    q_upper_rel.w = big_endian_bytes_to_float(receivePacket1 + 2);
                    q_upper_rel.x = big_endian_bytes_to_float(receivePacket1 + 6);
                    q_upper_rel.y = big_endian_bytes_to_float(receivePacket1 + 10);
                    q_upper_rel.z = big_endian_bytes_to_float(receivePacket1 + 14);
                    q_fore_rel.w = big_endian_bytes_to_float(receivePacket1 + 18);
                    q_fore_rel.x = big_endian_bytes_to_float(receivePacket1 + 22);
                    q_fore_rel.y = big_endian_bytes_to_float(receivePacket1 + 26);
                    q_fore_rel.z = big_endian_bytes_to_float(receivePacket1 + 30);
                    q_fore.w = big_endian_bytes_to_float(receivePacket1 + 34);
                    q_fore.x = big_endian_bytes_to_float(receivePacket1 + 38);
                    q_fore.y = big_endian_bytes_to_float(receivePacket1 + 42);
                    q_fore.z = big_endian_bytes_to_float(receivePacket1 + 46);
                    
                    quat_conjugate(&q_zero, &q_zero_conj);
                    quat_multiply(&q_zero_conj, &q_hand, &q_hand_delta);
                    
                    quat_conjugate(&q_fore, &q_fore_conj);
                    quat_multiply(&q_fore_conj, &q_hand_delta, &q_hand_rel);
                    
                    quat_to_euler(&q_hand_rel, &hand_angle);
                    quat_to_euler(&q_fore_rel, &fore_angle);
                    quat_to_euler(&q_upper_rel, &upper_angle);
                    
                    #ifdef DEBUG_MODE
                    
                    sprintf((char *)debug_buf, "%f,%f,%f,%f,%f,%f\n", upper_angle.pitch, upper_angle.roll, upper_angle.yaw, fore_angle.roll, fore_angle.yaw, hand_angle.roll);
                    R_SCI_UART_Write(g_uart2.p_ctrl, debug_buf, strlen((const char*)debug_buf));
                    while(uart2_send_complete_flag == false);
                    uart2_send_complete_flag = false;
                    
                    #endif
                    #ifndef DEBUG_MODE
                    
                    sendbuf[0] = 0xAF;
                    sendbuf[1] = 0x01;
                    float_trans_int(upper_angle.pitch, sendbuf + 2);     // upper
                    float_trans_int(upper_angle.roll, sendbuf + 6);
                    float_trans_int(upper_angle.yaw, sendbuf + 10);
                    float_trans_int(fore_angle.roll, sendbuf + 14);      // fore
                    float_trans_int(fore_angle.yaw, sendbuf + 18);
                    float_trans_int(hand_angle.roll, sendbuf + 22);      //hand
                    sendbuf[26] = (uint8_t)((ADC_9_value & 0xFF00) >> 8);       // 夹爪
                    sendbuf[27] = (uint8_t)(ADC_9_value & 0x00FF);
                    sendbuf[28] = 0xFA;
                    
                    R_SCI_UART_Write(g_uart2.p_ctrl, sendbuf, 29);
                    while(uart2_send_complete_flag == false);
                    uart2_send_complete_flag = false;
                    
                    #endif
                }
                
                R_SCI_UART_Read(&g_uart1_ctrl, receivePacket1, UART1_RECEIVE_LEN);
            }
        }
        
        if(uart2_receive_complete_flag == true)
        {
            uart2_receive_complete_flag = false;
            
            if(receivePacket2[0] == 0xAF && receivePacket2[1] == 0x04 && receivePacket2[4] == 0xFA)
            {
                uint16_t pressure = receivePacket2[2] << 8 | receivePacket2[3];
                PWM_Set((uint8_t)(1.0f / (float)pressure * 100));
            }
            
            R_SCI_UART_Read(&g_uart2_ctrl, receivePacket2, UART2_RECEIVE_LEN);
        }
        
        if(isKey1Active == true)
        {
            isKey1Active = false;
            
            R_SCI_UART_Write(g_uart1.p_ctrl, sendbuf_key1, 3);
            while(uart1_send_complete_flag == false);
            uart1_send_complete_flag = false;
            
            arm_calibrate();
        }
        else if(isTriggerActive == true)
        {
            isTriggerActive = false;
            
            R_SCI_UART_Write(g_uart1.p_ctrl, sendbuf_key2, 3);
            while(uart1_send_complete_flag == false);
            uart1_send_complete_flag = false;
        }
    }
}
