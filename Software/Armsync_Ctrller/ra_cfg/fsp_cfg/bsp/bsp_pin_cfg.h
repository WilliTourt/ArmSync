/* generated configuration header file - do not edit */
#ifndef BSP_PIN_CFG_H_
#define BSP_PIN_CFG_H_
#include "r_ioport.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

#define Grip_VR_2 (BSP_IO_PORT_00_PIN_00)
#define Grip_VR_1 (BSP_IO_PORT_00_PIN_14)
#define BUZZER (BSP_IO_PORT_01_PIN_04)
#define KEY (BSP_IO_PORT_01_PIN_05)
#define Builtin_LED (BSP_IO_PORT_01_PIN_06)
#define LED1 (BSP_IO_PORT_01_PIN_07)
#define LED2 (BSP_IO_PORT_03_PIN_03)

extern const ioport_cfg_t g_bsp_pin_cfg; /* R7FA4M1AB3CFM.pincfg */

void BSP_PinConfigSecurityInit();

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER
#endif /* BSP_PIN_CFG_H_ */
