#include "sub_0001_tensors.h"

const TensorInfo sub_0001_tensors[] = {
  { "_split_1_command_stream", 0, 500, "COMMAND_STREAM", 0xffffffff },
  { "_split_1_flash", 1, 496, "MODEL", 0xffffffff },
  { "_split_1_scratch", 2, 96, "ARENA", 0x0 },
  { "_split_1_scratch_fast", 3, 96, "FAST_SCRATCH", 0x0 },
  { "window_deg_70007_10024_70010", 5, 45, "INPUT_TENSOR", 0x0 },
  { "delta_deg_70003_10016", 4, 5, "OUTPUT_TENSOR", 0x10 },
};

const size_t sub_0001_tensors_count = sizeof(sub_0001_tensors) / sizeof(sub_0001_tensors[0]);

// Addresses for each input and output buffer inside of the arena
const uint32_t sub_0001_address_window_deg_70007_10024_70010 = 0x0;
const uint32_t sub_0001_address_delta_deg_70003_10016 = 0x10;

