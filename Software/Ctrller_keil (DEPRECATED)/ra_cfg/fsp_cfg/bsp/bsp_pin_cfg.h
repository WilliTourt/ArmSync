/* generated configuration header file - do not edit */
#ifndef BSP_PIN_CFG_H_
#define BSP_PIN_CFG_H_
#include "r_ioport.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

#define Trigger (BSP_IO_PORT_00_PIN_02)
#define ANALOG_GRIP_VR (BSP_IO_PORT_00_PIN_14)
#define QMC5883P_SCL (BSP_IO_PORT_01_PIN_00)
#define QMC5883P_SDA (BSP_IO_PORT_01_PIN_01)
#define ANALOG_BAT (BSP_IO_PORT_01_PIN_02)
#define VIBRATE (BSP_IO_PORT_01_PIN_03)
#define ICM42688P_INT1 (BSP_IO_PORT_01_PIN_04)
#define KEY1 (BSP_IO_PORT_01_PIN_05)
#define BUINTIN_LED (BSP_IO_PORT_01_PIN_06)
#define LED1 (BSP_IO_PORT_01_PIN_07)
#define ICM42688P_SDI (BSP_IO_PORT_01_PIN_09)
#define ICM42688P_SDO (BSP_IO_PORT_01_PIN_10)
#define ICM42688P_SCK (BSP_IO_PORT_01_PIN_11)
#define ICM42688P_CS (BSP_IO_PORT_01_PIN_12)
#define HC06_TX (BSP_IO_PORT_03_PIN_01)
#define HC06_RX (BSP_IO_PORT_03_PIN_02)
#define LED2 (BSP_IO_PORT_03_PIN_03)
#define KEY2 (BSP_IO_PORT_03_PIN_04)
#define DEBUG_TX (BSP_IO_PORT_05_PIN_01)
#define DEBUG_RX (BSP_IO_PORT_05_PIN_02)

extern const ioport_cfg_t g_bsp_pin_cfg; /* R7FA4M1AB3CFM.pincfg */

void BSP_PinConfigSecurityInit();

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER
#endif /* BSP_PIN_CFG_H_ */
