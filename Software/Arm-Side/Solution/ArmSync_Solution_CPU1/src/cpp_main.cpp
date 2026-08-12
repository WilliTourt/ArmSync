#include "cpp_main.h"
#include "ipc.h"
#include "arm.h"
#include "gripper.h"
#include "ElegantDebug.h"

IPC ipc;
Gripper gripper(0);
ElegantDebug dbg(&g_uart8, true, true);

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

FSP_HEADER

void vSysTick(timer_callback_args_t *p_args) {
    (void)p_args;
    ipc.tick();
    ElegantDebug::tick();
}

// IPC ISR callback
void IPC0_Callback(ipc_callback_args_t *p_args) {
    if (p_args->event == IPC_EVENT_MESSAGE_RECEIVED) {
        if (p_args->message == static_cast<uint32_t>(MsgToken::MSG_CTRL_READY)) {
            IPC::onCtrlReady();
        }
    }
}

void UART1_Callback(uart_callback_args_t *p_args) {
    Motor::uartCallback(p_args);
}

// Gripper SCI5 callback
void UART5_Callback(uart_callback_args_t *p_args) {
    Gripper::uartCallback(p_args);
}

// Debug SCI8 callback
void UART8_Callback(uart_callback_args_t *p_args) {
    ElegantDebug::onTxComplete(p_args);
}

FSP_FOOTER



void cpp_main() {

    R_SCI_B_UART_Open(&g_uart1_ctrl, &g_uart1_cfg);
    R_SCI_B_UART_Open(&g_uart5_ctrl, &g_uart5_cfg);
    R_SCI_B_UART_Open(&g_uart8_ctrl, &g_uart8_cfg);

    ipc.init();

    arm.init();
    gripper.init();

    dbg.info("M33 main loop started.\n");

    uint32_t lastFeedbackMs = 0;
    uint32_t lastGripMs = 0;

    while (1) {
        // ---- 1. Receive control from CPU0 ----
        ArmTarget armTarget;
        GripTarget grip;
        if (ipc.getCtrlPacket(armTarget, grip)) {
            arm.setAngles(armTarget.jointAngle);
            gripper.setRatio(grip.ratio);

            dbg.logWithType("M33", COLOR_GREEN,
                "RX: J1=%.1f J2=%.1f J3=%.1f J4=%.1f J5=%.1f J6=%.1f | grip=%.0f%%\n",
                armTarget.jointAngle[0], armTarget.jointAngle[1], armTarget.jointAngle[2],
                armTarget.jointAngle[3], armTarget.jointAngle[4], armTarget.jointAngle[5],
                grip.ratio);
        }

        // ---- 2. Send feedback to CPU0 (every ~50 ms) ----
        uint32_t now = ipc.getTick();
        if (now - lastFeedbackMs >= 50) {
            lastFeedbackMs = now;

            float angles_deg[6];
            float currents_ma[6];
            arm.getFeedback(angles_deg, currents_ma);

            bool locked = false;   // TODO: read from motor status
            bool stuck  = gripper.isStuck();

            ipc.sendFeedback(angles_deg, locked, stuck);
        }

        // ---- 3. Poll gripper angle (every ~200 ms) ----
        if (now - lastGripMs >= 200) {
            lastGripMs = now;
            gripper.getAngle();
        }

        R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    }
}
