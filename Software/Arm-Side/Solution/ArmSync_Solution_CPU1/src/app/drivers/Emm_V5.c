#include "Emm_V5.h"
#include "hal_data.h"
#include "stdint.h"
#include <string.h>

#define EMM_CAN_MAX_DATA_LENGTH       (8U)
#define EMM_CAN_COMMAND_DATA_LENGTH   (7U)
#define EMM_AT_FRAME_OVERHEAD         (9U)
#define EMM_UART_TX_BUFFER_SIZE       (EMM_CAN_MAX_DATA_LENGTH + EMM_AT_FRAME_OVERHEAD)
#define EMM_UART_WAIT_STEP_US         (100U)
#define EMM_UART_TX_TIMEOUT_STEPS     (500U)
#define EMM_AT_RESPONSE_TIMEOUT_STEPS (1000U)
#define EMM_AT_EXTENDED_FRAME_FLAG    (0x04U)
#define EMM_AT_RESPONSE_LENGTH        (4U)
#define EMM_AT_MODE_ATTEMPTS          (3U)
#define EMM_AT_RETRY_DELAY_MS         (100U)

static volatile bool g_emm_uart_tx_busy = false;
static volatile bool g_emm_at_waiting_for_ok = false;
static volatile bool g_emm_at_mode_ready = false;
static volatile bool g_emm_uart_rx_active = false;
static volatile uint8_t g_emm_at_response_state = 0U;
static uint8_t g_emm_uart_tx_buffer[EMM_UART_TX_BUFFER_SIZE];
static uint8_t g_emm_uart_rx_buffer[EMM_AT_RESPONSE_LENGTH];



static fsp_err_t Emm_V5_WaitForFlag(volatile bool const * p_flag, bool expected, uint32_t timeout_steps) {
	while ((*p_flag != expected) && (timeout_steps > 0U)) {
	  	R_BSP_SoftwareDelay(EMM_UART_WAIT_STEP_US, BSP_DELAY_UNITS_MICROSECONDS);
	  	--timeout_steps;
	}

	return (*p_flag == expected) ? FSP_SUCCESS : FSP_ERR_TIMEOUT;
}

static fsp_err_t Emm_V5_UartWrite(uint8_t const * p_data, uint32_t length) {
	if ((NULL == p_data) || (0U == length) || (length > sizeof(g_emm_uart_tx_buffer))) {
	  	return FSP_ERR_INVALID_ARGUMENT;
	}

	fsp_err_t err = Emm_V5_WaitForFlag(&g_emm_uart_tx_busy, false, EMM_UART_TX_TIMEOUT_STEPS);
	if (FSP_SUCCESS != err) {
	  	return err;
	}

	memcpy(g_emm_uart_tx_buffer, p_data, length);
	g_emm_uart_tx_busy = true;

	err = g_uart1.p_api->write(g_uart1.p_ctrl, g_emm_uart_tx_buffer, length);
	if (FSP_SUCCESS != err) {
	  	g_emm_uart_tx_busy = false;
	  	return err;
	}

	return Emm_V5_WaitForFlag(&g_emm_uart_tx_busy, false, EMM_UART_TX_TIMEOUT_STEPS);
}

static fsp_err_t Emm_V5_SendATFrame(uint32_t can_id, uint8_t const * p_data, uint8_t data_length) {
	if ((NULL == p_data) || (data_length > EMM_CAN_MAX_DATA_LENGTH) || (can_id > 0x1FFFFFFFU)) {
	  	return FSP_ERR_INVALID_ARGUMENT;
	}

	uint8_t at_frame[EMM_UART_TX_BUFFER_SIZE] = {0U};
	uint32_t const frame_header = (can_id << 3U) | EMM_AT_EXTENDED_FRAME_FLAG;
	uint32_t index = 0U;

	at_frame[index++] = 'A';
	at_frame[index++] = 'T';
	at_frame[index++] = (uint8_t) (frame_header >> 24U);
	at_frame[index++] = (uint8_t) (frame_header >> 16U);
	at_frame[index++] = (uint8_t) (frame_header >> 8U);
	at_frame[index++] = (uint8_t) frame_header;
	at_frame[index++] = data_length;
	memcpy(&at_frame[index], p_data, data_length);
	index += data_length;
	at_frame[index++] = '\r';
	at_frame[index++] = '\n';

	fsp_err_t const err = Emm_V5_UartWrite(at_frame, index);
	R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
	return err;
}

static fsp_err_t Emm_V5_Send(uint8_t const * p_cmd, uint32_t length) {
	if ((NULL == p_cmd) || (length < 2U)) {
	  	return FSP_ERR_INVALID_ARGUMENT;
	}

	uint32_t command_offset = 2U;
	uint32_t packet_number = 0U;

	do {
		uint8_t can_data[EMM_CAN_MAX_DATA_LENGTH] = {0U};
		uint32_t const remaining = length - command_offset;
		uint8_t const chunk_length = (uint8_t) ((remaining > EMM_CAN_COMMAND_DATA_LENGTH) ?
		                                         EMM_CAN_COMMAND_DATA_LENGTH : remaining);

		can_data[0] = p_cmd[1];
		if (chunk_length > 0U) {
		  memcpy(&can_data[1], &p_cmd[command_offset], chunk_length);
		}

		uint32_t const can_id = ((uint32_t) p_cmd[0] << 8U) | packet_number;
		fsp_err_t const err = Emm_V5_SendATFrame(can_id, can_data, (uint8_t) (chunk_length + 1U));
		if (FSP_SUCCESS != err) {
		  return err;
		}

		command_offset += chunk_length;
		++packet_number;
	} while (command_offset < length);

	return FSP_SUCCESS;
}

bool Emm_V5_Init(void) {
	static uint8_t const enter_at_mode[][7] = {
	 	{'A', 'T', '+', 'A', 'T', '\r', '\n'},
	 	{'A', 'T', '=', 'A', 'T', '\r', '\n'},
	};

	for (uint32_t attempt = 0U; attempt < EMM_AT_MODE_ATTEMPTS; ++attempt) {
	 	uint32_t const command_index = (1U == attempt) ? 1U : 0U;

	 	g_emm_at_mode_ready = false;
	 	g_emm_at_response_state = 0U;
	 	g_emm_at_waiting_for_ok = true;
	 	memset(g_emm_uart_rx_buffer, 0, sizeof(g_emm_uart_rx_buffer));

	 	fsp_err_t const read_err = g_uart1.p_api->read(g_uart1.p_ctrl,
	 	                                               g_emm_uart_rx_buffer,
	 	                                               sizeof(g_emm_uart_rx_buffer));
	 	g_emm_uart_rx_active = (FSP_SUCCESS == read_err);

	 	fsp_err_t const write_err = Emm_V5_UartWrite(enter_at_mode[command_index],
	 	                                             sizeof(enter_at_mode[command_index]));
	 	if ((FSP_SUCCESS == write_err) &&
	 	    (FSP_SUCCESS == Emm_V5_WaitForFlag(&g_emm_at_mode_ready,
	 	                                       true,
	 	                                       EMM_AT_RESPONSE_TIMEOUT_STEPS))) {
	 	  R_BSP_SoftwareDelay(10U, BSP_DELAY_UNITS_MILLISECONDS);
	 	  return true;
	 	}

	 	g_emm_at_waiting_for_ok = false;
	 	if (g_emm_uart_rx_active) {
	 	  (void) g_uart1.p_api->communicationAbort(g_uart1.p_ctrl, UART_DIR_RX);
	 	  g_emm_uart_rx_active = false;
	 	}

	 	R_BSP_SoftwareDelay(EMM_AT_RETRY_DELAY_MS, BSP_DELAY_UNITS_MILLISECONDS);
	}

	return false;
}

bool Emm_V5_IsReady(void) {
  	return g_emm_at_mode_ready;
}

static void Emm_V5_ProcessATResponseByte(uint8_t received) {
  	if ('O' == received) {
  	  g_emm_at_response_state = 1U;
  	} else if ((1U == g_emm_at_response_state) && ('K' == received)) {
  	  g_emm_at_mode_ready = true;
  	  g_emm_at_waiting_for_ok = false;
  	  g_emm_at_response_state = 0U;
  	} else if (('\r' != received) && ('\n' != received)) {
  	  g_emm_at_response_state = 0U;
  	}
}



void Emm_V5_UartCallback(uart_callback_args_t *p_args) {
	if (NULL == p_args) {
	  	return;
	}

	if (UART_EVENT_TX_COMPLETE == p_args->event) {
	  	g_emm_uart_tx_busy = false;
	} else if (UART_EVENT_RX_COMPLETE == p_args->event) {
	  	g_emm_uart_rx_active = false;
	  	if (g_emm_at_waiting_for_ok) {
	  	  for (uint32_t i = 0U; i < sizeof(g_emm_uart_rx_buffer); ++i) {
	  	    Emm_V5_ProcessATResponseByte(g_emm_uart_rx_buffer[i]);
	  	  }
	  	}
	} else if ((UART_EVENT_RX_CHAR == p_args->event) && g_emm_at_waiting_for_ok) {
	  	Emm_V5_ProcessATResponseByte((uint8_t) p_args->data);
	}
}



/**********************************************************
***	Emm_V5.0步进闭环控制例程
***	编写作者：ZHANGDATOU
***	技术支持：张大头闭环伺服
***	淘宝店铺：https://zhangdatou.taobao.com
***	CSDN博客：http s://blog.csdn.net/zhangdatou666
***	qq交流群：262438510
**********************************************************/

/**
  * @brief    将当前位置清零
  * @param    addr  ：电机地址
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Reset_CurPos_To_Zero(uint8_t addr) {
	uint8_t cmd[16] = {0};
	
	// 装载命令
	cmd[0] =  addr;                       // 地址
	cmd[1] =  0x0A;                       // 功能码
	cmd[2] =  0x6D;                       // 辅助码
	cmd[3] =  0x6B;                       // 校验字节
	
	// 发送命令
	Emm_V5_Send(cmd, 4);
}

/**
  * @brief    解除堵转保护
  * @param    addr  ：电机地址
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Reset_Clog_Pro(uint8_t addr) {
	uint8_t cmd[16] = {0};
	
	// 装载命令
	cmd[0] =  addr;                       // 地址
	cmd[1] =  0x0E;                       // 功能码
	cmd[2] =  0x52;                       // 辅助码
	cmd[3] =  0x6B;                       // 校验字节
	
	// 发送命令
	Emm_V5_Send(cmd, 4);
}

/**
  * @brief    读取系统参数
  * @param    addr  ：电机地址
  * @param    s     ：系统参数类型
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Read_Sys_Params(uint8_t addr, SysParams_t s) {
	uint8_t i = 0;
	uint8_t cmd[16] = {0};
	
	// 装载命令
	cmd[i] = addr; ++i;                   // 地址

	// 功能码
	switch(s) {
	  	case S_VER  : cmd[i] = 0x1F; ++i; break;
	  	case S_RL   : cmd[i] = 0x20; ++i; break;
	  	case S_PID  : cmd[i] = 0x21; ++i; break;
	  	case S_VBUS : cmd[i] = 0x24; ++i; break;
	  	case S_CPHA : cmd[i] = 0x27; ++i; break;
	  	case S_ENCL : cmd[i] = 0x31; ++i; break;
	  	case S_TPOS : cmd[i] = 0x33; ++i; break;
	  	case S_VEL  : cmd[i] = 0x35; ++i; break;
	  	case S_CPOS : cmd[i] = 0x36; ++i; break;
	  	case S_PERR : cmd[i] = 0x37; ++i; break;
	  	case S_FLAG : cmd[i] = 0x3A; ++i; break;
	  	case S_ORG  : cmd[i] = 0x3B; ++i; break;
	  	case S_Conf : cmd[i] = 0x42; ++i; cmd[i] = 0x6C; ++i; break;
	  	case S_State: cmd[i] = 0x43; ++i; cmd[i] = 0x7A; ++i; break;
	  	default: break;
	}

	cmd[i] = 0x6B; ++i;                   // 校验字节
	
	// 发送命令
	Emm_V5_Send(cmd, i);
}

/**
  * @brief    修改开环/闭环控制模式
  * @param    addr     ：电机地址
  * @param    svF      ：是否存储标志，false为不存储，true为存储
  * @param    ctrl_mode：控制模式（对应屏幕上的P_Pul菜单），0是关闭脉冲输入引脚，1是开环模式，2是闭环模式，3是让En端口复用为多圈限位开关输入引脚，Dir端口复用为到位输出高电平功能
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Modify_Ctrl_Mode(uint8_t addr, bool svF, uint8_t ctrl_mode) {
	uint8_t cmd[16] = {0};
	
	// 装载命令
	cmd[0] =  addr;                       // 地址
	cmd[1] =  0x46;                       // 功能码
	cmd[2] =  0x69;                       // 辅助码
	cmd[3] =  svF;                        // 是否存储标志，false为不存储，true为存储
	cmd[4] =  ctrl_mode;                  // 控制模式（对应屏幕上的P_Pul菜单），0是关闭脉冲输入引脚，1是开环模式，2是闭环模式，3是让En端口复用为多圈限位开关输入引脚，Dir端口复用为到位输出高电平功能
	cmd[5] =  0x6B;                       // 校验字节
	
	// 发送命令
	Emm_V5_Send(cmd, 6);
}

/**
  * @brief    使能信号控制
  * @param    addr  ：电机地址
  * @param    state ：使能状态     ，true为使能电机，false为关闭电机
  * @param    snF   ：多机同步标志 ，false为不启用，true为启用
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_En_Control(uint8_t addr, bool state, bool snF) {
	uint8_t cmd[16] = {0};
	
	// 装载命令
	cmd[0] =  addr;                       // 地址
	cmd[1] =  0xF3;                       // 功能码
	cmd[2] =  0xAB;                       // 辅助码
	cmd[3] =  (uint8_t)state;             // 使能状态
	cmd[4] =  snF;                        // 多机同步运动标志
	cmd[5] =  0x6B;                       // 校验字节
	
	// 发送命令
	Emm_V5_Send(cmd, 6);
}

/**
  * @brief    速度模式
  * @param    addr：电机地址
  * @param    dir ：方向       ，0为CW，其余值为CCW
  * @param    vel ：速度       ，范围0 - 5000RPM
  * @param    acc ：加速度     ，范围0 - 255，注意：0是直接启动
  * @param    snF ：多机同步标志，false为不启用，true为启用
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Vel_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, bool snF) {
	uint8_t cmd[16] = {0};

	// 装载命令
	cmd[0] =  addr;                       // 地址
	cmd[1] =  0xF6;                       // 功能码
	cmd[2] =  dir;                        // 方向
	cmd[3] =  (uint8_t)(vel >> 8);        // 速度(RPM)高8位字节
	cmd[4] =  (uint8_t)(vel >> 0);        // 速度(RPM)低8位字节
	cmd[5] =  acc;                        // 加速度，注意：0是直接启动
	cmd[6] =  snF;                        // 多机同步运动标志
	cmd[7] =  0x6B;                       // 校验字节
	
	// 发送命令
	Emm_V5_Send(cmd, 8);
}

/**
  * @brief    位置模式
  * @param    addr：电机地址
  * @param    dir ：方向        ，0为CW，其余值为CCW
  * @param    vel ：速度(RPM)   ，范围0 - 5000RPM
  * @param    acc ：加速度      ，范围0 - 255，注意：0是直接启动
  * @param    clk ：脉冲数      ，范围0- (2^32 - 1)个
  * @param    raF ：相位/绝对标志，false为相对运动，true为绝对值运动
  * @param    snF ：多机同步标志 ，false为不启用，true为启用
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Pos_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, uint32_t clk, bool raF, bool snF) {
	uint8_t cmd[16] = {0};

	// 装载命令
	cmd[0]  =  addr;                      // 地址
	cmd[1]  =  0xFD;                      // 功能码
	cmd[2]  =  dir;                       // 方向
	cmd[3]  =  (uint8_t)(vel >> 8);       // 速度(RPM)高8位字节
	cmd[4]  =  (uint8_t)(vel >> 0);       // 速度(RPM)低8位字节 
	cmd[5]  =  acc;                       // 加速度，注意：0是直接启动
	cmd[6]  =  (uint8_t)(clk >> 24);      // 脉冲数(bit24 - bit31)
	cmd[7]  =  (uint8_t)(clk >> 16);      // 脉冲数(bit16 - bit23)
	cmd[8]  =  (uint8_t)(clk >> 8);       // 脉冲数(bit8  - bit15)
	cmd[9]  =  (uint8_t)(clk >> 0);       // 脉冲数(bit0  - bit7 )
	cmd[10] =  raF;                       // 相位/绝对标志，false为相对运动，true为绝对值运动
	cmd[11] =  snF;                       // 多机同步运动标志，false为不启用，true为启用
	cmd[12] =  0x6B;                      // 校验字节
	
	// 发送命令
	Emm_V5_Send(cmd, 13);
}

/**
  * @brief    立即停止（所有控制模式都通用）
  * @param    addr  ：电机地址
  * @param    snF   ：多机同步标志，false为不启用，true为启用
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Stop_Now(uint8_t addr, bool snF) {
	uint8_t cmd[16] = {0};
	
	// 装载命令
	cmd[0] =  addr;                       // 地址
	cmd[1] =  0xFE;                       // 功能码
	cmd[2] =  0x98;                       // 辅助码
	cmd[3] =  snF;                        // 多机同步运动标志
	cmd[4] =  0x6B;                       // 校验字节
	
	// 发送命令
	Emm_V5_Send(cmd, 5);
}

/**
  * @brief    多机同步运动
  * @param    addr  ：电机地址
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Synchronous_motion(uint8_t addr) {
	uint8_t cmd[16] = {0};
	
	// 装载命令
	cmd[0] =  addr;                       // 地址
	cmd[1] =  0xFF;                       // 功能码
	cmd[2] =  0x66;                       // 辅助码
	cmd[3] =  0x6B;                       // 校验字节
	
	// 发送命令
	Emm_V5_Send(cmd, 4);
}

/**
  * @brief    设置单圈回零的零点位置
  * @param    addr  ：电机地址
  * @param    svF   ：是否存储标志，false为不存储，true为存储
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Origin_Set_O(uint8_t addr, bool svF) {
	uint8_t cmd[16] = {0};
	
	// 装载命令
	cmd[0] =  addr;                       // 地址
	cmd[1] =  0x93;                       // 功能码
	cmd[2] =  0x88;                       // 辅助码
	cmd[3] =  svF;                        // 是否存储标志，false为不存储，true为存储
	cmd[4] =  0x6B;                       // 校验字节
	
	// 发送命令
	Emm_V5_Send(cmd, 5);
}

/**
  * @brief    修改回零参数
  * @param    addr  ：电机地址
  * @param    svF   ：是否存储标志，false为不存储，true为存储
  * @param    o_mode ：回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
  * @param    o_dir  ：回零方向，0为CW，其余值为CCW
  * @param    o_vel  ：回零速度，单位：RPM（转/分钟）
  * @param    o_tm   ：回零超时时间，单位：毫秒
  * @param    sl_vel ：无限位碰撞回零检测转速，单位：RPM（转/分钟）
  * @param    sl_ma  ：无限位碰撞回零检测电流，单位：Ma（毫安）
  * @param    sl_ms  ：无限位碰撞回零检测时间，单位：Ms（毫秒）
  * @param    potF   ：上电自动触发回零，false为不使能，true为使能
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Origin_Modify_Params(uint8_t addr, bool svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, bool potF) {
	uint8_t cmd[32] = {0};
	
	// 装载命令
	cmd[0] =  addr;                       // 地址
	cmd[1] =  0x4C;                       // 功能码
	cmd[2] =  0xAE;                       // 辅助码
	cmd[3] =  svF;                        // 是否存储标志，false为不存储，true为存储
	cmd[4] =  o_mode;                     // 回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
	cmd[5] =  o_dir;                      // 回零方向
	cmd[6]  =  (uint8_t)(o_vel >> 8);     // 回零速度(RPM)高8位字节
	cmd[7]  =  (uint8_t)(o_vel >> 0);     // 回零速度(RPM)低8位字节 
	cmd[8]  =  (uint8_t)(o_tm >> 24);     // 回零超时时间(bit24 - bit31)
	cmd[9]  =  (uint8_t)(o_tm >> 16);     // 回零超时时间(bit16 - bit23)
	cmd[10] =  (uint8_t)(o_tm >> 8);      // 回零超时时间(bit8  - bit15)
	cmd[11] =  (uint8_t)(o_tm >> 0);      // 回零超时时间(bit0  - bit7 )
	cmd[12] =  (uint8_t)(sl_vel >> 8);    // 无限位碰撞回零检测转速(RPM)高8位字节
	cmd[13] =  (uint8_t)(sl_vel >> 0);    // 无限位碰撞回零检测转速(RPM)低8位字节 
	cmd[14] =  (uint8_t)(sl_ma >> 8);     // 无限位碰撞回零检测电流(Ma)高8位字节
	cmd[15] =  (uint8_t)(sl_ma >> 0);     // 无限位碰撞回零检测电流(Ma)低8位字节 
	cmd[16] =  (uint8_t)(sl_ms >> 8);     // 无限位碰撞回零检测时间(Ms)高8位字节
	cmd[17] =  (uint8_t)(sl_ms >> 0);     // 无限位碰撞回零检测时间(Ms)低8位字节
	cmd[18] =  potF;                      // 上电自动触发回零，false为不使能，true为使能
	cmd[19] =  0x6B;                      // 校验字节
	
	// 发送命令
	Emm_V5_Send(cmd, 20);
}

/**
  * @brief    触发回零
  * @param    addr   ：电机地址
  * @param    o_mode ：回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
  * @param    snF   ：多机同步标志，false为不启用，true为启用
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Origin_Trigger_Return(uint8_t addr, uint8_t o_mode, bool snF) {
	uint8_t cmd[16] = {0};
	
	// 装载命令
	cmd[0] =  addr;                       // 地址
	cmd[1] =  0x9A;                       // 功能码
	cmd[2] =  o_mode;                     // 回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
	cmd[3] =  snF;                        // 多机同步运动标志，false为不启用，true为启用
	cmd[4] =  0x6B;                       // 校验字节
	
	// 发送命令
	Emm_V5_Send(cmd, 5);
}

/**
  * @brief    强制中断并退出回零
  * @param    addr  ：电机地址
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Origin_Interrupt(uint8_t addr) {
	uint8_t cmd[16] = {0};
	
	// 装载命令
	cmd[0] =  addr;                       // 地址
	cmd[1] =  0x9C;                       // 功能码
	cmd[2] =  0x48;                       // 辅助码
	cmd[3] =  0x6B;                       // 校验字节
	
	// 发送命令
	Emm_V5_Send(cmd, 4);
}
