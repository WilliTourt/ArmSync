#include "normalizeTask.h"
#include "ElegantDebug.h"

extern ElegantDebug dbg;

void NormalizeTask::taskFunction() {
    dbg.info("NormalizeTask started.\n");

    for (;;) {
        auto rx = _inQueue.receive(portMAX_DELAY);
        if (!rx) continue;

        dbg.logWithType("TASK2 RX", COLOR_MAGENTA, "%ld, xxx\n", rx->timestamp);

        sharedDatatype::Attitude6DOF att = {};

        att.upper_pitch = rx->ctrllerData.angles[0];
        att.upper_roll  = rx->ctrllerData.angles[1];
        att.upper_yaw   = rx->ctrllerData.angles[2];

        att.rel_pitch = rx->ctrllerData.angles[3];
        att.rel_roll  = rx->ctrllerData.angles[4];
        att.rel_yaw   = rx->ctrllerData.angles[5];

        // TODO: 还未接收 jetson

        // TODO: 根据实际 ADC 量程校准
        att.grip_percent  = rx->ctrllerData.adc[0] / 3.3f;
        att.pitch_percent = rx->ctrllerData.adc[1] / 3.3f;

        att.timestamp = rx->timestamp;

        _outQueue.sendToBack(att, 0);
    }
}
