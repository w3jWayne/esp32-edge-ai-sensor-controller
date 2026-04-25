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

void app_inference_init(app_inference_context_t *context);
bool inference_run(const app_inference_context_t *context,
                   const app_feature_vector_t *features,
                   app_inference_result_t *result);
