#include "NPUTask.h"
#include "ElegantDebug.h"
#include <cmath>
#include <cstring>

#include "model.h"
#include "pmu_ethosu.h"

#include "UITask.h"


extern ElegantDebug dbg;



// Called once before scheduler start to open the Ethos-U peripheral.
fsp_err_t NPUTask::_initNPU() {
    fsp_err_t status = FSP_SUCCESS;
    status = RM_ETHOSU_Open(&g_rm_ethosu0_ctrl, &g_rm_ethosu0_cfg);
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

// ============================================================================
// Model pre-processing (agreed with the model author)
// ----------------------------------------------------------------------------
// Per frame:  elbow(mm) /500  -> x1,y1,z1
//             wrist(mm) /700  -> x2,y2,z2
//   elbow point:  mu_e   = (x1+y1+z1)/3,  sigma_e = std(x1,y1,z1)  (/3)
//                 norm   = (x1-mu_e)/sigma_e ... (population std, /3)
//   wrist point:  mu_w   = (x2+y2+z2)/3,  sigma_w = std(x2,y2,z2)  (/3)
//                 norm   = (x2-mu_w)/sigma_w ...
// 6 normalized values fill one 6-channel frame; 32 frames -> features[192].
// ============================================================================

// Normalize a 3-component point (in-place) using its own mean / population std.
// scale: per-point divisor applied to the raw mm values first.
void _normalizePoint(float out[3], const float raw[3], float scale) {
    const float x = raw[0] / scale;
    const float y = raw[1] / scale;
    const float z = raw[2] / scale;

    const float avg = (x + y + z) / 3.0f;
    const float x1 = x - avg, y1 = y - avg, z1 = z - avg;
    const float sigma = std::sqrt((x1 * x1 + y1 * y1 + z1 * z1) / 3.0f);

    out[0] = x1 / sigma;
    out[1] = y1 / sigma;
    out[2] = z1 / sigma;
}

void NPUTask::taskFunction() {
    dbg.ok("NPUTask started.\n");

    _initialized = (_initNPU() == FSP_SUCCESS) ? true : false;

    if (_initialized) {
        dbg.ok("NPU Opened successfully.\n");
        UITask::updateNPUStatus(UITask::NpuState::IDLE);
    } else {
        dbg.error("NPU INIT FAILED!\n");
    }

    // Sliding window of the last 32 frames, flattened in [frame, channel] order.
    float window[NPU_INPUT_SIZE] = { 0.0f };
    int   recvedFramesCnt = 0;

    char _errBuf[64];

    // Inference frequency estimation (sliding window over last N infer ticks).
    // configTICK_RATE_HZ = 1000 -> xTaskGetTickCount() is in ms already.
    uint32_t _inferTicks[NPU_FREQ_WINDOW] = {0};
    int      _inferIdx = 0;
    bool     _inferFilled = false;

    for (;;) {
        auto kp = _inQueue.receive(portMAX_DELAY);
        if (!kp) continue;

        // Shift the window left by one frame, drop the oldest.
        if (recvedFramesCnt >= NPU_INPUT_WINDOW) {
            memmove(window, window + NPU_INPUT_CHNS,
                    (NPU_INPUT_WINDOW - 1) * NPU_INPUT_CHNS * sizeof(float));
        }

        int base = (recvedFramesCnt < NPU_INPUT_WINDOW)
                   ? recvedFramesCnt * NPU_INPUT_CHNS
                   : (NPU_INPUT_WINDOW - 1) * NPU_INPUT_CHNS;
        _normalizePoint(&window[base + 0], kp->elbowCoord, 500.0f);  // elbow, mm -> /500
        _normalizePoint(&window[base + 3], kp->wristCoord, 700.0f);  // wrist, mm -> /700

        if (recvedFramesCnt < NPU_INPUT_WINDOW) {
            recvedFramesCnt++;
            continue;   // not enough history yet
        }


        if (_initialized) {
            // INFERENCE
            sharedDatatype::JointAngleData out = {};
            out.timestamp = kp->timestamp;

            memcpy(GetModelInputPtr_features(), window, NPU_INPUT_SIZE * sizeof(float));
            int status = RunModel(false);
            if (status == 0) {
                // UITask::updateNPUStatus(UITask::NpuState::RUNNING);

                out.angles[2] = GetModelOutputPtr_j3_deg_70055()[0];   // J3
                out.angles[4] = GetModelOutputPtr_j5_deg_70056()[0];   // J5

                out.angles[0] = 0.0f;
                out.angles[1] = 0.0f;
                out.angles[3] = 0.0f;
                out.angles[5] = 0.0f;
            } else {
                snprintf(_errBuf, sizeof(_errBuf), "NPU RunModel failed, status=%d\n", status);
                dbg.error(_errBuf);
            }

            _outQueue.overwrite(out);   // npuJointQueue is length-1: latest J3/J5 wins

            // ---- Inference frequency -> UI (index 2) ----
            uint32_t now = xTaskGetTickCount();
            _inferTicks[_inferIdx] = now;
            _inferIdx = (_inferIdx + 1) % NPU_FREQ_WINDOW;
            if (_inferIdx == 0) _inferFilled = true;

            if (_inferFilled && _uiHandle) {
                uint32_t oldest = _inferTicks[_inferIdx];   // index now points at oldest
                uint32_t span   = now - oldest;             // NPU_FREQ_WINDOW infers = window-1 periods
                if (span > 0) {
                    uint32_t avgPer = span / (NPU_FREQ_WINDOW - 1);   // avg period (ms)
                    // Guard against avgPer==0 (inference faster than 1ms/frame):
                    // skip this sample rather than divide by zero.
                    if (avgPer > 0) {
                        uint32_t hz = 1000u / avgPer;   // inference rate (Hz)
                        xTaskNotifyIndexed(_uiHandle, 2, hz, eSetValueWithOverwrite);
                    }
                }
            }
        }
    }
}
