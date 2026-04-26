#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "app_features.h"

typedef struct {
    const unsigned char *model_data;
    void *tensor_arena;
    size_t tensor_arena_size;
} app_inference_context_t;

typedef struct {
    float anomaly_score;
    bool is_anomaly;
    const char *backend_name;
} app_inference_result_t;

/**
 * @brief Initialize the inference backend.
 *
 * The backend implementation is intentionally hidden inside app_inference.c.
 * Callers do not need to know whether inference is implemented by a rule stub,
 * logistic regression, or TensorFlow Lite Micro.
 */
void app_inference_init(void);

/**
 * @brief Run inference using the current backend.
 *
 * @param features  Input feature vector from feature extraction stage.
 * @param result    Output anomaly score and classification.
 *
 * @return true if inference succeeds, false otherwise.
 */
bool inference_run(const app_feature_vector_t *features,
                   app_inference_result_t *result);
