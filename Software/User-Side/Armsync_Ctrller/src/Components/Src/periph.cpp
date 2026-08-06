#include "periph.h"

void Periph::init() {
    fsp_err_t err = FSP_SUCCESS;

    err = R_SCI_UART_Open(g_uart1.p_ctrl, g_uart1.p_cfg);
    assert(err == FSP_SUCCESS);

    err = R_SCI_UART_Open(g_uart2.p_ctrl, g_uart2.p_cfg);
    assert(err == FSP_SUCCESS);

    err = R_ICU_ExternalIrqOpen(g_external_irq0.p_ctrl, g_external_irq0.p_cfg);
    assert(err == FSP_SUCCESS);
    err = R_ICU_ExternalIrqEnable(g_external_irq0.p_ctrl);
    assert(err == FSP_SUCCESS);

    R_IOPORT_PinWrite(&g_ioport_ctrl, LED1, BSP_IO_LEVEL_HIGH);
    R_IOPORT_PinWrite(&g_ioport_ctrl, LED2, BSP_IO_LEVEL_HIGH); // LOW = led on

    initTimers();
}

void Periph::initTimers() {
    fsp_err_t err = FSP_SUCCESS;

    err = R_AGT_Open(g_timer_main.p_ctrl, g_timer_main.p_cfg);
    assert(err == FSP_SUCCESS);
    err = R_AGT_Start(g_timer_main.p_ctrl);
    assert(err == FSP_SUCCESS);
    err = R_AGT_Enable(g_timer_main.p_ctrl);
    assert(err == FSP_SUCCESS);

    err = R_AGT_Open(g_systick.p_ctrl, g_systick.p_cfg);
    assert(err == FSP_SUCCESS);
    err = R_AGT_Start(g_systick.p_ctrl);
    assert(err == FSP_SUCCESS);
    err = R_AGT_Enable(g_systick.p_ctrl);
    assert(err == FSP_SUCCESS);

    R_GPT_Open(&g_timer_buzzer_ctrl, &g_timer_buzzer_cfg);
    R_GPT_Start(&g_timer_buzzer_ctrl);
}

void Periph::setBuzzerDutyCycle(uint8_t dutyCycle) {
    timer_info_t info;
    uint32_t current_period_counts;
    uint32_t duty_cycle_counts;

    if (dutyCycle > 100) dutyCycle = 100;

    R_GPT_InfoGet(&g_timer_buzzer_ctrl, &info);

    current_period_counts = info.period_counts;
    duty_cycle_counts = (uint32_t)(((uint64_t) current_period_counts * dutyCycle) / 100);

    R_GPT_DutyCycleSet(&g_timer_buzzer_ctrl, duty_cycle_counts, GPT_IO_PIN_GTIOCB);
}

void Periph::setBuzzerFreq(uint32_t freq) {
    timer_info_t info;
    R_GPT_InfoGet(&g_timer_buzzer_ctrl, &info);
    uint32_t period = info.clock_frequency / freq;
    R_GPT_PeriodSet(&g_timer_buzzer_ctrl, period);
}