#include "FusionTask.h"
#include "RecPlayTask/RecPlayTask.h"
#include "UITask.h"
#include "ElegantDebug.h"
#include <cmath>
#include <cstring>

extern "C" {
#include "../src/app/npu_model/model.h"
}

#include "pmu_ethosu.h"

extern ElegantDebug dbg;

float FusionTask::_mapPitchToJ6(float pitch_percent) const {
    if (pitch_percent < 0.0f) pitch_percent = 0.0f;
    if (pitch_percent > 100.0f) pitch_percent = 100.0f;

    return J6_MIN_DEG + (pitch_percent / 100.0f) * (J6_MAX_DEG - J6_MIN_DEG);
}

void FusionTask::_applyLowPass(sharedDatatype::JointAngleData &out) {
    for (int i = 0; i < 6; i++) {
        float const raw = out.angles[i];
        if (!_filterInit) {
            _filtered[i] = raw;          // seed with the first frame
            continue;
        }
        _filtered[i] = LP_FILTER_ALPHA * raw
                     + (1.0f - LP_FILTER_ALPHA) * _filtered[i];
        out.angles[i] = _filtered[i];
    }
    _filterInit = true;
}

// Open the Ethos-U peripheral once (before the scheduler loop). Mirrors the
// old NPUTask init: RM_ETHOSU_Open + PMU cycle/event counters.
fsp_err_t FusionTask::_initNPU() {
    fsp_err_t status = RM_ETHOSU_Open(&g_rm_ethosu0_ctrl, &g_rm_ethosu0_cfg);
    if (status != FSP_SUCCESS) {
        return status;
    }

    ETHOSU_PMU_Enable(&g_ethosu0);
    ETHOSU_PMU_CNTR_Enable(&g_ethosu0, ETHOSU_PMU_CCNT_Msk);
    ethosu_pmu_event_type events[] = {
        ETHOSU_PMU_NPU_IDLE,
        ETHOSU_PMU_NPU_ACTIVE,
        ETHOSU_PMU_AXI0_ENABLED_CYCLES,
        ETHOSU_PMU_AXI1_ENABLED_CYCLES
    };
    for (uint32_t i = 0; i < sizeof(events) / sizeof(events[0]); i += 1) {
        ETHOSU_PMU_Set_EVTYPER(&g_ethosu0, i, events[i]);
        ETHOSU_PMU_CNTR_Enable(&g_ethosu0, 1u << i);
    }
    ETHOSU_PMU_CYCCNT_Reset(&g_ethosu0);
    ETHOSU_PMU_EVCNTR_ALL_Reset(&g_ethosu0);

    return status;
}

// Run the NPU filter on the fused J1..J5: push the current J1..J5 into the
// 9-frame sliding window, invoke the model, and add the returned deltas to
// J1..J5 (in place). Returns true once the window is full and inference ran.
bool FusionTask::_applyNpuFilter(sharedDatatype::JointAngleData &out) {
    if (!_npuReady) return false;

    // Shift the window left by one frame (drop oldest), then append the
    // current J1..J5 as the newest frame.
    const int jointLen = NPU_WIN_JOINTS;
    const int winLen   = NPU_WIN_FRAMES * jointLen;
    if (_npuFrames >= NPU_WIN_FRAMES) {
        std::memmove(_npuWin, _npuWin + jointLen,
                     (NPU_WIN_FRAMES - 1) * jointLen * sizeof(float));
    }
    int base = (_npuFrames < NPU_WIN_FRAMES)
               ? _npuFrames * jointLen
               : (NPU_WIN_FRAMES - 1) * jointLen;
    for (int j = 0; j < jointLen; j++) {
        _npuWin[base + j] = out.angles[j];   // J1..J5 (angles[0..4])
    }
    if (_npuFrames < NPU_WIN_FRAMES) {
        _npuFrames++;
        return false;   // not enough history yet
    }

    // Feed the 45-float window, run, then apply the 5 deltas to J1..J5.
    std::memcpy(GetModelInputPtr_window_deg(), _npuWin, winLen * sizeof(float));
    RunModel(false);   // new model's RunModel returns void
    float const * delta = GetModelOutputPtr_delta_deg_70003();
    for (int j = 0; j < 5; j++) {
        out.angles[j] += delta[j];
    }

    // Inference frequency -> UI (sliding window of ticks), like the old NPUTask.
    uint32_t now = static_cast<uint32_t>(xTaskGetTickCount());
    _npuTicks[_npuTickIdx] = now;
    _npuTickIdx = (_npuTickIdx + 1) % NPU_FREQ_WINDOW;
    if (_npuTickIdx == 0) _npuTickFull = true;
    if (_npuTickFull) {
        uint32_t oldest = _npuTicks[_npuTickIdx];   // index now points at oldest
        uint32_t span   = now - oldest;
        if (span > 0) {
            uint32_t avgPer = span / (NPU_FREQ_WINDOW - 1);
            if (avgPer > 0) {
                UITask::updateNPUFreq(static_cast<int>(1000u / avgPer));
            }
        }
    }
    return true;
}

void FusionTask::setUIHandle(TaskHandle_t handle) {
    _uiHandle = handle;
}

void FusionTask::setRecHandle(TaskHandle_t handle) {
    _recHandle = handle;
}

void FusionTask::taskFunction() {
    dbg.ok("FusionTask started.\n");

    // Open the Ethos-U once (NPU filter model) before the control loop.
    _npuReady = (_initNPU() == FSP_SUCCESS);
    if (_npuReady) {
        dbg.ok("Fusion NPU filter ready.\n");
        UITask::updateNPUStatus(UITask::NpuState::RUNNING);
    } else {
        dbg.error("Fusion NPU init failed; NPU filter disabled.\n");
        UITask::updateNPUStatus(UITask::NpuState::OFF);
    }

    float handJ3       = 0.0f; // forearm / upperarm roll (deg), from hand quaternion
    float handJ5       = 0.0f; // forearm roll (deg), from hand quaternion
    float latestPitch  = 50.0f; // default: J6 center

    for (;;) {

        // Always drain hand data, non-blocking. The last valid values are
        // kept (sticky) so a missing hand frame never snaps J3/J5/J6 to 0.
        auto hand = _handQueue.receive(0);
        if (hand) {
            handJ3       = hand->j3deg;
            handJ5       = hand->j5deg;
            latestPitch  = hand->pitch_percent;
        }

        // Block on IK data (main fusion trigger)
        auto ik = _ikQueue.receive(portMAX_DELAY);

        sharedDatatype::JointAngleData out = {};
        out.timestamp = ik->timestamp;

        // J1/J2/J4: IK only
        out.angles[0] = ik->angles[0];
        out.angles[1] = ik->angles[1];
        out.angles[3] = ik->angles[3];

        // J3: IK blended with the (sticky) hand roll. No separate NPU J3 source
        // anymore; the new NPU filter model smooths J1..J5 below instead.
        out.angles[2] = J3_IK_ALPHA * ik->angles[2] + J3_HAND_ALPHA * handJ3;

        // J5: hand roll only (sticky last valid; IK can't sense roll)
        out.angles[4] = -handJ5;

        // J6: hand pitch only (sticky last valid)
        out.angles[5] = _mapPitchToJ6(latestPitch);
        // Filter 1: NPU filter on J1..J5 (adds the model deltas).
        // _applyNpuFilter(out);

        // Filter 2: one-pole low-pass on all six
        _applyLowPass(out);
        
        // Poll the latest UI command (index 0, non-blocking, overwrite).
        // UITask pushes UICommand::REC to start, UICommand::NONE to stop.
        // NOTE (lock latch): UITask's REC button sends a ONE-SHOT notification
        // (REC or NONE), consumed once. If we reassign _recording from it every
        // frame, the notification is gone on the next frame -> _recording flips
        // back to false -> an immediate spurious "recording done" + saveToFlash.
        // So we only update _recording when a notification actually arrives
        // (pdTRUE); otherwise we KEEP the current recording state (latched).
        uint32_t uiVal = 0;
        bool haveUiCmd = false;
        if (_uiHandle != nullptr) {
            haveUiCmd = (xTaskNotifyWaitIndexed(0, 0, 0xFFFFFFFF, &uiVal, 0) == pdTRUE);
        }
        const auto uiCmd = static_cast<sharedDatatype::UICommand>(uiVal);

        // Detect state transition for REC start / REC end.
        const bool wasRecording = _recording;
        if (haveUiCmd) {
            _recording = (uiCmd == sharedDatatype::UICommand::REC);
        }
        // else: keep _recording unchanged (latched) until the next real command.

        // Record tap: mirror this fused frame to RecPlayTask while recording.
        // On the REC->END transition we still push this (the last) frame, then
        // tell RecPlayTask the take is complete so it saves to flash.
        if (_recording || wasRecording) {
            _recQueue.sendToBack(out, 0);
        }
        if (wasRecording && !_recording) {
            if (_recHandle != nullptr) {
                xTaskNotifyIndexed(_recHandle, 0,
                                   static_cast<uint32_t>(RecPlayTask::RecCmd::REC_DONE),
                                   eSetValueWithOverwrite);
            }
            dbg.info("FusionTask: recording done -> RecPlayTask save\n");
        }


        // (hand/Jetson blend alpha is now decided inside NormalizeTask from
        // the scaled wrist Z, so no fused-J2 notify is needed here anymore.)

        // Live output to MotionPlanningTask. (During playback FusionTask is
        // suspended by UITask, so this path is naturally inactive then.)
        // fusedJointQueue is length-1: overwrite so the latest fusion always
        // wins instead of silently dropping frames.
        _outQueue.overwrite(out);

        // DEBUG:
        // #define _D1(x) ((int)(x)), ((int)(fabsf((x) - (int)(x)) * 10.0f))
        // dbg.logWithType("FUSION OUTPUT", COLOR_DARK_GREEN,
        //     "J1=%d.%d J2=%d.%d J3=%d.%d J4=%d.%d J5=%d.%d\n",
        //     _D1(out.angles[0]), _D1(out.angles[1]), _D1(out.angles[2]),
        //     _D1(out.angles[3]), _D1(out.angles[4]));
        // #undef _D1
    }
}
