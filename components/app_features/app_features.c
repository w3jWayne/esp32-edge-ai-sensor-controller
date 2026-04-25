#include <math.h>

#include "app_features.h"

bool feature_extract(const app_window_t *window, app_feature_vector_t *features)
{
    const app_sensor_sample_t *sample;
    float temperature_sum;
    float pressure_sum;
    float temperature_min;
    float temperature_max;
    float pressure_min;
    float pressure_max;
    float temperature_delta_sum;
    float pressure_delta_sum;
    float previous_temperature;
    float previous_pressure;
    size_t count;
    size_t i;

    if (window == NULL || features == NULL) {
        return false;
    }

    count = app_window_count(window);
    if (count < 2U) {
        return false;
    }

    sample = app_window_get(window, 0U);
    if (sample == NULL) {
        return false;
    }

    temperature_sum = 0.0f;
    pressure_sum = 0.0f;
    temperature_min = sample->temperature_c;
    temperature_max = sample->temperature_c;
    pressure_min = sample->pressure_kpa;
    pressure_max = sample->pressure_kpa;
    temperature_delta_sum = 0.0f;
    pressure_delta_sum = 0.0f;
    previous_temperature = sample->temperature_c;
    previous_pressure = sample->pressure_kpa;

    for (i = 0U; i < count; i++) {
        sample = app_window_get(window, i);
        if (sample == NULL) {
            return false;
        }

        temperature_sum += sample->temperature_c;
        pressure_sum += sample->pressure_kpa;

        if (sample->temperature_c < temperature_min) {
            temperature_min = sample->temperature_c;
        }
        if (sample->temperature_c > temperature_max) {
            temperature_max = sample->temperature_c;
        }
        if (sample->pressure_kpa < pressure_min) {
            pressure_min = sample->pressure_kpa;
        }
        if (sample->pressure_kpa > pressure_max) {
            pressure_max = sample->pressure_kpa;
        }

        if (i > 0U) {
            temperature_delta_sum += fabsf(sample->temperature_c - previous_temperature);
            pressure_delta_sum += fabsf(sample->pressure_kpa - previous_pressure);
        }

        previous_temperature = sample->temperature_c;
        previous_pressure = sample->pressure_kpa;
    }

    features->values[APP_FEATURE_INDEX_TEMP_MEAN] = temperature_sum / (float)count;
    features->values[APP_FEATURE_INDEX_TEMP_RANGE] = temperature_max - temperature_min;
    features->values[APP_FEATURE_INDEX_TEMP_MEAN_DELTA] =
        temperature_delta_sum / (float)(count - 1U);
    features->values[APP_FEATURE_INDEX_PRESSURE_MEAN] = pressure_sum / (float)count;
    features->values[APP_FEATURE_INDEX_PRESSURE_RANGE] = pressure_max - pressure_min;
    features->values[APP_FEATURE_INDEX_PRESSURE_MEAN_DELTA] =
        pressure_delta_sum / (float)(count - 1U);
    features->length = APP_FEATURE_COUNT;

    return true;
}
