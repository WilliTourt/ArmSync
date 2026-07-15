#include "callbacks.h"
#include "adc.h"
#include "r_agt.h"
#include "r_icu.h"
#include "r_sci_uart.h"

extern "C" {

void ADC0_Callback(adc_callback_args_t *p_args) {
    FSP_PARAMETER_NOT_USED(p_args);
    AdcBase::onScanCplt();
    sliderGrip.update();
    sliderPitch.update();
}

void UART1_Callback(uart_callback_args_t *p_args) {
    switch (p_args->event) {
        case UART_EVENT_RX_COMPLETE:
            uart1_receive_complete_flag = true;
            break;
        case UART_EVENT_TX_COMPLETE:
            uart1_send_complete_flag = true;
            break;
        default: break;
    }
}

void UART2_Callback(uart_callback_args_t *p_args) {
    switch (p_args->event) {
        case UART_EVENT_RX_COMPLETE:
            uart2_receive_complete_flag = true;
            break;
        case UART_EVENT_TX_COMPLETE:
            uart2_send_complete_flag = true;
            break;
        default: break;
    }
}

void Timer_Main_Callback(timer_callback_args_t *p_args) {
    if (p_args->event == TIMER_EVENT_CYCLE_END) {
        AdcBase::startScan();
        main_timer_call = true;
    }
}

void Systick_Callback(timer_callback_args_t *p_args) {
    if (p_args->event == TIMER_EVENT_CYCLE_END) {
        sysTick++;
    }
}

void External_IRQ0_Callback(external_irq_callback_args_t *p_args) {
    if (p_args->channel == 0) {
        isKeyActive = true;
    }
}

} // extern "C"
