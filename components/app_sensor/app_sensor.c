#include <stddef.h>

#include "app_sensor.h"

#define APP_SENSOR_PATTERN_LENGTH 8U
#define APP_SENSOR_ANOMALY_PERIOD 48U
#define APP_SENSOR_ANOMALY_LENGTH 6U

static const float s_temperature_pattern[APP_SENSOR_PATTERN_LENGTH] = {
    0.00f, 0.15f, -0.10f, 0.20f, -0.05f, 0.10f, -0.15f, 0.05f,
};

static const float s_pressure_pattern[APP_SENSOR_PATTERN_LENGTH] = {
    0.00f, -0.10f, 0.15f, -0.05f, 0.10f, -0.15f, 0.05f, -0.05f,
};

static bool app_sensor_read_simulated_sample(app_sensor_t *sensor,
                                             app_sensor_sample_t *sample)
{
    if (sensor == NULL) {
        return false;
    }

    uint32_t anomaly_phase;
    uint32_t anomaly_offset;
    uint32_t pattern_index;
    pattern_index = sensor->sample_index % APP_SENSOR_PATTERN_LENGTH;

    sample->sample_index = sensor->sample_index;
    sample->temperature_c = 24.8f + s_temperature_pattern[pattern_index];
    sample->pressure_kpa = 101.2f + s_pressure_pattern[pattern_index];

    /*
     * Simulated anomaly injection:
     * Every 48 samples, the last 6 samples contain increasing temperature rise
     * and pressure drop. This is only for MVP testing until real sensor input
     * or replay data is available.
     */
    anomaly_phase = sensor->sample_index % APP_SENSOR_ANOMALY_PERIOD;
    if (anomaly_phase >= (APP_SENSOR_ANOMALY_PERIOD - APP_SENSOR_ANOMALY_LENGTH)) {
        anomaly_offset = anomaly_phase - (APP_SENSOR_ANOMALY_PERIOD - APP_SENSOR_ANOMALY_LENGTH);
        sample->temperature_c += 3.5f + (0.4f * (float)anomaly_offset);
        sample->pressure_kpa -= 2.8f + (0.3f * (float)anomaly_offset);
    }

    sensor->sample_index++;
    return true;
}

void app_sensor_init(app_sensor_t *sensor, app_sensor_mode_t mode)
{
    if (sensor == NULL) {
        return;
    }

    sensor->sample_index = 0U;
    sensor->mode = mode;
}

bool app_sensor_read_sample(app_sensor_t *sensor, app_sensor_sample_t *sample)
{
    if (sensor == NULL || sample == NULL) {
        return false;
    }

    if (sensor->mode != APP_SENSOR_MODE_SIMULATED) {
        return false;
    }

    return app_sensor_read_simulated_sample(sensor, sample);
}
