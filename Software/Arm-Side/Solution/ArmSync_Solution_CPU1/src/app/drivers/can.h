#ifndef __CAN_H
#define __CAN_H

#include <stdbool.h>
#include <stdint.h>

#include "r_can_api.h"

#define __IO volatile

/* CAN adapter for Renesas RA FSP (CANFD driver).
 * Keeps the original high-level API: `can_SendCmd()` packs a command into classic CAN frames.
 */

typedef struct
{
    __IO can_frame_t CAN_RxMsg;
    __IO can_frame_t CAN_TxMsg;
    __IO uint8_t     rxData[CAN_DATA_BUFFER_LENGTH];
    __IO uint8_t     rxDlc;
    __IO uint32_t    rxExtId;
    __IO uint8_t     motorAddr;
    __IO uint32_t    motorCurPosRaw;
    __IO float       motorCurPosDeg;

    __IO bool rxFrameFlag;
    __IO bool motorCurPosUpdated;
} CAN_t;

void can_Init(void);
void can_SendCmd(__IO uint8_t * cmd, uint8_t len);

extern __IO CAN_t can;

#endif
