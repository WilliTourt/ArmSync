#ifndef MODEL_TEST_DATA_H
#define MODEL_TEST_DATA_H

#include <stdint.h>

#define MODEL_TEST_CASE_COUNT 8
#define MODEL_TEST_FEATURE_COUNT 192

extern const float model_test_features[MODEL_TEST_CASE_COUNT][MODEL_TEST_FEATURE_COUNT];
extern const float model_test_ref_deg[MODEL_TEST_CASE_COUNT][2];
extern const uint16_t model_test_source_index[MODEL_TEST_CASE_COUNT];

#endif
