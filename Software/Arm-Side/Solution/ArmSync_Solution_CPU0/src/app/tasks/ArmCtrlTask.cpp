#include "ArmCtrlTask.h"

void ArmCtrlTask::taskFunction() {
    for (;;) {
        // TODO: get controller data from queue, compute arm control
        this->delay(pdMS_TO_TICKS(10));
    }
}
