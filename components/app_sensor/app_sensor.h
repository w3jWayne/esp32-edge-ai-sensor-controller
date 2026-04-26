#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t sample_index;
    float temperature_c;
    float pressure_kpa;
} app_sensor_sample_t;

typedef enum {
    APP_SENSOR_MODE_SIMULATED = 0,
    APP_SENSOR_MODE_HTTP_QUEUE,
} app_sensor_mode_t;

/**
 * @brief Sensor instance state.
 *
 * Keeping the sample index in an instance makes the sensor module reentrant and
 * easier to test. Multiple simulated sensors, replay sources, or real hardware
 * drivers can each own their own state without hidden global data.
 */
typedef struct {
    uint32_t sample_index;
    app_sensor_mode_t mode;
} app_sensor_t;

void app_sensor_init(app_sensor_t *sensor, app_sensor_mode_t mode);
bool app_sensor_read_sample(app_sensor_t *sensor, app_sensor_sample_t *sample);
