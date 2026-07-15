#include "src\UserCode\cpp_main\cpp_main.h"

#include "src\UserCode\ICM42688\ICM42688.h"
#include "src\UserCode\QMC5883P\qmc5883p.h"
#include "common_data.h"
#include "fsp_common_api.h"
#include "hal_data.h"
#include "r_agt.h"
#include "r_sci_uart.h"
#include "r_timer_api.h"
#include "string.h"
#include "r_iic_master.h"
#include "r_ioport_cfg.h"               // Renesas::Flex Software:Build Configuration
#include "r_icu.h"


extern "C"{
#include "stdio.h"
#include "src\UserCode\ICM42688\ESKF.h"
}

//#define BigArm

#ifdef BigArm

#define UART1_RECEIVE_LEN   3

#endif
#ifndef BigArm

#define UART1_RECEIVE_LEN   3
#define UART2_RECEIVE_LEN   19

#endif

// sysTick
volatile uint32_t sysTick = 0;

// 传感器相关
ICM42688 icm42688(ICM42688::ICM42688_MountingOrientation::MOUNT_Y_DOWN);
QMC5883P qmc5883p(QMC5883P::QMC5883P_Mode::CONTINUOUS, QMC5883P::QMC5883P_Spd::ODR_100HZ, QMC5883P::QMC5883P_Rng::RNG_2G);

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
    
    float gx = icm42688.general.get_gx();
    float gy = icm42688.general.get_gy();
    float gz = icm42688.general.get_gz();
    
    const float a = 0.8f;
    
    float g_sqrt = sqrt(gx * gx + gy * gy + gz * gz);
    if(g_sqrt < 1.1f)
    {
        gyro_bias_x = a * gyro_bias_x + (1-a) * gx;
        gyro_bias_y = a * gyro_bias_y + (1-a) * gy;
        gyro_bias_z = a * gyro_bias_z + (1-a) * gz;
    }
}

// 串口发送完成标志
volatile bool uart1_send_complete_flag = false;
volatile bool uart2_send_complete_flag = false;
volatile bool uart1_receive_complete_flag = false;
volatile bool uart2_receive_complete_flag = false;

// 主循环定时器
volatile bool main_timer_call = false;

// 数据包空间
uint8_t sendbuf[64] = {0};
uint8_t receivePacket1[32] = {0};
uint8_t receivePacket2[32] = {0};
uint8_t sendbuf_key1[3] = {0xAF, 0x02, 0xFA};
uint8_t sendbuf_key2[3] = {0xAF, 0x03, 0xFA};

// debug存储空间
uint8_t debug_buf[128] = {0};

typedef struct {
    float w, x, y, z;
} Quaternion;

typedef struct {
    float yaw, pitch, roll;  // 单位：角度
} EulerAngle;

Quaternion q_zero;      // 零点
Quaternion q_zero_conj; // 零点的逆
volatile bool is_calibrated  = false;

#ifdef BigArm

Quaternion q_upper;    
Quaternion q_upper_delta;

#endif
#ifndef BigArm
    
Quaternion q_fore;    
Quaternion q_fore_delta;
Quaternion q_upper_delta;
Quaternion q_upper_delta_conj;
Quaternion q_fore_final;

EulerAngle angle_upper;
EulerAngle angle_rel;

#endif

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

// 串口1中断回调函数
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

// 串口2中断回调函数
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
        #ifdef BigArm
        eskf_update(&eskf_state, (icm42688.general.get_gz() - gyro_bias_z) * 8.87f, (icm42688.general.get_gy() - gyro_bias_y) * 8.87f, (icm42688.general.get_gx() - gyro_bias_x) * 8.87f,
                    icm42688.general.get_az(), icm42688.general.get_ay(), icm42688.general.get_ax(),
                    qmc5883p.getY(), qmc5883p.getZ(), qmc5883p.getX());
        #endif
        
        #ifndef BigArm
        eskf_update(&eskf_state, (icm42688.general.get_gz() - gyro_bias_z) * 8.87f, (icm42688.general.get_gy() - gyro_bias_y) * 8.87f, (icm42688.general.get_gx() - gyro_bias_x) * 8.87f,
                    icm42688.general.get_az(), -icm42688.general.get_ay(), -icm42688.general.get_ax(),
                    qmc5883p.getX(), qmc5883p.getY(), qmc5883p.getZ());
        #endif

        main_timer_call = true;
    }
}

// 定时器中断回调函数systick
void g_systick_callback(timer_callback_args_t *p_args)      // 1ms
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
        case 1:
        {
            // ICM42688
            icm42688.Int.process_interrupts(ICM42688::INT::INT_NUM::INT1);
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
    
    #ifdef BigArm
    R_SCI_UART_Read(&g_uart1_ctrl, receivePacket1, UART1_RECEIVE_LEN);
    
    #endif
    #ifndef BigArm
    R_SCI_UART_Read(&g_uart2_ctrl, receivePacket2, UART2_RECEIVE_LEN);
    R_SCI_UART_Read(&g_uart1_ctrl, receivePacket1, UART1_RECEIVE_LEN);
    #endif
    
    
    static uint8_t breath = 0;
    
    while(1)
    {   
        if(main_timer_call == true)
        {
            breath++;
            
            if(is_calibrated == true)
            {
                if(breath >= 100)
                {
                    R_IOPORT_PinWrite(&g_ioport_ctrl, LED1, BSP_IO_LEVEL_HIGH);
                    if(breath == 200) breath = 0;
                }
                else 
                    R_IOPORT_PinWrite(&g_ioport_ctrl, LED1, BSP_IO_LEVEL_LOW);
            }
            else
            {
                if(breath >= 20)
                {
                    R_IOPORT_PinWrite(&g_ioport_ctrl, LED1, BSP_IO_LEVEL_HIGH);
                    if(breath == 40) breath = 0;
                }
                else 
                    R_IOPORT_PinWrite(&g_ioport_ctrl, LED1, BSP_IO_LEVEL_LOW);
            }

            
            
            main_timer_call = false;
            
        #ifdef BigArm    // 大臂程序
            q_upper.w = eskf_state.qw_f;
            q_upper.x = eskf_state.qx_f;
            q_upper.y = eskf_state.qy_f;
            q_upper.z = eskf_state.qz_f;
            
            if(uart1_receive_complete_flag == true)
            {
                uart1_receive_complete_flag = false;

                if(receivePacket1[0] == 0xAF && receivePacket1[1] == 0x02 && receivePacket1[2] == 0xFA)
                    arm_calibrate();
                else if(receivePacket1[0] == 0xAF && receivePacket1[1] == 0x03 && receivePacket1[2] == 0xFA)
                    icm42688.general.set_acc_gyro_offset(100);
            
                R_SCI_UART_Read(&g_uart1_ctrl, receivePacket1, UART1_RECEIVE_LEN);
            }
            
            if(is_calibrated == true)
            {
                quat_conjugate(&q_zero, &q_zero_conj);
                quat_multiply(&q_zero_conj, &q_upper, &q_upper_delta);  // Δq_upper = q_upper_zero⁻¹ ⊗ q_upper    计算相对于零点的变化
                
                sendbuf[0] = 0xAF;
                sendbuf[1] = 0x01;
                float_trans_int(q_upper_delta.w, sendbuf + 2);          
                float_trans_int(q_upper_delta.x, sendbuf + 6);
                float_trans_int(q_upper_delta.y, sendbuf + 10);
                float_trans_int(q_upper_delta.z, sendbuf + 14);
                sendbuf[18] = 0xFA;
                
                R_SCI_UART_Write(g_uart1.p_ctrl, sendbuf, 19);
                while(uart1_send_complete_flag == false);
                uart1_send_complete_flag = false;
            }
            

        #endif
        
        #ifndef BigArm   // 小臂程序
        
            q_fore.w = eskf_state.qw_f;
            q_fore.x = eskf_state.qx_f;
            q_fore.y = eskf_state.qy_f;
            q_fore.z = eskf_state.qz_f;
        
            if(uart1_receive_complete_flag == true)
            {
                uart1_receive_complete_flag = false;

                if(receivePacket1[0] == 0xAF && receivePacket1[1] == 0x02 && receivePacket1[2] == 0xFA)
                {
                    R_SCI_UART_Write(g_uart2.p_ctrl, sendbuf_key1, 3);
                    while(uart2_send_complete_flag == false);
                    uart2_send_complete_flag = false;
                    
                    arm_calibrate();
                }
                else if(receivePacket1[0] == 0xAF && receivePacket1[1] == 0x03 && receivePacket1[2] == 0xFA)
                {
                    R_SCI_UART_Write(g_uart2.p_ctrl, sendbuf_key2, 3);
                    while(uart2_send_complete_flag == false);
                    uart2_send_complete_flag = false;
                    
                    icm42688.general.set_acc_gyro_offset(100);
                }
                
                R_SCI_UART_Read(&g_uart1_ctrl, receivePacket1, UART1_RECEIVE_LEN);
            }
            
            if(uart2_receive_complete_flag == true)
            {
                uart2_receive_complete_flag = false;
                if(receivePacket2[0] == 0xAF && receivePacket2[1] == 0x01 && receivePacket2[18] == 0xFA)
                {
                    q_upper_delta.w = big_endian_bytes_to_float(receivePacket2 + 2);
                    q_upper_delta.x = big_endian_bytes_to_float(receivePacket2 + 6);
                    q_upper_delta.y = big_endian_bytes_to_float(receivePacket2 + 10);
                    q_upper_delta.z = big_endian_bytes_to_float(receivePacket2 + 14);
                }
                
                R_SCI_UART_Read(&g_uart2_ctrl, receivePacket2, UART2_RECEIVE_LEN);
                
                if((is_calibrated == true) && (!(q_upper_delta.w == 0 && q_upper_delta.x == 0 && q_upper_delta.y == 0 && q_upper_delta.z == 0)))
                {
                    quat_conjugate(&q_zero, &q_zero_conj);
                    quat_multiply(&q_zero_conj, &q_fore, &q_fore_delta);    // Δq_fore = q_fore_zero⁻¹ ⊗ q_fore    计算相对于零点的变化
                    
                    quat_conjugate(&q_upper_delta, &q_upper_delta_conj);    // q_rel = Δq_upper⁻¹ ⊗ Δq_fore     计算小臂相对大臂的相对姿态
                    quat_multiply(&q_upper_delta_conj, &q_fore_delta, &q_fore_final);
                    
                    quat_to_euler(&q_upper_delta, &angle_upper);
                    quat_to_euler(&q_fore_final, &angle_rel);
                    
                    sendbuf[0] = 0xAF;
                    sendbuf[1] = 0x01;
                    float_trans_int(q_upper_delta.w, sendbuf + 2);
                    float_trans_int(q_upper_delta.x, sendbuf + 6);
                    float_trans_int(q_upper_delta.y, sendbuf + 10);
                    float_trans_int(q_upper_delta.z, sendbuf + 14);
                    float_trans_int(q_fore_final.w, sendbuf + 18);
                    float_trans_int(q_fore_final.x, sendbuf + 22);
                    float_trans_int(q_fore_final.y, sendbuf + 26);
                    float_trans_int(q_fore_final.z, sendbuf + 30);
                    float_trans_int(q_fore_delta.w, sendbuf + 34);
                    float_trans_int(q_fore_delta.x, sendbuf + 38);
                    float_trans_int(q_fore_delta.y, sendbuf + 42);
                    float_trans_int(q_fore_delta.z, sendbuf + 46);
                    sendbuf[50] = 0xFA;
                    
                    R_SCI_UART_Write(g_uart1.p_ctrl, sendbuf, 51);
                    while(uart1_send_complete_flag == false);
                    uart1_send_complete_flag = false;
                    
//                    sendbuf[0] = 0xAF;
//                    sendbuf[1] = 0x01;
//                    float_trans_int(angle_upper.pitch, sendbuf + 2);
//                    float_trans_int(angle_upper.roll, sendbuf + 6);
//                    float_trans_int(angle_upper.yaw, sendbuf + 10);
//                    float_trans_int(angle_rel.pitch, sendbuf + 14);
//                    float_trans_int(angle_rel.roll, sendbuf + 18);
//                    float_trans_int(angle_rel.yaw, sendbuf + 22);
//                    sendbuf[26] = 0xFA;
//                    
//                    R_SCI_UART_Write(g_uart1.p_ctrl, sendbuf, 27);
//                    while(uart1_send_complete_flag == false);
//                    uart1_send_complete_flag = false;
                }
                
//                    sprintf((char *)debug_buf, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", q_upper_delta.w, q_upper_delta.x, q_upper_delta.y, q_upper_delta.z,
//                                                                                        q_fore_delta.w, q_fore_delta.x, q_fore_delta.y, q_fore_delta.z,
//                                                                                        q_fore_final.w, q_fore_final.x, q_fore_final.y, q_fore_final.z);
//                    R_SCI_UART_Write(g_uart1.p_ctrl, debug_buf, strlen((const char*)debug_buf));
//                    while(uart1_send_complete_flag == false);
//                    uart1_send_complete_flag = false;
            }
        
        #endif

            
            
            
            
            
//            sprintf((char *)debug_buf, "%f,%f,%f,%f\n", eskf_state.qw_f, eskf_state.qx_f, eskf_state.qy_f, eskf_state.qz_f);
//            sprintf((char *)debug_buf, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", icm42688.general.get_ax(), icm42688.general.get_ay(), icm42688.general.get_az(),
//                                                                                icm42688.general.get_gx(), icm42688.general.get_gy(), icm42688.general.get_gz(), 
//                                                                                qmc5883p.getX(), qmc5883p.getY(), qmc5883p.getZ(),
//                                                                                eskf_state.pitch, eskf_state.roll, eskf_state.yaw);
//            R_SCI_UART_Write(g_uart1.p_ctrl, debug_buf, strlen((const char*)debug_buf));
//            while(uart1_send_complete_flag == false);
//            uart1_send_complete_flag = false;
           
            
//            R_SCI_UART_Write(g_uart2.p_ctrl, sendbuf, 21);
//            while(uart2_send_complete_flag == false);
//            uart2_send_complete_flag = false;
            
        }
    }
}
