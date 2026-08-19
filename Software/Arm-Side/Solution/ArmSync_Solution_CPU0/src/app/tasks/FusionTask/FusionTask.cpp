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

    float latestJ5    = 0.0f;  // forearm roll (deg), from hand quaternion
    float latestJ3    = 0.0f;
    float latestPitch = 50.0f; // default: J6 center
    bool  handValid   = false;

    for (;;) {

        // Always drain hand data (J5 roll + pitch), non-blocking.
        auto hand = _handQueue.receive(0);
        if (hand) {
            latestJ3    = hand->j3deg;
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

        // J1/J2/J4: IK only
        out.angles[0] = ik->angles[0];
        out.angles[1] = ik->angles[1];
        out.angles[3] = ik->angles[3];

        // J3: fusion of IK + hand + NPU
        if (handValid) {
            // All three sources available.
            out.angles[2] = J3_IK_ALPHA   * ik->angles[2]
                          + J3_HAND_ALPHA * latestJ3
                          + J3_NPU_ALPHA  * (npu ? npu->angles[2] : ik->angles[2]);
        } else {
            // No hand data: fall back to NPU first, else IK.
            out.angles[2] = (npu) ? npu->angles[2] : ik->angles[2];
        }

        // J5: fusion of hand + NPU
        {
            float npuJ5 = (npu) ? npu->angles[4] : latestJ5;   // NPU fallback = hand
            if (handValid) {
                out.angles[4] = J5_HAND_ALPHA * latestJ5 + J5_NPU_ALPHA * npuJ5;
            } else {
                out.angles[4] = (npu) ? npu->angles[4] : 0.0f;
            }
        }
        out.angles[4] -= 60.0f;

        // J6: hand only
        out.angles[5] = handValid ? _mapPitchToJ6(latestPitch) : 0.0f;

        // Smooth the fused output before it reaches the record tap / live queue.
        _applyLowPass(out);
        
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
        // fusedJointQueue is length-1: overwrite so the latest fusion always
        // wins instead of silently dropping frames.
        _outQueue.overwrite(out);

        // Float-free but precision-kept: print as int.frac (one decimal
        #define _D1(x) ((int)(x)), ((int)(fabsf((x) - (int)(x)) * 10.0f))
        dbg.logWithType("FUSION OUTPUT", COLOR_DARK_GREEN,
            "J1=%d.%d J2=%d.%d J3=%d.%d J4=%d.%d J5=%d.%d\n",
            _D1(out.angles[0]), _D1(out.angles[1]), _D1(out.angles[2]),
            _D1(out.angles[3]), _D1(out.angles[4]));
        #undef _D1
    }
}
