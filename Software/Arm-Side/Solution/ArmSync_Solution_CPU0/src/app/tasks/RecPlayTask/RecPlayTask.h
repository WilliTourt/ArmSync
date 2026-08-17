#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "queues.h"
#include "app/flash/FlashStorage.h"

/**
 * @brief Record / playback of the arm's fused joint angles.
 *
 * RECORD: FusionTask mirrors each fused JointAngleData to recQueue only while
 *   the UI says REC. RecPlayTask constantly receive(0)s recQueue and appends
 *   whatever arrives into _recBuf (so it only accumulates while recording).
 *   On a REC_DONE notification (sent by FusionTask after the last frame) it
 *   flushes the accumulated take to external flash (W25Q256JV) with a header.
 *
 * PLAY: On PLAY_START it reads the take back from flash and re-injects the
 *   frames to replayQueue at 30 Hz, which MotionPlanningTask consumes. It keeps
 *   sending until a PLAY_END notification (or the take is exhausted).
 *
 * All commands arrive as task notifications on index 0 (RecCmd value), pushed
 * by UITask (PLAY_START/PLAY_END) and FusionTask (REC_DONE).
 */
class RecPlayTask : public FreeRTOS::Task {
    public:
        // Notification values received on index 0.
        enum class RecCmd : uint32_t {
            NONE       = 0,   // also: PLAY_END / reset
            REC_DONE   = 1,   // FusionTask: recording finished, save to flash
            PLAY_START = 2,   // UITask: begin replay -> read flash, send to replayQueue
            PLAY_END   = 3,   // UITask: stop replay
            PLAY_DONE  = 4,   // RecPlayTask->UITask: take exhausted naturally
        };

        // Sampling / replay rate (Hz). Public for the timing constant.
        static constexpr uint16_t RECORD_RATE_HZ = 30U;

        // Playback pacing clamps (ms) on the recorded inter-frame delta.
        static constexpr uint32_t PLAY_MIN_DELAY_MS = 1U;      // avoid busy-spin on 0/dup
        static constexpr uint32_t PLAY_MAX_DELAY_MS = 1000U;   // cap long stalls

        RecPlayTask(FreeRTOS::Queue<sharedDatatype::JointAngleData> &inRecQueue,
                    FreeRTOS::Queue<sharedDatatype::JointAngleData> &outReplayQueue,
                    FreeRTOS::Queue<sharedDatatype::JointAngleData> &liveQueue)
            : Task(tskIDLE_PRIORITY + 2, 2048, "RecPlay"),
              _recQueue(inRecQueue), _replayQueue(outReplayQueue), _liveQueue(liveQueue) {}

        // Give RecPlayTask the UITask handle so it can report PLAY_DONE back.
        void setUIHandle(TaskHandle_t h) { _uiHandle = h; }

        // True while a replay is feeding replayQueue. Exposed for diagnostics.
        bool isReplaying() const { return _replaying; }

    private:
        void taskFunction() override;

        // --- Flash layout --------------------------------------------------
        static constexpr uint32_t FLASH_OFFSET    = 0x10000UL;  // recording region
        static constexpr uint32_t HEADER_SIZE     = 16U;        // magic+count+rate+reserved (8-aligned)
        static constexpr uint32_t DATA_OFFSET     = FLASH_OFFSET + HEADER_SIZE;

        // 60 s @ 30 Hz = 1800 frames; 28 B/frame -> ~49 KB, needs 13 x 4 KB sectors.
        static constexpr uint32_t MAX_FRAMES = 1800U;
        static constexpr uint32_t RECORD_SECTORS =
            (MAX_FRAMES * sizeof(sharedDatatype::JointAngleData) + HEADER_SIZE
             + FlashStorage::SECTOR_SIZE - 1U) / FlashStorage::SECTOR_SIZE;

        // Flash header/frame constants.
        static constexpr uint32_t MAGIC           = 0x524350B0UL; // "RCP0"

        bool saveToFlash();
        bool loadFromFlash();
        void _notifyUI(RecCmd cmd);   // push a RecCmd back to UITask

        uint8_t  _recBuf[MAX_FRAMES * sizeof(sharedDatatype::JointAngleData)] = {};
        uint16_t _frameCount = 0;   // frames accumulated in the current take
        uint16_t _playCount  = 0;   // frames loaded from flash for playback
        uint16_t _playIdx    = 0;   // next frame index to send during playback
        bool     _replaying  = false;
        TaskHandle_t _uiHandle = nullptr;   // UITask, for PLAY_DONE report

        FreeRTOS::Queue<sharedDatatype::JointAngleData> &_recQueue;    // from FusionTask (record)
        FreeRTOS::Queue<sharedDatatype::JointAngleData> &_replayQueue; // -> MotionPlanningTask (playback)
        FreeRTOS::Queue<sharedDatatype::JointAngleData> &_liveQueue;   // fusedJointQueue (drain on PLAY_START)

        static volatile bool _playActive;
};
