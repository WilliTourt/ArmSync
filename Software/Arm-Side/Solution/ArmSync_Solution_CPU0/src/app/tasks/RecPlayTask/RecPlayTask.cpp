#include "RecPlayTask.h"
#include "UITask.h"      // static updateHMS()
#include "ElegantDebug.h"

extern ElegantDebug dbg;

volatile bool RecPlayTask::_playActive = false;
bool RecPlayTask::_loop = false;

namespace {
// ms between frames at the configured rate
constexpr TickType_t FRAME_PERIOD_MS = pdMS_TO_TICKS(1000U / RecPlayTask::RECORD_RATE_HZ);
} // namespace

void RecPlayTask::taskFunction() {
    dbg.ok("RecPlayTask started.\n");

    FlashStorage flash;
    if (!flash.init()) {
        dbg.error("RecPlayTask: FlashStorage init failed\n");
    }

    // Playback pacing uses RECORDED timestamps only. recordedLastTs = previous
    // frame's recorded ts; recFrameTs = current frame's recorded ts. Never mix
    // with the send-time xTaskGetTickCount stamp.
    uint32_t recordedLastTs = 0, recFrameTs = 0;

    for (;;) {
        // ------------------------------------------------------------------
        // 1. Consume the latest command notification (index 0, non-blocking).
        // ------------------------------------------------------------------
        uint32_t notif = 0;
        if (xTaskNotifyWaitIndexed(0, 0, 0xFFFFFFFF, &notif, 0) == pdTRUE) {
            switch (static_cast<RecCmd>(notif)) {
                case RecCmd::REC_DONE:
                    // Drain any still-queued frames so the last part of the take
                    // isn't lost before we save.
                    {
                        auto f = _recQueue.receive(0);
                        float grip = _lastGrip;
                        while (f.has_value()) {
                            if (_frameCount < MAX_FRAMES) {
                                uint32_t off = _frameCount * sizeof(sharedDatatype::JointAngleData);
                                __builtin_memcpy(&_recBuf[off], &(*f), sizeof(sharedDatatype::JointAngleData));
                                // keep last grip if no new grip frame arrived this tick
                                if (auto g = _gripInQueue.receive(0)) { grip = g->grip_percent; }
                                _recGrip[_frameCount] = grip;
                                _frameCount++;
                            }
                            f = _recQueue.receive(0);
                        }
                        _lastGrip = grip;
                    }
                    if (_frameCount > 0) {
                        if (saveToFlash(flash)) {
                            dbg.info("RecPlayTask: saved %u frames\n", (unsigned)_frameCount);
                            // UITask::updateHMS("REC save OK");
                        } else {
                            dbg.error("RecPlayTask: flash save FAILED\n");
                            // UITask::updateHMS("REC save FAIL");
                        }
                    } else {
                        dbg.warning("RecPlayTask: nothing recorded\n");
                        // UITask::updateHMS("REC empty");
                    }
                    _frameCount = 0;
                    break;

                case RecCmd::PLAY_START:
                    // Clear any residual live frames so replay starts clean.
                    while (_liveQueue.receive(0).has_value()) { /* drain fully */ }

                    if (loadFromFlash(flash)) {
                        _replaying  = true;
                        _playActive = true;
                        _playIdx    = 0;   // start a fresh take
                        // Relative timestamps (frame 0 = 0): anchor the whole
                        // replay on the current tick. Frame 0 sends immediately;
                        // frame N sends at replayStartTick + relTs[N].
                        _replayStartTick = static_cast<uint32_t>(xTaskGetTickCount());
                        dbg.info("RecPlayTask: PLAY %u frames\n", (unsigned)_playCount);
                        // UITask::updateHMS("Replay start");
                    } else {
                        // No take stored: tell UITask so it resumes the
                        // suspended upstream chain (otherwise the arm stays
                        // frozen with everything held).
                        dbg.warning("RecPlayTask: no take stored, PLAY ignored\n");
                        _notifyUI(RecCmd::PLAY_DONE);   // -> UI resumes upstream
                        // UITask::updateHMS("No take stored");
                    }
                    break;

                case RecCmd::PLAY_END:
                case RecCmd::NONE:
                default:
                    _replaying  = false;
                    _playActive = false;
                    _playIdx    = 0;   // re-arm for the next PLAY
                    // Drain any residual replay frames so they don't leak into
                    // the next live control run.
                    while (_replayQueue.receive(0).has_value()) { /* drain */ }
                    break;
            }
        }

        // ------------------------------------------------------------------
        // 2. Data plane.
        // ------------------------------------------------------------------
        if (_replaying) {
            // Playback with ABSOLUTE time alignment. _recBuf timestamps are
            // relative offsets from frame 0 (frame 0 = 0). Frame 0 sends
            // immediately; frame N sends when the tick reaches
            // replayStartTick + relTs[N]. This is immune to per-frame
            // processing/send overhead (unlike subtracting deltas each loop,
            // which accumulated to ~2x slow).
            if (_playIdx < _playCount) {
                // Wait until this frame's absolute due tick (relTs is the
                // offset; frame 0 has relTs = 0, so it sends right away).
                sharedDatatype::JointAngleData frame;
                uint32_t off = _playIdx * sizeof(frame);
                __builtin_memcpy(&frame, &_recBuf[off], sizeof(frame));
                uint32_t dueTick = _replayStartTick + frame.timestamp;
                while (static_cast<int32_t>(dueTick - static_cast<uint32_t>(xTaskGetTickCount())) > 0) {
                    vTaskDelay(1);   // busy-wait in 1ms steps until due
                }

                // Stamp with the send-time tick and push to the replay queue.
                frame.timestamp = static_cast<uint32_t>(xTaskGetTickCount());
                if (!_replayQueue.sendToBack(frame, 0)) {
                    // Queue full: retry next loop, don't burn a delay yet.
                    vTaskDelay(1);
                    continue;
                }
                // Push this frame's recorded grip to CPUCommTask (eeDataQueue)
                // so the gripper servo follows the recording during playback.
                // Caveat: eeDataQueue is length-2 and OVERWRITE blocks when full
                // (xQueueOverwrite is only safe on length-1 queues). So drain one
                // old grip first (non-blocking) then send with 0 timeout never blocks.
                sharedDatatype::EndEffectorData grip;
                grip.grip_percent = _recGrip[_playIdx];
                grip.timestamp    = frame.timestamp;
                _gripOutQueue.receive(0);
                _gripOutQueue.sendToBack(grip, 0);
                _playIdx++;
                if ((_playIdx & 127u) == 0u) {
                    dbg.logWithType("REPLAY", COLOR_CYAN,
                                    "sent idx=%lu/%lu\n",
                                    (unsigned long)_playIdx, (unsigned long)_playCount);
                }

                if (_playIdx >= _playCount) {
                    if (_loop) {
                        // Infinite replay: wrap back to frame 0. relTs is
                        // frame-relative, so just re-anchor the start tick and
                        // reset the index (frame 0 plays again immediately).
                        _playIdx = 0;
                        _replayStartTick = static_cast<uint32_t>(xTaskGetTickCount());
                        dbg.logWithType("REPLAY", COLOR_CYAN, "loop wrap\n");
                        vTaskDelay(1);
                        continue;
                    } else {
                        dbg.info("RecPlayTask: playback done (%u frames)\n",
                                 (unsigned)_playCount);
                        _replaying  = false;
                        _playActive = false;
                        _playIdx    = 0;
                        // UITask::updateHMS("Replay done");
                        _notifyUI(RecCmd::PLAY_DONE);
                        continue;
                    }
                }
            } else {
                _replaying  = false;
                _playActive = false;
                _playIdx    = 0;
                _notifyUI(RecCmd::PLAY_DONE);
            }
        } else {
            // Not replaying: accumulate whatever Fusion pushes on recQueue.
            // (Fusion only sends while recording, so this is naturally a take.)
            auto f = _recQueue.receive(0);
            if (f.has_value()) {
                if (_frameCount < MAX_FRAMES) {
                    uint32_t off = _frameCount * sizeof(sharedDatatype::JointAngleData);
                    __builtin_memcpy(&_recBuf[off], &(*f), sizeof(sharedDatatype::JointAngleData));
                    // Record the matching grip for this frame (keep last if none).
                    if (auto g = _gripInQueue.receive(0)) { _lastGrip = g->grip_percent; }
                    _recGrip[_frameCount] = _lastGrip;
                    _frameCount++;
                } else {
                    // Buffer full: drop further frames until REC_DONE is sent.
                    dbg.info("RecPlayTask: take buffer full (%u)\n", (unsigned)MAX_FRAMES);
                    UITask::updateHMS("REC FULL");   // surface the truncation to the UI
                }
            }
            vTaskDelay(FRAME_PERIOD_MS);   // loop pacing (recording side)
        }
    }
}

void RecPlayTask::_notifyUI(RecCmd cmd) {
    if (_uiHandle != nullptr) {
        xTaskNotifyIndexed(_uiHandle, 0, static_cast<uint32_t>(cmd), eSetValueWithOverwrite);
    }
}

bool RecPlayTask::saveToFlash(FlashStorage &flash) {
    // RAM take (no OSPI write): the frames already live in _recBuf. Convert
    // the per-frame timestamps to RELATIVE offsets from frame 0 (frame 0 -> 0)
    // so playback can align each frame to an absolute due tick. Power-loss
    // persistence is intentionally not provided by this path.
    (void)flash;
    if (_frameCount > 0) {
        sharedDatatype::JointAngleData tmp;
        __builtin_memcpy(&tmp, &_recBuf[0], sizeof(tmp));
        uint32_t base = tmp.timestamp;
        for (uint32_t i = 0; i < _frameCount; i++) {
            uint32_t off = i * sizeof(sharedDatatype::JointAngleData);
            __builtin_memcpy(&tmp, &_recBuf[off], sizeof(tmp));
            tmp.timestamp = tmp.timestamp - base;   // relative ms from frame 0
            __builtin_memcpy(&_recBuf[off], &tmp, sizeof(tmp));
        }
    }
    _playCount = _frameCount;
    _haveTake  = (_playCount > 0);
    return true;
}

bool RecPlayTask::loadFromFlash(FlashStorage &flash) {
    // RAM take: nothing to load from flash. If a take was recorded this boot,
    // _playCount already holds the frame count and _recBuf the data.
    (void)flash;
    if (!_haveTake) {
        return false;
    }
    return true;
}
