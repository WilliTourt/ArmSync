#include "tasks.h"

BlinkTask blinkTask;

UartRecvTask uartRecvTask(originalDataQueue);
NormalizeTask normalizeTask(originalDataQueue, normalizedDataQueue);
