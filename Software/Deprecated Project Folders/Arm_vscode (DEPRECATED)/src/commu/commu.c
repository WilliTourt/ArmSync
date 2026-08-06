#include "commu/commu.h"
#include "hal_data.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define UART0_RX_PACKET_SIZE    (29U)
#define UART0_RX_SOF            (0xAFU)
#define UART0_RX_CMD            (0x01U)
#define UART0_RX_EOF            (0xFAU)

uint32_t counter = 0U;

static volatile bool g_uart1_printf_tx_complete = true;
static volatile uint8_t g_uart0_rx_byte = 0U;
static volatile uint8_t g_uart0_rx_frame_buffer[UART0_RX_PACKET_SIZE];
static volatile uint32_t g_uart0_rx_frame_index = 0U;
static volatile bool g_uart0_rx_frame_active = false;
static volatile bool g_uart0_packet_ready = false;
static volatile uint32_t g_uart0_packet_error_count = 0U;
static volatile uint32_t g_uart0_packet_overwrite_count = 0U;
static beam_uart_packet_t g_uart0_latest_packet;
static uart_cfg_t g_uart1_runtime_cfg;

static void uart0_start_receive(void);
static void uart0_store_received_byte(uint8_t byte);
static void uart1_write_blocking(uint8_t const * p_src, uint32_t bytes);
static float uart0_bytes_to_float(uint8_t const * p_src);
static bool uart0_decode_packet(uint8_t const * p_packet, beam_uart_packet_t * p_decoded);
static void uart0_print_packet(beam_uart_packet_t const * p_packet);
static void uart0_reset_frame_state(void);

static void uart0_start_receive(void)
{
    fsp_err_t err = R_SCI_UART_Read(&g_uart0_ctrl, (uint8_t *) &g_uart0_rx_byte, 1U);
    if ((FSP_SUCCESS != err) && (FSP_ERR_IN_USE != err))
    {
        while (1)
        {
        }
    }
}

static void uart0_reset_frame_state(void)
{
    g_uart0_rx_frame_index = 0U;
    g_uart0_rx_frame_active = false;
}

static void uart0_store_received_byte(uint8_t byte)
{
    if (!g_uart0_rx_frame_active)
    {
        if (UART0_RX_SOF == byte)
        {
            g_uart0_rx_frame_buffer[0] = byte;
            g_uart0_rx_frame_index = 1U;
            g_uart0_rx_frame_active = true;
        }

        return;
    }

    if (g_uart0_rx_frame_index >= UART0_RX_PACKET_SIZE)
    {
        g_uart0_packet_error_count++;
        uart0_reset_frame_state();
        return;
    }

    g_uart0_rx_frame_buffer[g_uart0_rx_frame_index] = byte;
    g_uart0_rx_frame_index++;

    if (2U == g_uart0_rx_frame_index)
    {
        if (UART0_RX_CMD != g_uart0_rx_frame_buffer[1])
        {
            g_uart0_packet_error_count++;
            uart0_reset_frame_state();
        }

        return;
    }

    if (UART0_RX_PACKET_SIZE != g_uart0_rx_frame_index)
    {
        return;
    }

    {
        beam_uart_packet_t decoded_packet;

        if (uart0_decode_packet((uint8_t const *) g_uart0_rx_frame_buffer, &decoded_packet))
        {
            if (g_uart0_packet_ready)
            {
                g_uart0_packet_overwrite_count++;
            }

            g_uart0_latest_packet = decoded_packet;
            g_uart0_packet_ready = true;
        }
        else
        {
            g_uart0_packet_error_count++;
        }
    }

    uart0_reset_frame_state();
}

static void uart1_write_blocking(uint8_t const * p_src, uint32_t bytes)
{
    if ((NULL == p_src) || (0U == bytes))
    {
        return;
    }

    while (!g_uart1_printf_tx_complete)
    {
    }

    while (1)
    {
        g_uart1_printf_tx_complete = false;

        fsp_err_t err = R_SCI_UART_Write(&g_uart1_ctrl, p_src, bytes);
        if (FSP_SUCCESS == err)
        {
            break;
        }

        g_uart1_printf_tx_complete = true;
        if (FSP_ERR_IN_USE == err)
        {
            continue;
        }

        return;
    }

    while (!g_uart1_printf_tx_complete)
    {
    }
}

static float uart0_bytes_to_float(uint8_t const * p_src)
{
    uint32_t raw = 0U;
    float value = 0.0f;

    if (NULL == p_src)
    {
        return 0.0f;
    }

    raw = ((uint32_t) p_src[0] << 24) |
          ((uint32_t) p_src[1] << 16) |
          ((uint32_t) p_src[2] << 8)  |
          ((uint32_t) p_src[3]);

    memcpy(&value, &raw, sizeof(value));
    return value;
}

static bool uart0_decode_packet(uint8_t const * p_packet, beam_uart_packet_t * p_decoded)
{
    if ((NULL == p_packet) || (NULL == p_decoded))
    {
        return false;
    }

    if ((UART0_RX_SOF != p_packet[0]) ||
        (UART0_RX_CMD != p_packet[1]) ||
        (UART0_RX_EOF != p_packet[UART0_RX_PACKET_SIZE - 1U]))
    {
        return false;
    }

    p_decoded->upper.pitch = uart0_bytes_to_float(&p_packet[2]);
    p_decoded->upper.roll  = uart0_bytes_to_float(&p_packet[6]);
    p_decoded->upper.yaw   = uart0_bytes_to_float(&p_packet[10]);

    p_decoded->fore.pitch  = 0.0f;
    p_decoded->fore.roll   = uart0_bytes_to_float(&p_packet[14]);
    p_decoded->fore.yaw    = uart0_bytes_to_float(&p_packet[18]);

    p_decoded->hand.pitch  = 0.0f;
    p_decoded->hand.roll   = uart0_bytes_to_float(&p_packet[22]);
    p_decoded->hand.yaw    = 0.0f;

    p_decoded->claw_adc = (uint16_t) (((uint16_t) p_packet[26] << 8) | p_packet[27]);

    return true;
}

static void uart0_print_packet(beam_uart_packet_t const * p_packet)
{
    if (NULL == p_packet)
    {
        return;
    }

    printf("UART RX packet:\n");
    printf("  upper_pitch=%ld deg\n", (long) p_packet->upper.pitch);
    printf("  upper_roll=%ld deg\n", (long) p_packet->upper.roll);
    printf("  upper_yaw=%ld deg\n", (long) p_packet->upper.yaw);
    printf("  fore_roll=%ld deg\n", (long) p_packet->fore.roll);
    printf("  fore_yaw=%ld deg\n", (long) p_packet->fore.yaw);
    printf("  hand_roll=%ld deg\n", (long) p_packet->hand.roll);

    printf("  claw_adc=%u\n", p_packet->claw_adc);
}




void g_uart0_callback(uart_callback_args_t * p_args)
{
    if (NULL == p_args)
    {
        return;
    }

    if (UART_EVENT_RX_COMPLETE == p_args->event)
    {
        uart0_store_received_byte((uint8_t) g_uart0_rx_byte);
        uart0_start_receive();
    }
    else if (UART_EVENT_RX_CHAR == p_args->event)
    {
        uart0_store_received_byte((uint8_t) p_args->data);
    }
    else if ((UART_EVENT_ERR_OVERFLOW == p_args->event) ||
             (UART_EVENT_ERR_FRAMING == p_args->event) ||
             (UART_EVENT_ERR_PARITY == p_args->event))
    {
        g_uart0_packet_error_count++;
        uart0_reset_frame_state();
        uart0_start_receive();
    }
}

void g_uart1_callback(uart_callback_args_t * p_args)
{
    if ((NULL != p_args) && (UART_EVENT_TX_COMPLETE == p_args->event))
    {
        g_uart1_printf_tx_complete = true;
    }
}

void g_uart0_Init(void)
{
    fsp_err_t err = R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
    if (FSP_SUCCESS != err)
    {
        while (1)
        {
        }
    }

    uart0_reset_frame_state();
    g_uart0_packet_ready = false;
    g_uart0_packet_error_count = 0U;
    g_uart0_packet_overwrite_count = 0U;
    uart0_start_receive();
}

void g_uart1_Init(void)
{
    g_uart1_runtime_cfg = g_uart1_cfg;
    g_uart1_runtime_cfg.p_callback = g_uart1_callback;

    fsp_err_t err = R_SCI_UART_Open(&g_uart1_ctrl, &g_uart1_runtime_cfg);
    if (FSP_SUCCESS != err)
    {
        while (1)
        {
        }
    }

    g_uart1_printf_tx_complete = true;
}

int _write(int file, char * ptr, int len);
int _write(int file, char * ptr, int len)
{
    uint8_t out_buf[128];
    uint32_t out_len = 0U;

    (void) file;

    if ((NULL == ptr) || (len <= 0))
    {
        return 0;
    }

    if (0U == g_uart1_ctrl.open)
    {
        return len;
    }

    for (int i = 0; i < len; i++)
    {
        if ('\n' == ptr[i])
        {
            if ((out_len + 2U) > sizeof(out_buf))
            {
                uart1_write_blocking(out_buf, out_len);
                out_len = 0U;
            }

            out_buf[out_len++] = (uint8_t) '\r';
            out_buf[out_len++] = (uint8_t) '\n';
        }
        else
        {
            if ((out_len + 1U) > sizeof(out_buf))
            {
                uart1_write_blocking(out_buf, out_len);
                out_len = 0U;
            }

            out_buf[out_len++] = (uint8_t) ptr[i];
        }
    }

    if (out_len > 0U)
    {
        uart1_write_blocking(out_buf, out_len);
    }

    return len;
}

void uart0_process_received_packet(void)
{
    beam_uart_packet_t packet_to_print;
    uint32_t packet_error_count = 0U;
    uint32_t packet_overwrite_count = 0U;
    bool has_packet = false;

    __disable_irq();
    if (g_uart0_packet_ready)
    {
        packet_to_print = g_uart0_latest_packet;
        g_uart0_packet_ready = false;
        has_packet = true;
    }

    if (g_uart0_packet_error_count > 0U)
    {
        packet_error_count = g_uart0_packet_error_count;
        g_uart0_packet_error_count = 0U;
    }

    if (g_uart0_packet_overwrite_count > 0U)
    {
        packet_overwrite_count = g_uart0_packet_overwrite_count;
        g_uart0_packet_overwrite_count = 0U;
    }
    __enable_irq();

    if (has_packet)
    {
        uart0_print_packet(&packet_to_print);
    }

    if (packet_error_count > 0U)
    {
        printf("UART RX packet parse error count: %lu\n", packet_error_count);
    }

    if (packet_overwrite_count > 0U)
    {
        printf("UART RX packet overwrite count: %lu\n", packet_overwrite_count);
    }
}

bool uart0_get_latest_packet(beam_uart_packet_t * p_packet)
{
    bool has_packet = false;

    if (NULL == p_packet)
    {
        return false;
    }

    __disable_irq();
    if (g_uart0_packet_ready)
    {
        *p_packet = g_uart0_latest_packet;
        g_uart0_packet_ready = false;
        has_packet = true;
    }
    __enable_irq();

    return has_packet;
}
