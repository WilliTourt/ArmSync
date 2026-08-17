#include "UITask.h"
#include "ElegantDebug.h"
#include "hal_data.h"
#include "CPUCommTask/CPUCommTask.h"
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cmath>

extern ElegantDebug dbg;


uint8_t UITask::_rxBuf[128];
RingBuf UITask::_rxRing(UITask::_rxBuf, sizeof(UITask::_rxBuf));
volatile bool UITask::_txDone = true;

void UITask::uart3Callback(uart_callback_args_t *p_args) {
    if (p_args->event == UART_EVENT_RX_CHAR) {
        _rxRing.put((uint8_t)p_args->data);
    }
    if (p_args->event == UART_EVENT_TX_COMPLETE) {
        _txDone = true;
    }
}



static char _tjcBuf[128];

void UITask::_send(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(_tjcBuf, sizeof(_tjcBuf) - 3, fmt, args);
    va_end(args);
    if (len < 0) return;

    _tjcBuf[len]     = '\xFF';
    _tjcBuf[len + 1] = '\xFF';
    _tjcBuf[len + 2] = '\xFF';
    len += 3;

    // Wait for previous TX to complete
    while (!_txDone) {
        if (FreeRTOS::Kernel::getSchedulerState() != FreeRTOS::Kernel::SchedulerState::Running) {
            R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);        
        } else {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
    _txDone = false;
    R_SCI_B_UART_Write(&g_uart3_ctrl, (uint8_t*)_tjcBuf, (uint32_t)len);
}

void UITask::_parseScreenInput() {
    // Single-char protocol from TJC buttons: E=ESTOP, B=BTZ, R=REC, P=PLAY.
    // Screen sends exactly one ASCII char per press (no frame tail).
    int ch;
    while ((ch = _rxRing.get()) >= 0) {
        // Map button presses following the mutual-exclusion rules:
        //  - while PLAY: REC/HOME ignored (ESTOP valid)
        //  - while REC : PLAY ignored (ESTOP and BTZ/HOME both valid)
        switch (ch) {
            case 'E':  // ESTOP : valid in any state, only released by BTZ
                CPUCommTask::setEstop(true);
                updateStatusText(StatusText::STOP);   // "STOP"
                dbg.logWithType("TJC BTN", COLOR_RED, "ESTOP!\n");
                break;
            case 'B':  // BTZ (home/zero) : valid except while playing (REC ok)
                if (_isPlaying) break;   // ignore while playing
                CPUCommTask::setBtz(true);   // one-shot home pulse
                if (CPUCommTask::getEstop()) {
                    CPUCommTask::setEstop(false);   // BTZ also releases estop
                    updateStatusText(_alarmActive ? StatusText::STOP
                                     : (_isRecording ? StatusText::RECORD
                                                     : StatusText::MANUAL));
                    dbg.logWithType("TJC BTN", COLOR_YELLOW, "BTZ, ESTOP RELEASED\n");
                } else {
                    dbg.logWithType("TJC BTN", COLOR_YELLOW, "BTZ\n");
                }
                break;
            case 'R':  // REC : start/stop recording
                if (_isPlaying) break;   // ignore while playing
                _isRecording = !_isRecording;
                updateStatusText(_isRecording ? StatusText::RECORD
                                              : StatusText::MANUAL);
                _notifyFusion();   // REC start -> Fusion records; end -> Fusion saves + tells RecPlay
                break;
            case 'P':  // PLAY : start/stop playback
                if (_isRecording) break;   // ignore while recording
                _isPlaying = !_isPlaying;
                updateStatusText(_isPlaying ? StatusText::PLAYING
                                            : StatusText::MANUAL);
                if (_isPlaying) {
                    RecPlayTask::setLoop(_loopMode);   // LOOP vs SINGLE
                    _suspendUpstream();   // pause IK/Normalize/UartRecv/Fusion
                    _notifyRecPlay(RecPlayTask::RecCmd::PLAY_START);
                } else {
                    _notifyRecPlay(RecPlayTask::RecCmd::PLAY_END);
                    _resumeUpstream();
                }
                break;
            case 'L':  // Radio selected -> LOOP playback mode
                _loopMode = true;
                RecPlayTask::setLoop(true);
                dbg.logWithType("TJC MODE", COLOR_CYAN, "LOOP\n");
                break;
            case 'S':  // Radio cleared -> SINGLE playback mode
                _loopMode = false;
                RecPlayTask::setLoop(false);
                dbg.logWithType("TJC MODE", COLOR_CYAN, "SINGLE\n");
                break;
            default: break;
        }
    }
}



void UITask::_updateJointAngle(int idx, float angle_deg) {
    // J1Angle ~ J6Angle, virtual float vvs1=2
    int val = (int)(angle_deg * 100.0f);
    _send("J%dAngle.val=%d", idx + 1, val);
    vTaskDelay(pdMS_TO_TICKS(3));
}

void UITask::_updateJointStatus(int idx, bool ok) {
    // J1Status ~ J6Status: text "●", color green/red
    _send("J%dStatus.pco=%d", idx + 1, (ok ? tjcCOLOR_GREEN : tjcCOLOR_RED));
    vTaskDelay(pdMS_TO_TICKS(3));
}

void UITask::_updateGrip(float percent, bool stuck) {
    // GRIPPercent: 0=69mm open, 100=0mm closed
    _send("GRIPPercent.val=%d", (int)percent);
    vTaskDelay(pdMS_TO_TICKS(3));
    // static glyph on screen; only color changes.
    _send("GRIPStatus.pco=%d", (stuck ? tjcCOLOR_RED : tjcCOLOR_GREEN));
    vTaskDelay(pdMS_TO_TICKS(3));
}


void UITask::setTaskHandles(TaskHandle_t fusion, TaskHandle_t recplay,
                            TaskHandle_t uartRecv, TaskHandle_t normalize,
                            TaskHandle_t ik) {
    _fusionHandle  = fusion;
    _recPlayHandle = recplay;
    _suspendHandles[0] = uartRecv;
    _suspendHandles[1] = normalize;
    _suspendHandles[2] = ik;
    _suspendHandles[3] = fusion;
}

void UITask::_notifyFusion() {
    if (_fusionHandle == nullptr) return;
    sharedDatatype::UICommand cmd = sharedDatatype::UICommand::NONE;
    if (_isRecording) cmd = sharedDatatype::UICommand::REC;
    xTaskNotifyIndexed(_fusionHandle, 0, static_cast<uint32_t>(cmd), eSetValueWithOverwrite);
    dbg.logWithType("UI->FUSION", COLOR_CYAN, "%s\n", (_isRecording ? "REC" : "NONE"));
}

void UITask::_notifyRecPlay(RecPlayTask::RecCmd cmd) {
    if (_recPlayHandle == nullptr) return;
    xTaskNotifyIndexed(_recPlayHandle, 0, static_cast<uint32_t>(cmd), eSetValueWithOverwrite);
}

void UITask::_suspendUpstream() {
    for (auto h : _suspendHandles) {
        if (h != nullptr) vTaskSuspend(h);
    }
}

void UITask::_resumeUpstream() {
    for (auto h : _suspendHandles) {
        if (h != nullptr) vTaskResume(h);
    }
}


void UITask::updateStatusText(StatusText text) {
    const char* txt = "STOP   ";
    switch (text) {
        case StatusText::MANUAL : txt = "MANUAL "; break;
        case StatusText::RECORD : txt = "RECORD."; break;
        case StatusText::PLAYING: txt = "PLAYING"; break;
        case StatusText::STOP   : txt = "STOP   "; break;
    }

    _send("StatusText.txt=\"%s\"", txt);
    vTaskDelay(pdMS_TO_TICKS(3));
    _send("Status.pco=%d", ((text == StatusText::STOP) ? tjcCOLOR_RED : tjcCOLOR_GREEN));
    vTaskDelay(pdMS_TO_TICKS(3));
}

void UITask::updateFreq(int hz) {
    _send("Freq.txt=\"%dHz\"", hz);
    vTaskDelay(pdMS_TO_TICKS(3));
}

// HMS_Msg0 ~ HMS_Msg4: circular slot, newest message takes the current slot
// and the pointer advances 0->1->...->4->0. Static so any task can report.
static uint8_t _hmsSlot = 0;
void UITask::updateHMS(const char* msg) {
    _send("HMS_Msg%d.txt=\"%s\"", _hmsSlot, msg);
    vTaskDelay(pdMS_TO_TICKS(3));
    _hmsSlot++;
    if (_hmsSlot > 4) _hmsSlot = 0;
}


// NPUStatus dot color + NPUText (txt is always 7 chars; spaces pad for alignment).
void UITask::updateNPUStatus(NpuState state) {
    const char* txt = "OFF    ";
    uint16_t    color = tjcCOLOR_GRAY;
    switch (state) {
        case NpuState::OFF:     txt = "OFF    "; color = tjcCOLOR_GRAY;  break;
        case NpuState::IDLE:    txt = "IDLE   "; color = tjcCOLOR_ORANGE; break;
        case NpuState::RUNNING: txt = "RUNNING"; color = tjcCOLOR_GREEN; break;
    }

    _send("NPUText.txt=\"%s\"", txt);
    vTaskDelay(pdMS_TO_TICKS(3));
    _send("NPUStatus.pco=%d", color);
    vTaskDelay(pdMS_TO_TICKS(3));
    _send("NPUText.pco=%d", color);
    vTaskDelay(pdMS_TO_TICKS(3));
}

void UITask::booting(BootingPhase phase) {
    _send("ProgressBar.val=%d", static_cast<int>(phase));

    if (phase == BootingPhase::DONE) {
        _send("page ArmSync_Main");
    }
}

void UITask::taskFunction() {
    dbg.ok("UITask started.\n");

    for (;;) {
        // Receive PLAY_DONE from RecPlayTask: take exhausted naturally.
        // Respawn upstream tasks and clear the playback state so the UI returns
        // to manual control (see mutual-exclusion note).
        uint32_t uiNotif = 0;
        if (xTaskNotifyWaitIndexed(0, 0, 0xFFFFFFFF, &uiNotif, 0) == pdTRUE) {
            if (static_cast<RecPlayTask::RecCmd>(uiNotif) == RecPlayTask::RecCmd::PLAY_DONE) {
                if (_isPlaying) {
                    _isPlaying = false;
                    _resumeUpstream();
                    _notifyFusion();
                    updateStatusText(StatusText::MANUAL);
                    dbg.logWithType("UI", COLOR_YELLOW, "playback done -> auto resume\n");
                }
            }
        }

        // Receive live control frequency from CPUCommTask (index 1).
        uint32_t freqNotif = 0;
        if (xTaskNotifyWaitIndexed(1, 0, 0xFFFFFFFF, &freqNotif, 0) == pdTRUE) {
            updateFreq((int)freqNotif);
        }

        _parseScreenInput();

        auto feedback = _fdbk.receive(0);
        if (feedback) {

            // Divided by motor reduction ratio
            feedback->jointAngle[0] /= 20.0f;
            feedback->jointAngle[1] /= 30.0f;
            feedback->jointAngle[2] /= 10.0f;
            feedback->jointAngle[3] /= 10.0f;
            feedback->jointAngle[5] /= 10.0f;

            for (int i = 0; i < 6; i++) {
                _updateJointAngle(i, feedback->jointAngle[i]);
            }

            // ---- Over-limit / stall alarm (rising edge, fire once) ----
            bool anyAlarm = false;
            const uint32_t mask = 0u;
            for (int i = 0; i < 6; i++) {
                const float deg = feedback->jointAngle[i];
                bool overLimit = (deg <= JOINT_LIMIT_MIN[i]) || (deg >= JOINT_LIMIT_MAX[i]);
                bool stalled   = feedback->isLockedRotor[i];
                bool alarm     = overLimit || stalled;
                if (alarm) anyAlarm = true;

                if (alarm && !(_alarmPrev & (1u << i))) {
                    const char* why = stalled ? "STALL" : "LIMIT";
                    char msg[32];
                    snprintf(msg, sizeof(msg), "J%d %s", i + 1, why);
                    UITask::updateHMS(msg);
                    dbg.logWithType("ALARM", COLOR_RED, "J%d %s (deg=%.1f)\n",
                                    i + 1, why, deg);
                }
                if (alarm) _alarmPrev |= (1u << i);
                else       _alarmPrev &= ~(1u << i);
            }
            if (anyAlarm && !_alarmActive) {
                _alarmActive = true;
                updateStatusText(StatusText::STOP);   // "STOP"
            } else if (!anyAlarm && _alarmActive) {
                _alarmActive = false;
                // Don't override a latched ESTOP: only return to REC/PLAY/MANUAL
                // once the emergency stop is actually released (BTZ) and no alarm.
                if (!CPUCommTask::getEstop()) {
                    updateStatusText(_isRecording ? StatusText::RECORD
                                                  : (_isPlaying ? StatusText::PLAYING
                                                                : StatusText::MANUAL));
                }
            }

            for (int i = 0; i < 6; i++) {
                _updateJointStatus(i, !feedback->isLockedRotor[i]);
            }
        }

        auto ee = _eeUIQueue.receive(0);
        if (ee) {
            _updateGrip(ee->grip_percent, false);
        }

        this->delay(pdMS_TO_TICKS(40));
    }
}   
