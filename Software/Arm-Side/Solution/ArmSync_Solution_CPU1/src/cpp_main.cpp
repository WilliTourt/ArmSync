#include "cpp_main.h"
#include "ipc.h"
#include "arm.h"

IPC ipc;

FSP_HEADER

void vSysTick(timer_callback_args_t *p_args) {
    (void)p_args;
    ipc.tick();
}

void UART1_Callback(uart_callback_args_t *p_args) {
    Motor::uartCallback(p_args);
}

// IPC ISR callback
void IPC0_Callback(ipc_callback_args_t *p_args) {
    if (p_args->event == IPC_EVENT_MESSAGE_RECEIVED) {
        if (p_args->message == static_cast<uint32_t>(MsgToken::MSG_CTRL_READY)) {
            IPC::onCtrlReady();
        }
    }
}

FSP_FOOTER



// Define the joint motors      addr, mindeg , maxdeg
Motor   J1_Upper_Swing          (6  , -90.0f , 90.0f );
Motor   J2_Upper_Abduction      (1  , -22.5f , 125.0f);
Motor   J3_Upper_Rot            (2  , -90.0f , 90.0f );
Motor   J4_Forearm_Swing        (3  , -122.0f, 0.0f  );
Motor   J5_Forearm_Rot          (4  , -90.0f , 90.0f );
Motor   J6_Wrist_Swing          (5  , -90.0f , 90.0f );

Arm arm(J1_Upper_Swing, J2_Upper_Abduction, J3_Upper_Rot,
        J4_Forearm_Swing, J5_Forearm_Rot,
        J6_Wrist_Swing);

void cpp_main() {

    R_SCI_B_UART_Open(&g_uart1_ctrl, &g_uart1_cfg);

    ipc.init();

    arm.init();

    while (1) {

    }
}
