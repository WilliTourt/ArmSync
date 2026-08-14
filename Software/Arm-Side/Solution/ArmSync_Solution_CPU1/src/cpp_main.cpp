#include "cpp_main.h"
#include "ipc.h"
#include "gripper.h"
#include "ElegantDebug.h"
#include "app/drivers/Emm_V5.h"

IPC ipc;
Gripper gripper(0);
ElegantDebug dbg(&g_uart8, true, true);

// Motor UART addresses (matches CPU0 joint order J1~J6)
static const uint8_t kJointAddr[6] = {6, 1, 2, 3, 4, 5};

// Feedback poll state (position/status alternating, one motor per poll)
static uint8_t pollIndex = 0;
static bool    pollStatus = false;

#define IPC_MS 40 
#define MOTOR_FEEDBACK_MS 5
#define GRIPPER_FEEDBACK_MS 25

#define WATCHDOG_MS 500    // arm.stop() if no ctrl from CPU0 within this


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

// Motor UART1 callback
void UART1_Callback(uart_callback_args_t *p_args) {
    Emm_V5_UartCallback(p_args);
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

// Query the next motor's feedback (position S_CPOS / status S_FLAG alternating)
static void pollNextFeedback() {
    Emm_V5_Read_Sys_Params(kJointAddr[pollIndex], pollStatus ? S_FLAG : S_CPOS);

    pollIndex++;
    if (pollIndex >= 6) {
        pollIndex = 0;
        pollStatus = !pollStatus;
    }
}

// Emergency stop all 6 motors (called on watchdog trip)
static void armEmergencyStop() {
    for (int i = 0; i < 6; i++) {
        Emm_V5_Stop_Now(kJointAddr[i], true);
    }
    Emm_V5_Synchronous_motion(0);
}

void cpp_main() {

    R_SCI_B_UART_Open(&g_uart1_ctrl, &g_uart1_cfg);
    R_SCI_B_UART_Open(&g_uart5_ctrl, &g_uart5_cfg);
    R_SCI_B_UART_Open(&g_uart8_ctrl, &g_uart8_cfg);

    R_AGT_Open(&agt_SysTick_ctrl, &agt_SysTick_cfg);
    R_AGT_Start(&agt_SysTick_ctrl);

    ipc.init();

    Emm_V5_Init();
    for (int i = 0; i < 6; i++) {
        Emm_V5_Modify_Ctrl_Mode(kJointAddr[i], true, 2);   // closed-loop
        Emm_V5_En_Control(kJointAddr[i], true, false);     // enable
    }
    gripper.init();

    dbg.info("M33 main loop started.\n");

    uint32_t lastIPCMs = 0;
    uint32_t lastMotorFbMs = 0;
    uint32_t lastGripFbMs = 0;
    bool dogBark = false;

    while (1) {
        // Watchdog: if CPU0 stops sending ctrl, emergency-stop the arm
        if (ipc.isWatchDogHungry(WATCHDOG_MS)) {
            if (!dogBark) {
                dogBark = true;
                armEmergencyStop();
                dbg.logWithType("M33", COLOR_RED, "WATCHDOG: no ctrl from CPU0 for %dms, EMERGENCY STOP!\n", WATCHDOG_MS);
            }
        } else {
            dogBark = false;   // link healthy again
        }

        // Receive motion plan from CPU0, drive motors directly
        MotionPlanPacket plan;
        float gripPercent;
        if (!dogBark && ipc.getCtrlPacket(plan, gripPercent)) {
            for (int i = 0; i < 6; i++) {
                Emm_V5_Pos_Control(kJointAddr[i],
                                   plan.motors[i].dir,
                                   plan.motors[i].rpm,
                                   plan.motors[i].acc,
                                   plan.motors[i].pulse,
                                   true,   // absolute position
                                   true);  // cache, trigger together
            }
            Emm_V5_Synchronous_motion(0);   // move all together

            gripper.setRatio(gripPercent);

            dbg.logWithType("M33", COLOR_GREEN,
                "RX: grip=%.0f%% | m0(dir%d r%d a%d p%lu)\n",
                gripPercent,
                plan.motors[0].dir, plan.motors[0].rpm, plan.motors[0].acc,
                plan.motors[0].pulse);
        }

        // Send feedback to CPU0
        uint32_t now = ipc.getTick();
        if (now - lastIPCMs >= IPC_MS) {
            lastIPCMs = now;

            float angles_deg[6] = {0};
            bool locked[6] = {false};
            for (int i = 0; i < 6; i++) {
                float motorDeg = 0.0f;
                if (Emm_V5_GetPositionDegrees(kJointAddr[i], &motorDeg)) {
                    angles_deg[i] = motorDeg;
                }
                Emm_V5_Feedback_t fb;
                if (Emm_V5_GetFeedback(kJointAddr[i], &fb) && fb.status_valid) {
                    if (fb.status_flags & (EMM_V5_STATUS_STALLED | EMM_V5_STATUS_STALL_PROTECTION)) {
                        locked[i] = true;
                    }
                }
            }
            bool stuck = gripper.isStuck();
            float gripAngle = gripper.getAngle();

            ipc.sendFeedback(angles_deg, gripAngle, locked, stuck);
        }

        // Poll next motor's feedback
        if (now - lastMotorFbMs >= MOTOR_FEEDBACK_MS) {
            lastMotorFbMs = now;
            pollNextFeedback();
        }

        // Update gripper feedback
        if (now - lastGripFbMs >= GRIPPER_FEEDBACK_MS) {
            lastGripFbMs = now;
            gripper.updateFeedback();
        }

        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
    }
}
