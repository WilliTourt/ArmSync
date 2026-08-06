/**
 * @file    ipc_frame.h
 * @brief   ArmSync 双核间通信 (IPC) 帧结构定义
 * 
 * 本文件定义 CPU0 (M85) 与 CPU1 (M33) 之间通过共享内存传递的
 * 数据帧格式。两个工程必须引用同一份头文件，确保结构体对齐一致。
 * 
 * 帧格式: [0xA5][7×float32][0x5A] = 30 字节
 */

#ifndef IPC_FRAME_H
#define IPC_FRAME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 机械臂关节数量 (J1-J6 + 夹爪) */
#define IPC_JOINT_COUNT  6
#define IPC_GRIPPER_IDX  6

/** IPC 帧接收状态机 */
typedef enum {
    IPC_WAIT_HEADER = 0,   /**< 等待帧头 0xA5 */
    IPC_RECV_DATA,         /**< 接收7个float32 */
    IPC_WAIT_FOOTER        /**< 等待帧尾 0x5A */
} ipc_state_t;

/**
 * @brief IPC 数据帧 (30 字节)
 * 
 * 内存布局 (小端):
 *   [0]    0xA5              帧头
 *   [1]    joints[0]         J1 基座旋转       (float32, rad)
 *   [2]    joints[1]         J2 肩部俯仰       (float32, rad)
 *   [3]    joints[2]         J3 肩部偏航       (float32, rad)
 *   [4]    joints[3]         J4 肘部俯仰       (float32, rad)
 *   [5]    joints[4]         J5 腕部俯仰       (float32, rad)
 *   [6]    joints[5]         J6 腕部偏航       (float32, rad)
 *   [7]    gripper           夹爪开合比例     (float32, 0.0~1.0)
 *   [8]    0x5A              帧尾
 */
#pragma pack(push, 1)
typedef struct {
    uint8_t   header;                          /**< 帧头 0xA5 */
    float     joints[IPC_JOINT_COUNT];          /**< 6个关节目标角度 (rad) */
    float     gripper;                          /**< 夹爪开合比例 */
    uint8_t   footer;                           /**< 帧尾 0x5A */
} ipc_frame_t;
#pragma pack(pop)

/** IPC 帧总字节数 */
#define IPC_FRAME_SIZE  sizeof(ipc_frame_t)

/** 帧头/帧尾常量 */
#define IPC_HEADER  0xA5
#define IPC_FOOTER  0x5A

/**
 * @brief 校验 IPC 帧完整性
 * @param frame 待校验的帧指针
 * @return 1=有效, 0=无效
 */
static inline int ipc_frame_valid(const ipc_frame_t *frame) {
    return (frame->header == IPC_HEADER) && (frame->footer == IPC_FOOTER);
}

/**
 * @brief 打包角度数据到 IPC 帧
 * @param frame    输出帧指针
 * @param joints   6个关节角度数组 (rad)
 * @param gripper  夹爪开合比例 (0.0~1.0)
 */
static inline void ipc_frame_pack(ipc_frame_t *frame,
                                   const float joints[IPC_JOINT_COUNT],
                                   float gripper) {
    frame->header = IPC_HEADER;
    for (int i = 0; i < IPC_JOINT_COUNT; i++) {
        frame->joints[i] = joints[i];
    }
    frame->gripper = gripper;
    frame->footer = IPC_FOOTER;
}

#ifdef __cplusplus
}
#endif

#endif /* IPC_FRAME_H */
