#include "MotionPlanningTask.h"
#include "ElegantDebug.h"
#include <cmath>

extern ElegantDebug dbg;

// Max velocities per joint
static const uint16_t kMaxVel[6] = {3000, 4500, 1500, 1500, 150, 1500};

// Joint motors: minDeg, maxDeg, reductionRatio, inverted, velocity(RPM)
Motor MotionPlanningTask::_j1(-90.0f , 90.0f , 20.0f, false, kMaxVel[0]);
Motor MotionPlanningTask::_j2(-22.5f , 125.0f, 30.0f, true , kMaxVel[1]);
Motor MotionPlanningTask::_j3(-90.0f , 90.0f , 10.0f, false, kMaxVel[2]);
Motor MotionPlanningTask::_j4(-122.0f, 0.0f  , 10.0f, true , kMaxVel[3]);
Motor MotionPlanningTask::_j5(-90.0f , 90.0f , 1.0f , false, kMaxVel[4]);
Motor MotionPlanningTask::_j6(-90.0f , 90.0f , 10.0f, true , kMaxVel[5]);

// P-gain: rpm per degree of error. Start conservative; tune in the field.
static const float kKp = MotionPlanningTask::PID_KP;

void MotionPlanningTask::_applyPid(sharedDatatype::MotionPlanPacket &pkt,
                                   const float targetAngles[6]) {
    // vel = clamp(Kp * |target - feedback|, vel_min, max). Stop in deadband.
    // acc is always 0: Emm_V5 starts directly and the P loop owns the speed.
    for (int i = 0; i < 6; i++) {
        sharedDatatype::MotorCommand &cmd = pkt.motors[i];
        if (cmd.pulse == 0u) {
            // No target -> leave idle, but still normalise acc to 0 so the
            // motor is always in the same (PID-driven) control mode.
            cmd.acc = 0u;
            continue;
        }

        float const err = targetAngles[i] - _lastFeedback[i];
        if (std::fabs(err) < PID_DEADBAND_DEG) {
            cmd.rpm = 0u;   // at target -> stop
            cmd.acc = 0u;
            continue;
        }

        float vel = kKp * std::fabs(err);
        float const velMax = static_cast<float>(kMaxVel[i]);
        float const velMin = velMax * PID_VEL_MIN_PCT;
        if (vel < velMin) vel = velMin;
        if (vel > velMax) vel = velMax;

        cmd.rpm = static_cast<uint16_t>(vel);
        cmd.acc = 0u;   // Emm_V5 direct start, no internal accel curve
    }
}

void MotionPlanningTask::taskFunction() {
    dbg.ok("MotionPlanningTask started.\n");

    for (;;) {
        // Latest motor feedback for the PID (non-blocking; 30ms @ IPC).
        // Keep the last known-good values; a missing frame must NOT be read as
        // "all joints at 0 deg" (that would make the PID think it's far from
        // target and slam the speed).
        auto fb = _fbQueue.receive(0);
        if (fb) {
            for (int i = 0; i < 6; i++) {
                _lastFeedback[i] = fb->jointAngle[i];
            }
        }

        // Playback has priority: drain replayQueue first (RecPlayTask feeds it).
        auto rp = _replayQueue.receive(0);
        if (rp.has_value()) {
            sharedDatatype::MotionPlanPacket pkt = _arm.setAngles(rp->angles);
            pkt.timestamp = rp->timestamp;
            // Playback also runs the P loop so it approaches targets smoothly
            // (same behaviour as live control, avoids overshoot at speed).
            _applyPid(pkt, rp->angles);
            _outQueue.sendToBack(pkt, 0);
            continue;
        }

        // Otherwise block for live fused joint data.
        auto joint = _inQueue.receive(portMAX_DELAY);
        if (!joint) continue;

        // Build the base plan from the fused target angles.
        sharedDatatype::MotionPlanPacket pkt = _arm.setAngles(joint->angles);
        pkt.timestamp = joint->timestamp;

        // PID: drive each motor's velocity from the angle error.
        _applyPid(pkt, joint->angles);

        _outQueue.sendToBack(pkt, 0);
    }
}
