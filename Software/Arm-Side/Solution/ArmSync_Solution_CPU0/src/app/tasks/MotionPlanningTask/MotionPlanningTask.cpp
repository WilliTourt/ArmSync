#include "MotionPlanningTask.h"
#include "ElegantDebug.h"

extern ElegantDebug dbg;

// Joint motors: minDeg, maxDeg, reductionRatio, inverted, velocity
Motor MotionPlanningTask::g_j1(-90.0f,  90.0f, 20.0f, false, 150);
Motor MotionPlanningTask::g_j2(-22.5f, 125.0f, 30.0f, true,  100);
Motor MotionPlanningTask::g_j3(-90.0f,  90.0f, 10.0f, false, 100);
Motor MotionPlanningTask::g_j4(-122.0f, 0.0f,  10.0f, true,  100);
Motor MotionPlanningTask::g_j5(-90.0f,  90.0f,  1.0f, false, 100);
Motor MotionPlanningTask::g_j6(-90.0f,  90.0f, 10.0f, true,  100);

void MotionPlanningTask::taskFunction() {
    dbg.info("MotionPlanningTask started.\n");

    for (;;) {
        auto joint = _inQueue.receive(portMAX_DELAY);
        if (!joint) continue;

        sharedDatatype::MotionPlanPacket pkt = _arm.setAngles(joint->angles);
        pkt.timestamp = joint->timestamp;

        _outQueue.sendToBack(pkt, 0);

        dbg.logWithType("PLAN", COLOR_CYAN,
            "m0(dir%d r%d a%d p%lu) m3(dir%d r%d a%d p%lu)\n",
            pkt.motors[0].dir, pkt.motors[0].rpm, pkt.motors[0].acc, (unsigned long)pkt.motors[0].pulse,
            pkt.motors[3].dir, pkt.motors[3].rpm, pkt.motors[3].acc, (unsigned long)pkt.motors[3].pulse);
    }
}
