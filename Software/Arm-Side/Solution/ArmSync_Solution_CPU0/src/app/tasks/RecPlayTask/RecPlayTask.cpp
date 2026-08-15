#include "RecPlayTask.h"
#include "ElegantDebug.h"

extern ElegantDebug dbg;

volatile bool RecPlayTask::_playActive = false;

namespace {
// ms between frames at the configured rate
constexpr TickType_t FRAME_PERIOD_MS = pdMS_TO_TICKS(1000U / RecPlayTask::RECORD_RATE_HZ);
} // namespace

void RecPlayTask::taskFunction() {
    dbg.info("RecPlayTask started.\n");

    FlashStorage flash;
    if (!flash.init()) {
        dbg.error("RecPlayTask: FlashStorage init failed\n");
    }

    for (;;) {
        // ------------------------------------------------------------------
        // 1. Consume the latest command notification (index 0, non-blocking).
        // ------------------------------------------------------------------
        uint32_t notif = 0;
        if (xTaskNotifyWaitIndexed(0, 0, UINT32_MAX, &notif, 0) == pdTRUE) {
            switch (static_cast<RecCmd>(notif)) {
                case RecCmd::REC_DONE:
                    // Drain any still-queued frames so the last part of the take
                    // isn't lost before we save.
                    {
                        auto f = _recQueue.receive(0);
                        while (f.has_value()) {
                            if (_frameCount < MAX_FRAMES) {
                                uint32_t off = _frameCount * sizeof(sharedDatatype::JointAngleData);
                                __builtin_memcpy(&_recBuf[off], &(*f), sizeof(sharedDatatype::JointAngleData));
                                _frameCount++;
                            }
                            f = _recQueue.receive(0);
                        }
                    }
                    if (_frameCount > 0) {
                        if (saveToFlash()) {
                            dbg.info("RecPlayTask: saved %u frames\n", (unsigned)_frameCount);
                        } else {
                            dbg.error("RecPlayTask: flash save FAILED\n");
                        }
                    } else {
                        dbg.warning("RecPlayTask: nothing recorded\n");
                    }
                    _frameCount = 0;
                    break;

                case RecCmd::PLAY_START:
                    // Clear any residual live frames so replay starts clean.
                    while (_liveQueue.receive(0).has_value()) { /* drain fully */ }

                    if (loadFromFlash()) {
                        _replaying  = true;
                        _playActive = true;
                        _playIdx    = 0;   // start a fresh take
                        dbg.info("RecPlayTask: PLAY %u frames\n", (unsigned)_playCount);
                    } else {
                        dbg.warning("RecPlayTask: no take stored, PLAY ignored\n");
                    }
                    break;

                case RecCmd::PLAY_END:
                case RecCmd::NONE:
                default:
                    _replaying  = false;
                    _playActive = false;
                    _playIdx    = 0;   // re-arm for the next PLAY
                    break;
            }
        }

        // ------------------------------------------------------------------
        // 2. Data plane.
        // ------------------------------------------------------------------
        if (_replaying) {
            // Playback paced by the RECORDED timestamp deltas, so the motion
            // rhythm (including pauses and speed variations) is reproduced 1:1.
            if (_playIdx < _playCount) {
                sharedDatatype::JointAngleData frame;
                uint32_t off = _playIdx * sizeof(frame);
                __builtin_memcpy(&frame, &_recBuf[off], sizeof(frame));

                // Stamp with the send-time tick and push to the replay queue.
                frame.timestamp = static_cast<uint32_t>(xTaskGetTickCount());
                if (!_replayQueue.sendToBack(frame, 0)) {
                    // Queue full: retry next loop, don't burn a delay yet.
                    vTaskDelay(1);
                    continue;
                }
                _playIdx++;

                if (_playIdx >= _playCount) {
                    dbg.info("RecPlayTask: playback done (%u frames)\n",
                             (unsigned)_playCount);
                    _replaying  = false;
                    _playActive = false;
                    _playIdx    = 0;
                    _notifyUI(RecCmd::PLAY_DONE);
                } else {
                    // Wait the recorded interval to the NEXT frame.
                    sharedDatatype::JointAngleData next;
                    __builtin_memcpy(&next, &_recBuf[_playIdx * sizeof(frame)], sizeof(frame));
                    uint32_t dt = next.timestamp - frame.timestamp;
                    if (dt < PLAY_MIN_DELAY_MS) dt = PLAY_MIN_DELAY_MS;
                    if (dt > PLAY_MAX_DELAY_MS) dt = PLAY_MAX_DELAY_MS;
                    vTaskDelay(pdMS_TO_TICKS(dt));
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
                    _frameCount++;
                } else {
                    // Buffer full: drop further frames until REC_DONE is sent.
                    dbg.info("RecPlayTask: take buffer full (%u)\n", (unsigned)MAX_FRAMES);
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

bool RecPlayTask::saveToFlash() {
    FlashStorage flash;
    if (!flash.init()) return false;

    // Header: magic | frameCount | rateHz | reserved (zero-padded to 16, 8-aligned)
    uint8_t hdr[HEADER_SIZE] = {};
    uint32_t magic = MAGIC;
    uint32_t count = _frameCount;
    uint16_t rate  = RECORD_RATE_HZ;
    __builtin_memcpy(&hdr[0], &magic, 4);
    __builtin_memcpy(&hdr[4], &count, 4);
    __builtin_memcpy(&hdr[8], &rate,  2);

    // Erase the whole recording region (multi-sector) before writing a new take.
    for (uint32_t s = 0; s < RECORD_SECTORS; s++) {
        if (!flash.eraseSector(FLASH_OFFSET + s * FlashStorage::SECTOR_SIZE)) {
            return false;
        }
    }
    if (!flash.write(FLASH_OFFSET, hdr, HEADER_SIZE)) return false;
    if (!flash.write(DATA_OFFSET, _recBuf, _frameCount * sizeof(sharedDatatype::JointAngleData))) {
        return false;
    }
    return true;
}

bool RecPlayTask::loadFromFlash() {
    FlashStorage flash;

    uint8_t hdr[HEADER_SIZE];
    flash.read(FLASH_OFFSET, hdr, HEADER_SIZE);

    uint32_t magic = 0, count32 = 0;
    __builtin_memcpy(&magic,  &hdr[0], 4);
    __builtin_memcpy(&count32, &hdr[4], 4);
    if (magic != MAGIC) return false;                 // no take / erased
    if (count32 == 0 || count32 > MAX_FRAMES) return false;

    _playCount = static_cast<uint16_t>(count32);
    flash.read(DATA_OFFSET, _recBuf, _playCount * sizeof(sharedDatatype::JointAngleData));
    return true;
}
