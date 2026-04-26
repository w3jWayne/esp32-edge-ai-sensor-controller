#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "app_window.h"

enum {
    APP_FEATURE_INDEX_TEMP_MEAN = 0,
    APP_FEATURE_INDEX_TEMP_RANGE,
    APP_FEATURE_INDEX_TEMP_MEAN_DELTA,
    APP_FEATURE_INDEX_PRESSURE_MEAN,
    APP_FEATURE_INDEX_PRESSURE_RANGE,
    APP_FEATURE_INDEX_PRESSURE_MEAN_DELTA,
    APP_FEATURE_COUNT,
};

typedef struct {
    float values[APP_FEATURE_COUNT];
    size_t length;
} app_feature_vector_t;

bool feature_extract(const app_window_t *window, app_feature_vector_t *features);
