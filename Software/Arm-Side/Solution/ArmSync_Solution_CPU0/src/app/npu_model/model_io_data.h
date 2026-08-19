#ifndef MODEL_IO_DATA_H
#define MODEL_IO_DATA_H

#include <stddef.h>

/* Representative standardized [1, 1, 32, 6] calibration window. */
extern const float model_features[192];
#define model_features_SIZE (sizeof(model_features))

/* Quantized Ethos hard-QAT reference for this exact window (degrees). */
extern const float model_j3_deg_70055[1];
#define model_j3_deg_70055_COUNT \
    (sizeof(model_j3_deg_70055) / sizeof(model_j3_deg_70055[0]))
extern const float model_j5_deg_70056[1];
#define model_j5_deg_70056_COUNT \
    (sizeof(model_j5_deg_70056) / sizeof(model_j5_deg_70056[0]))

#endif /* MODEL_IO_DATA_H */
