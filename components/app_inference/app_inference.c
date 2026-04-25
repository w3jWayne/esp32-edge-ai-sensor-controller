#include "app_inference.h"

static float app_inference_clamp01(float value)
{
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }

    return value;
}

static float app_inference_ratio(float value, float threshold)
{
    if (threshold <= 0.0f) {
        return 0.0f;
    }

    return app_inference_clamp01(value / threshold);
}

void app_inference_init(app_inference_context_t *context)
{
    if (context == NULL) {
        return;
    }

    context->model_data = NULL;
    context->tensor_arena = NULL;
    context->tensor_arena_size = 0U;
}

bool inference_run(const app_inference_context_t *context,
                   const app_feature_vector_t *features,
                   app_inference_result_t *result)
{
    float score;
    float temp_range_score;
    float temp_delta_score;
    float pressure_range_score;
    float pressure_delta_score;
    float mean_bias_score;

    if (context == NULL || features == NULL || result == NULL ||
        features->length != APP_FEATURE_COUNT) {
        return false;
    }

    temp_range_score = app_inference_ratio(
        features->values[APP_FEATURE_INDEX_TEMP_RANGE], 4.5f);
    temp_delta_score = app_inference_ratio(
        features->values[APP_FEATURE_INDEX_TEMP_MEAN_DELTA], 0.9f);
    pressure_range_score = app_inference_ratio(
        features->values[APP_FEATURE_INDEX_PRESSURE_RANGE], 3.8f);
    pressure_delta_score = app_inference_ratio(
        features->values[APP_FEATURE_INDEX_PRESSURE_MEAN_DELTA], 0.7f);
    mean_bias_score = app_inference_ratio(
        (features->values[APP_FEATURE_INDEX_TEMP_MEAN] - 24.65f) +
        (101.60f - features->values[APP_FEATURE_INDEX_PRESSURE_MEAN]),
        4.0f);

    score = (0.30f * temp_range_score) +
            (0.20f * temp_delta_score) +
            (0.25f * pressure_range_score) +
            (0.15f * pressure_delta_score) +
            (0.10f * mean_bias_score);

    result->anomaly_score = app_inference_clamp01(score);
    result->is_anomaly =
        (result->anomaly_score >= 0.60f) ||
        (features->values[APP_FEATURE_INDEX_TEMP_RANGE] >= 5.0f) ||
        (features->values[APP_FEATURE_INDEX_PRESSURE_RANGE] >= 4.0f);
    result->backend_name = "rule_stub";

    return true;
}
