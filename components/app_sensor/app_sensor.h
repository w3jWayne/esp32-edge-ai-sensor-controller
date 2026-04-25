#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t sample_index;
    float temperature_c;
    float pressure_kpa;
} app_sensor_sample_t;

void app_sensor_init(void);
bool sensor_read_sample(app_sensor_sample_t *sample);
