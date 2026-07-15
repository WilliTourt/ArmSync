#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__

#include "hal_data.h"
#include "adc.h"

extern "C" {

void ADC0_Callback(adc_callback_args_t *p_args);
void UART1_Callback(uart_callback_args_t *p_args);
void UART2_Callback(uart_callback_args_t *p_args);
void Timer_Main_Callback(timer_callback_args_t *p_args);
void Systick_Callback(timer_callback_args_t *p_args);
void External_IRQ0_Callback(external_irq_callback_args_t *p_args);

}

extern AdcSlider sliderGrip;
extern AdcSlider sliderPitch;
extern volatile uint32_t sysTick;
extern volatile bool main_timer_call;
extern volatile bool uart1_receive_complete_flag;
extern volatile bool uart2_receive_complete_flag;
extern volatile bool uart1_send_complete_flag;
extern volatile bool uart2_send_complete_flag;
extern volatile bool uart2_send_pending;
extern volatile bool isKeyActive;
extern volatile bool is_calibrated;

#endif
