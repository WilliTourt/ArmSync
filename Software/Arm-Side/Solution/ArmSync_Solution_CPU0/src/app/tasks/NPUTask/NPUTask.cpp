#include "NPUTask.h"
#include "ElegantDebug.h"
#include <cmath>
#include <cstring>

extern ElegantDebug dbg;

// ============================================================================
// Ethos-U model integration hook
// ----------------------------------------------------------------------------
// The generated model files (model.h/c, compute_sub_0000/0002, sub_0001_*,
// kernel_library_*, model_io_data.*, model_test_data.*) are NOT in this
// project yet, and the FSP NPU peripheral (rm_ethosu0) is not configured.
//
// To enable the real inference:
//   1. In e2studio/RASC add the Ethos-U NPU (rm_ethosu0) to the CPU0 project
//      and regenerate.
//   2. Copy the model .c/.h files into this project's source tree and add
//      the folder as a source folder.
//   3. Define ARMSYNC_NPU_READY (a compile define) and call
//      NPUTask::initNPU() once from cpp_main before the scheduler starts.
// The code below is ready to use those accessors once the macro is defined.
// ============================================================================

// Model input: [1,1,32,6] flattened = 192 floats. 6 channels per frame are a
// PLACEHOLDER (elbow[3] + wrist[3]). The real keypoint->channel mapping is
// still TBD with the classmate who trained the model — swap the fill below
// once it's agreed.
static constexpr int NPU_WINDOW = 32;   // frames of history
static constexpr int NPU_CHANNELS = 6;  // elbow[3]+wrist[3] placeholder
static constexpr int NPU_INPUT_SIZE = NPU_WINDOW * NPU_CHANNELS;  // 192

#ifdef ARMSYNC_NPU_READY
#include "model.h"
static bool s_npuInitialized = false;
// Called once before scheduler start to open the Ethos-U peripheral.
void NPUTask::initNPU() {
    // e.g. status = RM_ETHOSU_Open(&g_rm_ethosu0_ctrl, &g_rm_ethosu0_cfg);
    // plus any PMU setup from the classmate's hal_entry.c
}
#endif

void NPUTask::taskFunction() {
    dbg.ok("NPUTask started.\n");

    // Sliding window of the last 32 frames, flattened in [frame, channel] order.
    float window[NPU_INPUT_SIZE] = {0.0f};
    int   frameCount = 0;

    for (;;) {
        auto kp = _inQueue.receive(portMAX_DELAY);
        if (!kp) continue;

        // Shift the window left by one frame, drop the oldest.
        if (frameCount >= NPU_WINDOW) {
            memmove(window, window + NPU_CHANNELS,
                    (NPU_WINDOW - 1) * NPU_CHANNELS * sizeof(float));
        }

        // Append the newest frame. PLACEHOLDER: elbow[3]+wrist[3] as channels.
        // TODO: replace with the agreed keypoint mapping once known.
        int base = (frameCount < NPU_WINDOW)
                   ? frameCount * NPU_CHANNELS
                   : (NPU_WINDOW - 1) * NPU_CHANNELS;
        window[base + 0] = kp->elbowCoord[0];
        window[base + 1] = kp->elbowCoord[1];
        window[base + 2] = kp->elbowCoord[2];
        window[base + 3] = kp->wristCoord[0];
        window[base + 4] = kp->wristCoord[1];
        window[base + 5] = kp->wristCoord[2];

        if (frameCount < NPU_WINDOW) {
            frameCount++;
            continue;   // not enough history yet
        }

        // ==================== Inference ====================
        sharedDatatype::JointAngleData out = {};
        out.timestamp = kp->timestamp;

#ifdef ARMSYNC_NPU_READY
        memcpy(GetModelInputPtr_features(), window, NPU_INPUT_SIZE * sizeof(float));
        int status = RunModel(false);
        if (status == 0) {
            out.angles[2] = GetModelOutputPtr_j3_deg_70055()[0];   // J3
            out.angles[4] = GetModelOutputPtr_j5_deg_70056()[0];   // J5
        } else {
            dbg.error("NPU RunModel failed (status=%d)\n", status);
        }
#else
        // Skeleton mode: publish zeros for J3/J5 until the model is wired in.
        // This keeps the pipeline (window -> output -> Fusion) testable.
        out.angles[2] = 0.0f;   // J3
        out.angles[4] = 0.0f;   // J5
#endif
        // J1/J2/J4/J6 stay 0: Fusion blends those from IK, not the NPU.

        _outQueue.sendToBack(out, 0);
    }
}
