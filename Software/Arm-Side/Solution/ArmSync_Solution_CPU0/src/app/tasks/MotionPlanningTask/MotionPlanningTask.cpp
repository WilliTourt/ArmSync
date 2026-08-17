#include "MotionPlanningTask.h"
#include "ElegantDebug.h"

extern ElegantDebug dbg;

// Joint motors: minDeg, maxDeg, reductionRatio, inverted, maxRPM
Motor MotionPlanningTask::_j1(-90.0f , 90.0f , 20.0f, false, 200);
Motor MotionPlanningTask::_j2(-22.5f , 125.0f, 30.0f, true , 300);
Motor MotionPlanningTask::_j3(-90.0f , 90.0f , 10.0f, false, 200);
Motor MotionPlanningTask::_j4(-122.0f, 0.0f  , 10.0f, true , 200);
Motor MotionPlanningTask::_j5(-90.0f , 90.0f , 1.0f , false, 150);
Motor MotionPlanningTask::_j6(-90.0f , 90.0f , 10.0f, true , 200);

void MotionPlanningTask::taskFunction() {
    dbg.ok("MotionPlanningTask started.\n");

    for (;;) {
        // Playback has priority: drain replayQueue first (RecPlayTask feeds it).
        auto rp = _replayQueue.receive(0);
        if (rp.has_value()) {
            sharedDatatype::MotionPlanPacket pkt = _arm.setAngles(rp->angles);
            pkt.timestamp = rp->timestamp;
            _outQueue.sendToBack(pkt, 0);
            dbg.logWithType("REPLAY", COLOR_CYAN,
                "m0(dir%d r%d a%d p%lu)\n",
                pkt.motors[0].dir, pkt.motors[0].rpm, pkt.motors[0].acc,
                (unsigned long)pkt.motors[0].pulse);
            continue;
        }

        // Otherwise block for live fused joint data.
        auto joint = _inQueue.receive(portMAX_DELAY);
        if (!joint) continue;

        sharedDatatype::MotionPlanPacket pkt = _arm.setAngles(joint->angles);
        pkt.timestamp = joint->timestamp;

        _outQueue.sendToBack(pkt, 0);

        // dbg.logWithType("MOTION PLAN", COLOR_GREEN,
        //     "m0(dir%d r%d a%d p%lu) m1(dir%d r%d a%d p%lu) m2(dir%d, r%d a%d p%lu) m3(dir%d r%d a%d p%lu) m4(dir%d r%d a%d p%lu) m5(dir%d r%d a%d p%lu)\n",
        //     pkt.motors[0].dir, pkt.motors[0].rpm, pkt.motors[0].acc, (unsigned long)pkt.motors[0].pulse,
        //     pkt.motors[1].dir, pkt.motors[1].rpm, pkt.motors[1].acc, (unsigned long)pkt.motors[1].pulse,
        //     pkt.motors[2].dir, pkt.motors[2].rpm, pkt.motors[2].acc, (unsigned long)pkt.motors[2].pulse,
        //     pkt.motors[3].dir, pkt.motors[3].rpm, pkt.motors[3].acc, (unsigned long)pkt.motors[3].pulse,
        //     pkt.motors[4].dir, pkt.motors[4].rpm, pkt.motors[4].acc, (unsigned long)pkt.motors[4].pulse,
        //     pkt.motors[5].dir, pkt.motors[5].rpm, pkt.motors[5].acc, (unsigned long)pkt.motors[5].pulse);
    }
}
