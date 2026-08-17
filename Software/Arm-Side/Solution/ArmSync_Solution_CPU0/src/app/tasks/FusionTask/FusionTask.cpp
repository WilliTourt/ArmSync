#include "FusionTask.h"
#include "RecPlayTask/RecPlayTask.h"
#include "ElegantDebug.h"
#include <cmath>

extern ElegantDebug dbg;

float FusionTask::_mapPitchToJ6(float pitch_percent) const {
    if (pitch_percent < 0.0f) pitch_percent = 0.0f;
    if (pitch_percent > 100.0f) pitch_percent = 100.0f;

    return J6_MIN_DEG + (pitch_percent / 100.0f) * (J6_MAX_DEG - J6_MIN_DEG);
}

void FusionTask::setUIHandle(TaskHandle_t handle) {
    _uiHandle = handle;
}

void FusionTask::setRecHandle(TaskHandle_t handle) {
    _recHandle = handle;
}

void FusionTask::setNormalizeHandle(TaskHandle_t handle) {
    _normalizeHandle = handle;
}

void FusionTask::taskFunction() {
    dbg.ok("FusionTask started.\n");

    float latestJ5  = 0.0f;    // forearm roll (deg), from hand quaternion
    float latestPitch = 50.0f; // default: J6 center
    bool  handValid = false;

    for (;;) {

        // Always drain hand data (J5 roll + pitch), non-blocking.
        auto hand = _handQueue.receive(0);
        if (hand) {
            latestJ5    = hand->j5deg;
            latestPitch = hand->pitch_percent;
            handValid   = true;
        }

        // Block on IK data (main fusion trigger)
        auto ik = _ikQueue.receive(portMAX_DELAY);
        if (!ik) continue;

        // Try to grab latest NPU data (non-blocking)
        auto npu = _npuQueue.receive(0);

        sharedDatatype::JointAngleData out = {};
        out.timestamp = ik->timestamp;

        // J1~J4: complementary blend of IK and NPU (J5/J6 come from hand).
        for (int i = 0; i < 4; i++) {
            if (npu) {
                out.angles[i] = FUSION_ALPHA * ik->angles[i] +
                               (1.0f - FUSION_ALPHA) * npu->angles[i];
            } else {
                // no NPU data yet -> pure IK
                out.angles[i] = ik->angles[i];
            }
        }

        // J5: forearm roll from hand quaternion only (IK cannot sense it).
        out.angles[4] = handValid ? latestJ5 : 0.0f;

        // J6: mapped from pitch (not blended)
        out.angles[5] = handValid ? _mapPitchToJ6(latestPitch) : 0.0f;




        // Poll the latest UI command (index 0, non-blocking, overwrite).
        // UITask pushes UICommand::REC to start, UICommand::NONE to stop.
        uint32_t uiVal = static_cast<uint32_t>(sharedDatatype::UICommand::NONE);
        if (_uiHandle != nullptr) {
            xTaskNotifyWaitIndexed(0, 0, 0xFFFFFFFF, &uiVal, 0);
        }
        const auto uiCmd = static_cast<sharedDatatype::UICommand>(uiVal);

        // Detect state transition for REC start / REC end.
        const bool wasRecording = _recording;
        _recording = (uiCmd == sharedDatatype::UICommand::REC);

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



        
        // Notify NormalizeTask with fused J2 (deg, x100 fixed-point) so it can
        // pick this frame's hand/Jetson blend alpha for the next frame.
        if (_normalizeHandle != nullptr) {
            int32_t j2x100 = (int32_t)(out.angles[1] * 100.0f);
            xTaskNotifyIndexed(_normalizeHandle, 0,
                               (uint32_t)j2x100, eSetValueWithOverwrite);
        }

        // Live output to MotionPlanningTask. (During playback FusionTask is
        // suspended by UITask, so this path is naturally inactive then.)
        _outQueue.sendToBack(out, 0);

        // // Float-free but precision-kept: print as int.frac (one decimal
        // #define _D1(x) ((int)(x)), ((int)(fabsf((x) - (int)(x)) * 10.0f))
        // // NOTICE: NO NPU data now, so this log is just IK data with J5 deg
        // dbg.logWithType("FUSION OUTPUT", COLOR_DARK_GREEN,
        //     "J1=%d.%d J2=%d.%d J3=%d.%d J4=%d.%d J5=%d.%d\n",
        //     _D1(out.angles[0]), _D1(out.angles[1]), _D1(out.angles[2]),
        //     _D1(out.angles[3]), _D1(out.angles[4]));
        // #undef _D1
    }
}
