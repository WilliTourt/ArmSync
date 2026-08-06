#include "hal_data.h"
#include "can.h"
#include "r_canfd.h"

#include <string.h>

__IO CAN_t can = {0};

static volatile bool      g_can_opened   = false;
static volatile bool      g_can_tx_done  = true;
static volatile fsp_err_t g_can_last_err = FSP_SUCCESS;

static bool can_TxBufferReady(uint32_t buffer)
{
    canfd_instance_ctrl_t * p_ctrl = (canfd_instance_ctrl_t *) g_canfd0.p_ctrl;

    if ((NULL == p_ctrl) || (NULL == p_ctrl->p_reg))
    {
        return false;
    }

    return (0U == p_ctrl->p_reg->CFDTMSTS_b[buffer].TMTRM);
}

void can_Init(void)
{
    if (g_can_opened)
    {
        return;
    }

    fsp_err_t err = g_canfd0.p_api->open(g_canfd0.p_ctrl, g_canfd0.p_cfg);
    g_can_last_err = err;
    if (FSP_SUCCESS != err)
    {
        return;
    }
    g_can_opened = true;

    /* Make sure we're in normal mode and NOT in internal loopback,
     * otherwise CAN_TX pin will not output waveforms.
     */
    err = g_canfd0.p_api->modeTransition(g_canfd0.p_ctrl, CAN_OPERATION_MODE_NORMAL, CAN_TEST_MODE_DISABLED);
    g_can_last_err = err;
}


/* FSP CANFD callback (configured in ra_gen/hal_data.c -> g_canfd0_cfg.p_callback). */
void canfd0_callback(can_callback_args_t * p_args)
{
    if (NULL == p_args)
    {
        return;
    }

    if (CAN_EVENT_RX_COMPLETE == p_args->event)
    {
        can.CAN_RxMsg   = p_args->frame;
        can.rxDlc       = p_args->frame.data_length_code;
        can.rxExtId     = p_args->frame.id;
        memcpy((void *) can.rxData, (void const *) p_args->frame.data, sizeof(can.rxData));
        can.rxFrameFlag = true;

        if ((CAN_ID_MODE_EXTENDED == p_args->frame.id_mode) &&
            (7U == p_args->frame.data_length_code) &&
            (0x36U == p_args->frame.data[0]))
        {
            uint32_t pos = ((uint32_t) p_args->frame.data[2] << 24) |
                           ((uint32_t) p_args->frame.data[3] << 16) |
                           ((uint32_t) p_args->frame.data[4] << 8)  |
                           ((uint32_t) p_args->frame.data[5]);

            float motor_pos = (float) pos * 360.0f / 65536.0f;
            if (0U != p_args->frame.data[1])
            {
                motor_pos = -motor_pos;
            }

            can.motorAddr          = (uint8_t) (p_args->frame.id >> 8);
            can.motorCurPosRaw     = pos;
            can.motorCurPosDeg     = motor_pos;
            can.motorCurPosUpdated = true;
        }
    }
    else if (CAN_EVENT_TX_COMPLETE == p_args->event)
    {
        g_can_tx_done = true;
    }
}

void can_SendCmd(__IO uint8_t * cmd, uint8_t len)
{
    __IO uint8_t i = 0, j = 0, k = 0, l = 0, packNum = 0;

    if ((NULL == cmd) || (len < 2))
    {
        return;
    }

    can_Init();
    if (!g_can_opened)
    {
        (void) g_can_last_err;
        return;
    }

    /* Data length excluding address and function code. */
    j = (uint8_t) (len - 2U);

    while (i < j)
    {
        k = (uint8_t) (j - i);

        /* Keep original STM32 packing logic:
         * - Extended ID: (addr<<8) | packNum
         * - data[0]     : function code
         * - remaining   : payload (up to 7 bytes each frame)
         */
        memset((void *) can.CAN_TxMsg.data, 0, sizeof(can.CAN_TxMsg.data));
        can.CAN_TxMsg.id      = ((uint32_t) cmd[0] << 8) | (uint32_t) packNum;
        can.CAN_TxMsg.id_mode = CAN_ID_MODE_EXTENDED;
        can.CAN_TxMsg.type    = CAN_FRAME_TYPE_DATA;

        can.CAN_TxMsg.data[0] = cmd[1];

        if (k < 8U)
        {
            for (l = 0; l < k; l++, i++)
            {
                can.CAN_TxMsg.data[l + 1U] = cmd[i + 2U];
            }
            can.CAN_TxMsg.data_length_code = (uint8_t) (k + 1U);
        }
        else
        {
            for (l = 0; l < 7U; l++, i++)
            {
                can.CAN_TxMsg.data[l + 1U] = cmd[i + 2U];
            }
            can.CAN_TxMsg.data_length_code = 8U;
        }

        /* MB0 can only be reused after the previous frame has completed. */
        uint32_t wait = 0U;
        while ((!g_can_tx_done) && (!can_TxBufferReady(0U)) && (wait < 100000U))
        {
            wait++;
        }

        if ((!g_can_tx_done) && (!can_TxBufferReady(0U)))
        {
            g_can_last_err = FSP_ERR_TIMEOUT;
            break;
        }

        g_can_tx_done = false;

        fsp_err_t err = g_canfd0.p_api->write(g_canfd0.p_ctrl, 0U, (can_frame_t *) &can.CAN_TxMsg);
        if (FSP_SUCCESS != err)
        {
            g_can_tx_done  = true;
            g_can_last_err = err;
            break;
        }
        g_can_last_err = err;

        wait = 0U;
        while ((!g_can_tx_done) && (!can_TxBufferReady(0U)) && (wait < 100000U))
        {
            wait++;
        }

        if ((!g_can_tx_done) && (!can_TxBufferReady(0U)))
        {
            g_can_last_err = FSP_ERR_TIMEOUT;
            break;
        }

        g_can_tx_done = true;

        ++packNum;
    }
}
