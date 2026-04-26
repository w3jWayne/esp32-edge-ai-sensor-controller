#include <math.h>

#include "app_features.h"

/**
 * @brief Extract feature vector from a sliding window of sensor samples
 *
 * This function computes statistical and temporal features from a window of
 * temperature and pressure data. The extracted features are designed to help
 * a lightweight ML model detect anomalies in the signal.
 *
 * Features include:
 * - Mean value (overall signal level)
 * - Range (min/max spread → extreme conditions)
 * - Mean delta (average change between samples → signal stability / fluctuation)
 *
 * @param window    Pointer to sensor data window (time-series buffer)
 * @param features  Output feature vector
 *
 * @return true if feature extraction is successful, false otherwise
 */

bool feature_extract(const app_window_t *window, app_feature_vector_t *features)
{
    const app_sensor_sample_t *sample;

    // Accumulators for statistical features
    float temperature_sum;
    float pressure_sum;

    // Track min/max values (used to compute range)
    float temperature_min;
    float temperature_max;
    float pressure_min;
    float pressure_max;

    // Accumulators for temporal features (signal change)
    float temperature_delta_sum;
    float pressure_delta_sum;

    // Previous sample values (used to compute delta)
    float previous_temperature;
    float previous_pressure;

    size_t count;
    size_t i;

    if (window == NULL || features == NULL) {
        return false;
    }

    // Get number of samples in the window
    // Need at least 2 samples to compute delta features
    count = app_window_count(window);
    if (count < 2U) {
        return false;
    }

    // Initialize using the first sample
    sample = app_window_get(window, 0U);
    if (sample == NULL) {
        return false;
    }

    // Initialize accumulators
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

        // Update min/max (for range feature)
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

        // Compute temporal change (delta between consecutive samples)
        if (i > 0U) {
            temperature_delta_sum += fabsf(sample->temperature_c - previous_temperature);
            pressure_delta_sum += fabsf(sample->pressure_kpa - previous_pressure);
        }

        // Update previous sample for next iteration
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
