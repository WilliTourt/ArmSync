#include "r_canfd.h"
#include "r_canfd_cfg.h"

const canfd_afl_entry_t p_canfd0_afl[CANFD_CFG_AFL_CH0_RULE_NUM] =
{
    {
        .id =
        {
            /* Specify the ID, ID type and frame type to accept. */
            .id         = 0x000,
            .frame_type = CAN_FRAME_TYPE_DATA,
            .id_mode    = CAN_ID_MODE_EXTENDED
        },

        .mask =
        {
            /* These values mask which ID/mode bits to compare when filtering messages. */
            .mask_id         = 0x000,
            .mask_frame_type = 1,
            .mask_id_mode    = 1
        },

        .destination =
        {
            /* If DLC checking is enabled any messages shorter than the below setting will be rejected. */
            .minimum_dlc = CANFD_MINIMUM_DLC_0,

            /* Optionally specify a Receive Message Buffer (RX MB) to store accepted frames. RX MBs do not have an
             * interrupt or overwrite protection and must be checked with R_CANFD_InfoGet and R_CANFD_Read. */
            .rx_buffer   = CANFD_RX_MB_NONE,

            /* Specify which FIFO(s) to send filtered messages to. Multiple FIFOs can be OR'd together. */
            .fifo_select_flags = CANFD_RX_FIFO_0
        }
    },
};
