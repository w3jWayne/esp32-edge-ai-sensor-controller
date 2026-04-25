#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "app_sensor.h"

#define APP_WINDOW_CAPACITY 16U

typedef struct {
    app_sensor_sample_t samples[APP_WINDOW_CAPACITY];
    size_t next_index;
    size_t count;
} app_window_t;

void app_window_init(app_window_t *window);
bool window_push(app_window_t *window, const app_sensor_sample_t *sample);
bool app_window_is_full(const app_window_t *window);
size_t app_window_count(const app_window_t *window);
const app_sensor_sample_t *app_window_get(const app_window_t *window, size_t ordered_index);
