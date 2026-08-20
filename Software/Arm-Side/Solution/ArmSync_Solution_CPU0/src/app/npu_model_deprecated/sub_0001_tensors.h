#ifndef __SUB_0001_TENSORS_H__
#define __SUB_0001_TENSORS_H__

#include <stddef.h>
#include <stdint.h>
#include "ethosu_common.h"

extern const TensorInfo sub_0001_tensors[];
extern const size_t sub_0001_tensors_count;

#define kArenaSize_sub_0001 1536

// Addresses for each input and output buffer inside of the arena
extern const uint32_t sub_0001_address_features_70085_10146_70034;
extern const uint32_t sub_0001_address_j5_norm_70055_10117;
extern const uint32_t sub_0001_address_j3_norm_70056_10112;


#endif // __SUB_0001_TENSORS_H__
