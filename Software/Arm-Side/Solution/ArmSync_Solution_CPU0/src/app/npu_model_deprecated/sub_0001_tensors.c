#include "sub_0001_tensors.h"

const TensorInfo sub_0001_tensors[] = {
  { "_split_1_command_stream", 0, 1344, "COMMAND_STREAM", 0xffffffff },
  { "_split_1_flash", 1, 17856, "MODEL", 0xffffffff },
  { "_split_1_scratch", 2, 1536, "ARENA", 0x0 },
  { "_split_1_scratch_fast", 3, 1536, "FAST_SCRATCH", 0x0 },
  { "features_70085_10146_70034", 4, 192, "INPUT_TENSOR", 0x0 },
  { "j5_norm_70055_10117", 6, 1, "OUTPUT_TENSOR", 0x0 },
  { "j3_norm_70056_10112", 5, 1, "OUTPUT_TENSOR", 0x20 },
};

const size_t sub_0001_tensors_count = sizeof(sub_0001_tensors) / sizeof(sub_0001_tensors[0]);

// Addresses for each input and output buffer inside of the arena
const uint32_t sub_0001_address_features_70085_10146_70034 = 0x0;
const uint32_t sub_0001_address_j5_norm_70055_10117 = 0x0;
const uint32_t sub_0001_address_j3_norm_70056_10112 = 0x20;

