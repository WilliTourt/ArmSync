#include "Blink.h"
#include "hal_data.h"

void BlinkTask::taskFunction() {
    bsp_io_level_t level = BSP_IO_LEVEL_LOW;
    for (;;) {
        level = (level == BSP_IO_LEVEL_HIGH) ? BSP_IO_LEVEL_LOW : BSP_IO_LEVEL_HIGH;
        R_IOPORT_PinWrite(&g_ioport_ctrl, LED_USER, level);
        this->delay(pdMS_TO_TICKS(500));
    }
}
