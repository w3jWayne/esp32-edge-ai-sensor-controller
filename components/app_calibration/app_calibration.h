#pragma once

#include <stdbool.h>

#include "app_sensor.h"

typedef struct {
    float temperature_offset_c;
    float pressure_offset_kpa;
    float temperature_scale;
    float pressure_scale;
} app_calibration_t;

void app_calibration_init(app_calibration_t *calibration);
bool calibration_apply(const app_calibration_t *calibration,
                       const app_sensor_sample_t *raw_sample,
                       app_sensor_sample_t *calibrated_sample);
