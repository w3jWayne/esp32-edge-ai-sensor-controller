#include "app_inference.h"
#include "app_config.h"

#include <math.h>

/*
 * Logistic regression model parameters.
 *
 * NOTE:
 * These values are placeholder parameters for the MVP AI inference backend.
 * In the real AI workflow, they should be generated offline from labeled data,
 * for example by a Python training script using scikit-learn.
 *
 * Expected feature order must match app_feature_index_t / APP_FEATURE_INDEX_*:
 *   0: temperature mean
 *   1: temperature range
 *   2: temperature mean delta
 *   3: pressure mean
 *   4: pressure range
 *   5: pressure mean delta
 */
static const float s_model_weights[APP_FEATURE_COUNT] = {
    0.20f,  /* APP_FEATURE_INDEX_TEMP_MEAN */
    1.80f,  /* APP_FEATURE_INDEX_TEMP_RANGE */
    1.20f,  /* APP_FEATURE_INDEX_TEMP_MEAN_DELTA */
   -0.15f,  /* APP_FEATURE_INDEX_PRESSURE_MEAN */
    1.60f,  /* APP_FEATURE_INDEX_PRESSURE_RANGE */
    1.10f   /* APP_FEATURE_INDEX_PRESSURE_MEAN_DELTA */
};

/* Model bias/intercept. This is also learned during offline training. */
static const float s_model_bias = -2.20f;

/*
 * Feature normalization parameters.
 *
 * Logistic regression works best when features are on comparable scales.
 * These mean/std values should be calculated from the training dataset and
 * exported together with the weights.
 *
 * For now, they are reasonable placeholder values based on expected sensor
 * operating conditions. Replace them after collecting real data.
 */
static const float s_feature_mean[APP_FEATURE_COUNT] = {
    24.65f,  /* temperature mean baseline (deg C) */
     1.00f,  /* temperature range baseline */
     0.20f,  /* temperature mean delta baseline */
   101.60f,  /* pressure mean baseline (kPa) */
     0.80f,  /* pressure range baseline */
     0.15f   /* pressure mean delta baseline */
};

static const float s_feature_std[APP_FEATURE_COUNT] = {
    2.00f,   /* temperature mean scale */
    1.50f,   /* temperature range scale */
    0.40f,   /* temperature mean delta scale */
    2.00f,   /* pressure mean scale */
    1.30f,   /* pressure range scale */
    0.35f    /* pressure mean delta scale */
};

static float app_inference_sigmoid(float x)
{
    /*
     * Prevent expf() overflow for very large positive/negative values.
     * This keeps inference numerically stable on embedded targets.
     */
    if (x >= 20.0f) {
        return 1.0f;
    }
    if (x <= -20.0f) {
        return 0.0f;
    }

    return 1.0f / (1.0f + expf(-x));
}

static float app_inference_normalize_feature(float value, float mean, float std)
{
    if (std <= 0.0f) {
        return 0.0f;
    }

    return (value - mean) / std;
}

void app_inference_init(app_inference_context_t *context)
{
    if (context == NULL) {
        return;
    }

    /*
     * Keep these fields for future backends such as TensorFlow Lite Micro.
     * The current logistic regression backend uses static C arrays above.
     */
    context->model_data = NULL;
    context->tensor_arena = NULL;
    context->tensor_arena_size = 0U;
}

/**
 * @brief Run AI inference on extracted feature vector
 *
 * This function implements a lightweight logistic regression inference backend.
 * Unlike the earlier MVP rule-based weighted score, the model structure here is
 * the same as a trained linear classifier:
 *
 *   z = bias + w1*x1 + w2*x2 + ... + wn*xn
 *   anomaly_probability = sigmoid(z)
 *
 * In a production TinyAI/TinyML flow, the weights, bias, and normalization
 * parameters are not guessed manually. They are learned offline from labeled
 * data using Python training, then exported as static C constants for ESP32.
 *
 * Typical workflow:
 * - ESP32 logs feature vectors and labels: normal / anomaly
 * - Python trains a logistic regression model
 * - Training exports feature mean/std, weights, bias, and decision threshold
 * - ESP32 runs only inference using those fixed trained parameters
 *
 * This keeps runtime cost very low while making the decision data-driven.
 *
 * @param context   Inference context (reserved for future model/config use)
 * @param features  Input feature vector from feature extraction stage
 * @param result    Output inference result (probability + classification)
 *
 * @return true if inference succeeds, false otherwise
 */
bool inference_run(const app_inference_context_t *context,
                   const app_feature_vector_t *features,
                   app_inference_result_t *result)
{
    float z;
    size_t i;

    if (context == NULL || features == NULL || result == NULL ||
        features->length != APP_FEATURE_COUNT) {
        return false;
    }

    /*
     * The current implementation uses compile-time model parameters.
     * Keep context validation so the API remains compatible with future
     * runtime-loaded model backends.
     */
    (void)context;

    z = s_model_bias;

    for (i = 0U; i < APP_FEATURE_COUNT; i++) {
        float normalized_feature;

        /*
         * Normalize each feature using training-set statistics.
         * This allows weights to operate on comparable feature scales.
         */
        normalized_feature = app_inference_normalize_feature(
            features->values[i], s_feature_mean[i], s_feature_std[i]);

        /*
         * Accumulate linear model output.
         * Larger learned weights mean that feature has stronger influence.
         */
        z += s_model_weights[i] * normalized_feature;
    }

    /* Convert raw model output into a probability-like anomaly score [0, 1]. */
    result->anomaly_score = app_inference_sigmoid(z);

    /* Final classification threshold, usually tuned using validation data. */
    result->is_anomaly = (result->anomaly_score >= APP_INFER_ANOMALY_THRESHOLD);

    result->backend_name = "logistic_regression";

    return true;
}
