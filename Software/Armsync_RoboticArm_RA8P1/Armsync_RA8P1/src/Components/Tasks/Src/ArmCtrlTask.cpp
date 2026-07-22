#include "ArmCtrlTask.h"
#include "ElegantDebug.h"

extern ElegantDebug dbg;

void ArmCtrlTask::taskFunction() {
    dbg.info("ArmCtrlTask started.");

    for (;;) {
        auto data = _queue.receive(portMAX_DELAY);
        if (data) {
            // dbg.success("Angles(mrad): %d %d %d | %d %d %d | ADC: %d %d",
            //     (int)(data->angles[0]*1000),
            //     (int)(data->angles[1]*1000),
            //     (int)(data->angles[2]*1000),
            //     (int)(data->angles[3]*1000),
            //     (int)(data->angles[4]*1000),
            //     (int)(data->angles[5]*1000),
            //     (int)(data->adc[0]*100),
            //     (int)(data->adc[1]*100));
            // this->delayUntil(pdMS_TO_TICKS(30));
        }
    }
}
