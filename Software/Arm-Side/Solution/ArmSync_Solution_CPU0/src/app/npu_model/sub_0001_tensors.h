#ifndef __SUB_0001_TENSORS_H__
#define __SUB_0001_TENSORS_H__

#include <stddef.h>
#include <stdint.h>
#include "ethosu_common.h"

extern const TensorInfo sub_0001_tensors[];
extern const size_t sub_0001_tensors_count;

#define kArenaSize_sub_0001 96

// Addresses for each input and output buffer inside of the arena
extern const uint32_t sub_0001_address_window_deg_70007_10024_70010;
extern const uint32_t sub_0001_address_delta_deg_70003_10016;


#endif // __SUB_0001_TENSORS_H__
