#pragma once

#include <FreeRTOS/Task.hpp>
#include <FreeRTOS/Queue.hpp>
#include "queues.h"
#include "arm.h"

/**
 * @brief Holds the Arm (6 Motor planners) and turns fused joint angles
 *        into a MotionPlanPacket for CPUCommTask / M33.
 */
class MotionPlanningTask : public FreeRTOS::Task {
    public:
        MotionPlanningTask(FreeRTOS::Queue<sharedDatatype::JointAngleData>   &inQueue,
                           FreeRTOS::Queue<sharedDatatype::JointAngleData>   &inReplayQueue,
                           FreeRTOS::Queue<sharedDatatype::MotionPlanPacket> &outQueue,
                           FreeRTOS::Queue<sharedDatatype::IPCFeedback>      &fbQueue)
            : Task(tskIDLE_PRIORITY + 3, 2048, "MotionPlan"),
              _inQueue(inQueue), _replayQueue(inReplayQueue), _outQueue(outQueue),
              _fbQueue(fbQueue),
              _arm(_j1, _j2, _j3, _j4, _j5, _j6) {}

        // P-gain for the velocity PID: vel[rpm] = Kp * |deg error|.
        // Set so a full-swing error (~MAX_SPAN_DEG) reaches max velocity.
        static constexpr float PID_KP = 0.18f;            // rpm per degree (tune)
        static constexpr float PID_DEADBAND_DEG = 4.0f;   // stop within this error
        static constexpr float PID_VEL_MIN_PCT = 0.05f;   // min vel = 5% of max

        // First-order low-pass applied to the PID target angles (extra
        // smoothing on top of Fusion's LP; small delay acceptable). Smaller
        // alpha = smoother but more lag.
        static constexpr float PID_TGT_ALPHA = 0.22f;

    private:
        void taskFunction() override;

        // Apply the P velocity loop to pkt.motors from target angles vs the
        // last-known feedback. Shared by live and playback paths so both
        // smooth to target identically.
        void _applyPid(sharedDatatype::MotionPlanPacket &pkt,
                       const float targetAngles[6]);

        // Joint motors: minDeg, maxDeg, reductionRatio, inverted, velocity
        static Motor _j1;   // J1 upper swing
        static Motor _j2;   // J2 upper abduction
        static Motor _j3;   // J3 upper rotation
        static Motor _j4;   // J4 forearm swing
        static Motor _j5;   // J5 forearm rotation
        static Motor _j6;   // J6 wrist swing

        Arm _arm;

        FreeRTOS::Queue<sharedDatatype::JointAngleData>  &_inQueue;      // live fused joints
        FreeRTOS::Queue<sharedDatatype::JointAngleData>  &_replayQueue;  // playback feed
        FreeRTOS::Queue<sharedDatatype::MotionPlanPacket> &_outQueue;
        FreeRTOS::Queue<sharedDatatype::IPCFeedback>      &_fbQueue;     // latest motor feedback (PID)

        // Last known-good motor feedback, retained across loops so a missing
        // feedback frame doesn't get treated as "all joints at 0 deg".
        float _lastFeedback[6] = {0.0f};

        // Low-pass filter state for the PID target angles (one per joint).
        float _tgtFiltered[6] = {0.0f};
        bool  _tgtFilterInit  = false;
};
