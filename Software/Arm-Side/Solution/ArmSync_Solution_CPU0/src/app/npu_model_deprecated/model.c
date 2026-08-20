/*
 * This file is developed by EdgeCortix Inc. to be used with certain Renesas Electronics Hardware only.
 *
 * Copyright © 2025 EdgeCortix Inc. Licensed to Renesas Electronics Corporation with the
 * right to sublicense under the Apache License, Version 2.0.
 *
 * This file also includes source code originally developed by the Renesas Electronics Corporation.
 * The Renesas disclaimer below applies to any Renesas-originated portions for usage of the code.
 *
 * The Renesas Electronics Corporation
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED 'AS IS' AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Changed from original python code to C source code.
 * Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
 *
 * This file also includes source codes originally developed by the TensorFlow Authors which were distributed under the following conditions.
 *
 * The TensorFlow Authors
 * Copyright 2023 The Apache Software Foundation
 *
 * This product includes software developed at
 * The Apache Software Foundation (http://www.apache.org/).
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "model.h"

// CPU compute declarations
#include "compute_sub_0000.h"
#include "sub_0001_invoke.h"
#include "compute_sub_0002.h"

// Buffers for CPU units
float buf_features[192];
int8_t buf_features_70085_10146_70034[192];
float buf_j3_norm_70055[1];
float buf_j5_norm_70056[1];
float buf_j3_deg_70055[1];
float buf_j5_deg_70056[1];

// Arenas for CPU units
uint8_t compute_arena_sub_0000[kBufferSize_sub_0000];
uint8_t compute_arena_sub_0002[kBufferSize_sub_0002];

  // Model input pointers
float* GetModelInputPtr_features() {
  return buf_features;
}


  // Model output pointers
float* GetModelOutputPtr_j3_norm_70055() {
  return buf_j3_norm_70055;
}

float* GetModelOutputPtr_j5_norm_70056() {
  return buf_j5_norm_70056;
}

int8_t* GetModelRawOutputPtr_j3_norm_70055() {
  /* The generated candidate names its J3 head j3_norm_70056.  Keep the
   * application-facing accessor stable while mapping to the new tensor. */
  return (int8_t*) (sub_0001_arena + sub_0001_address_j3_norm_70056_10112);
}

int8_t* GetModelRawOutputPtr_j5_norm_70056() {
  return (int8_t*) (sub_0001_arena + sub_0001_address_j5_norm_70055_10117);
}

float* GetModelOutputPtr_j3_deg_70055() {
  return buf_j3_deg_70055;
}

float* GetModelOutputPtr_j5_deg_70056() {
  return buf_j5_deg_70056;
}


int RunModel(bool clean_outputs) {
  // Buffers for NPU units
  int8_t* buf_j3_norm_70056_10112 = (int8_t*) (sub_0001_arena + sub_0001_address_j3_norm_70056_10112);
  int8_t* buf_j5_norm_70055_10117 = (int8_t*) (sub_0001_arena + sub_0001_address_j5_norm_70055_10117);

  // CPU Unit
  compute_sub_0000(compute_arena_sub_0000, buf_features, buf_features_70085_10146_70034  );

  memcpy((sub_0001_arena + sub_0001_address_features_70085_10146_70034), buf_features_70085_10146_70034, 192);
  // NPU Unit
  int invoke_status = sub_0001_invoke(clean_outputs);

  // CPU Unit
  compute_sub_0002(compute_arena_sub_0002, buf_j3_norm_70056_10112, buf_j5_norm_70055_10117, buf_j3_norm_70055, buf_j5_norm_70056  );

  // The normalized ONNX heads represent degrees / 180.  Keep the generated
  // normalized buffers intact and expose degree-valued outputs to the rest of
  // the application.
  buf_j3_deg_70055[0] = buf_j3_norm_70055[0] * 180.0f;
  buf_j5_deg_70056[0] = buf_j5_norm_70056[0] * 180.0f;

  return invoke_status;

}
