/* generated configuration header file - do not edit */
#ifndef BSP_PIN_CFG_H_
#define BSP_PIN_CFG_H_
#include "r_ioport.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

#define ICM42688P_IRQ (BSP_IO_PORT_01_PIN_04)
#define LED1 (BSP_IO_PORT_01_PIN_07)
#define ICM42688P_CS (BSP_IO_PORT_01_PIN_12)

extern const ioport_cfg_t g_bsp_pin_cfg; /* R7FA4M1AB3CFM.pincfg */

void BSP_PinConfigSecurityInit();

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER
#endif /* BSP_PIN_CFG_H_ */
