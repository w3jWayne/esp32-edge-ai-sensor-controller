#include <stddef.h>

#include "app_sensor.h"

#define APP_SENSOR_PATTERN_LENGTH 8U
#define APP_SENSOR_ANOMALY_PERIOD 48U
#define APP_SENSOR_ANOMALY_LENGTH 6U

static uint32_t s_sample_index = 0;

static const float s_temperature_pattern[APP_SENSOR_PATTERN_LENGTH] = {
    0.00f, 0.15f, -0.10f, 0.20f, -0.05f, 0.10f, -0.15f, 0.05f,
};

static const float s_pressure_pattern[APP_SENSOR_PATTERN_LENGTH] = {
    0.00f, -0.10f, 0.15f, -0.05f, 0.10f, -0.15f, 0.05f, -0.05f,
};

void app_sensor_init(void)
{
    s_sample_index = 0;
}

bool sensor_read_sample(app_sensor_sample_t *sample)
{
    uint32_t anomaly_phase;
    uint32_t anomaly_offset;
    uint32_t pattern_index;

    if (sample == NULL) {
        return false;
    }

    pattern_index = s_sample_index % APP_SENSOR_PATTERN_LENGTH;
    sample->sample_index = s_sample_index;
    sample->temperature_c = 24.8f + s_temperature_pattern[pattern_index];   // Temperature varies: 24.65°C to 25.00°C (±0.2°C oscillation)
    sample->pressure_kpa = 101.2f + s_pressure_pattern[pattern_index];      // Pressure varies: 100.95 to 101.45 kPa (±0.15 kPa oscillation)

    // Every 48 samples, the last 6 samples (samples 42-47 of each cycle) contain anomalies
    anomaly_phase = s_sample_index % APP_SENSOR_ANOMALY_PERIOD;
    if (anomaly_phase >= (APP_SENSOR_ANOMALY_PERIOD - APP_SENSOR_ANOMALY_LENGTH)) {
        anomaly_offset = anomaly_phase - (APP_SENSOR_ANOMALY_PERIOD - APP_SENSOR_ANOMALY_LENGTH);
        sample->temperature_c += 3.5f + (0.4f * (float)anomaly_offset);
        sample->pressure_kpa -= 2.8f + (0.3f * (float)anomaly_offset);
    }

    s_sample_index++;
    return true;
}
