#include <math.h>
#include <stdio.h>

#include "app_decision.h"
#include "app_features.h"
#include "app_inference.h"
#include "app_window.h"

#define APP_TEST_FLOAT_EPSILON    0.001f

static unsigned int s_test_total;
static unsigned int s_test_failed;

static void app_test_assert_impl(bool condition, const char *message)
{
    s_test_total++;

    if (!condition) {
        s_test_failed++;
        printf("FAIL: %s\n", message);
    } else {
        printf("PASS: %s\n", message);
    }
}

#define TEST_ASSERT(cond, msg) \
    app_test_assert_impl((cond), (msg))

static bool app_test_float_equal(float actual, float expected)
{
    return fabsf(actual - expected) <= APP_TEST_FLOAT_EPSILON;
}

static void app_test_fill_window(app_window_t *window,
                                 const app_sensor_sample_t *samples,
                                 size_t sample_count)
{
    size_t i;

    app_window_init(window);

    for (i = 0U; i < sample_count; i++) {
        window_push(window, &samples[i]);
    }
}

static void test_feature_extract_basic(void)
{
    static const app_sensor_sample_t samples[] = {
        { .sample_index = 0U, .temperature_c = 25.0f, .pressure_kpa = 100.0f },
        { .sample_index = 1U, .temperature_c = 25.0f, .pressure_kpa = 100.0f },
        { .sample_index = 2U, .temperature_c = 25.0f, .pressure_kpa = 100.0f },
        { .sample_index = 3U, .temperature_c = 25.0f, .pressure_kpa = 100.0f },
    };
    app_window_t window;
    app_feature_vector_t features = { 0 };
    bool ok;

    printf("\n[test_feature_extract_basic]\n");

    app_test_fill_window(&window, samples, sizeof(samples) / sizeof(samples[0]));
    ok = feature_extract(&window, &features);

    printf("temp_mean=%.3f temp_delta=%.3f pressure_mean=%.3f pressure_delta=%.3f\n",
           features.values[APP_FEATURE_INDEX_TEMP_MEAN],
           features.values[APP_FEATURE_INDEX_TEMP_MEAN_DELTA],
           features.values[APP_FEATURE_INDEX_PRESSURE_MEAN],
           features.values[APP_FEATURE_INDEX_PRESSURE_MEAN_DELTA]);

    TEST_ASSERT(ok, "feature_extract succeeds for constant data");
    TEST_ASSERT(features.length == APP_FEATURE_COUNT, "feature vector length matches");
    TEST_ASSERT(app_test_float_equal(features.values[APP_FEATURE_INDEX_TEMP_MEAN], 25.0f),
                "constant temperature mean is correct");
    TEST_ASSERT(app_test_float_equal(features.values[APP_FEATURE_INDEX_TEMP_MEAN_DELTA], 0.0f),
                "constant temperature delta is zero");
    TEST_ASSERT(app_test_float_equal(features.values[APP_FEATURE_INDEX_PRESSURE_MEAN], 100.0f),
                "constant pressure mean is correct");
    TEST_ASSERT(app_test_float_equal(features.values[APP_FEATURE_INDEX_PRESSURE_MEAN_DELTA], 0.0f),
                "constant pressure delta is zero");
}

static void test_feature_extract_increasing(void)
{
    static const app_sensor_sample_t samples[] = {
        { .sample_index = 0U, .temperature_c = 20.0f, .pressure_kpa = 100.0f },
        { .sample_index = 1U, .temperature_c = 21.0f, .pressure_kpa = 100.5f },
        { .sample_index = 2U, .temperature_c = 22.0f, .pressure_kpa = 101.0f },
        { .sample_index = 3U, .temperature_c = 23.0f, .pressure_kpa = 101.5f },
    };
    app_window_t window;
    app_feature_vector_t features = { 0 };
    bool ok;

    printf("\n[test_feature_extract_increasing]\n");

    app_test_fill_window(&window, samples, sizeof(samples) / sizeof(samples[0]));
    ok = feature_extract(&window, &features);

    printf("temp_range=%.3f temp_delta=%.3f pressure_range=%.3f pressure_delta=%.3f\n",
           features.values[APP_FEATURE_INDEX_TEMP_RANGE],
           features.values[APP_FEATURE_INDEX_TEMP_MEAN_DELTA],
           features.values[APP_FEATURE_INDEX_PRESSURE_RANGE],
           features.values[APP_FEATURE_INDEX_PRESSURE_MEAN_DELTA]);

    TEST_ASSERT(ok, "feature_extract succeeds for increasing data");
    TEST_ASSERT(features.values[APP_FEATURE_INDEX_TEMP_MEAN_DELTA] > 0.0f,
                "increasing temperature data has positive delta");
    TEST_ASSERT(features.values[APP_FEATURE_INDEX_PRESSURE_MEAN_DELTA] > 0.0f,
                "increasing pressure data has positive delta");
}

static void test_inference_run_low(void)
{
    app_feature_vector_t features = {
        .values = {
            24.65f,
            1.00f,
            0.20f,
            101.60f,
            0.80f,
            0.15f,
        },
        .length = APP_FEATURE_COUNT,
    };
    app_inference_result_t result = { 0 };
    const char *backend_name;
    bool ok;

    printf("\n[test_inference_run_low]\n");

    app_inference_init();
    ok = inference_run(&features, &result);
    backend_name = (result.backend_name != NULL) ? result.backend_name : "null";

    printf("backend=%s score=%.3f is_anomaly=%d\n",
           backend_name,
           result.anomaly_score,
           result.is_anomaly);

    TEST_ASSERT(ok, "inference_run succeeds for low feature values");
    TEST_ASSERT(result.anomaly_score < 0.50f, "low feature values produce low anomaly score");
    TEST_ASSERT(!result.is_anomaly, "low feature values are not classified as anomaly");
}

static void test_inference_run_high(void)
{
    app_feature_vector_t features = {
        .values = {
            30.0f,
            4.0f,
            1.2f,
            100.0f,
            3.0f,
            1.0f,
        },
        .length = APP_FEATURE_COUNT,
    };
    app_inference_result_t result = { 0 };
    const char *backend_name;
    bool ok;

    printf("\n[test_inference_run_high]\n");

    app_inference_init();
    ok = inference_run(&features, &result);
    backend_name = (result.backend_name != NULL) ? result.backend_name : "null";

    printf("backend=%s score=%.3f is_anomaly=%d\n",
           backend_name,
           result.anomaly_score,
           result.is_anomaly);

    TEST_ASSERT(ok, "inference_run succeeds for high feature values");
    TEST_ASSERT(result.anomaly_score > 0.80f, "high feature values produce high anomaly score");
    TEST_ASSERT(result.is_anomaly, "high feature values are classified as anomaly");
}

static void test_decision_update_normal(void)
{
    app_decision_state_t state;
    app_inference_result_t result = {
        .anomaly_score = 0.10f,
        .is_anomaly = false,
        .backend_name = "test",
    };

    printf("\n[test_decision_update_normal]\n");

    app_decision_init(&state);
    decision_update(&state, &result);

    printf("label=%s streak=%lu total=%lu anomalies=%lu\n",
           app_decision_state_to_string(state.label),
           (unsigned long)state.anomaly_streak,
           (unsigned long)state.total_windows,
           (unsigned long)state.anomaly_windows);

    TEST_ASSERT(state.label == APP_DECISION_STATE_NORMAL,
                "no anomaly keeps decision state at NORMAL");
}

static void test_decision_update_single_anomaly(void)
{
    app_decision_state_t state;
    app_inference_result_t result = {
        .anomaly_score = 0.60f,
        .is_anomaly = true,
        .backend_name = "test",
    };

    printf("\n[test_decision_update_single_anomaly]\n");

    app_decision_init(&state);
    decision_update(&state, &result);

    printf("label=%s streak=%lu total=%lu anomalies=%lu\n",
           app_decision_state_to_string(state.label),
           (unsigned long)state.anomaly_streak,
           (unsigned long)state.total_windows,
           (unsigned long)state.anomaly_windows);

    TEST_ASSERT(state.label == APP_DECISION_STATE_MONITOR,
                "single anomaly moves decision state to MONITOR");
}

static void test_decision_update_consecutive_anomaly(void)
{
    app_decision_state_t state;
    app_inference_result_t result = {
        .anomaly_score = 0.60f,
        .is_anomaly = true,
        .backend_name = "test",
    };

    printf("\n[test_decision_update_consecutive_anomaly]\n");

    app_decision_init(&state);
    decision_update(&state, &result);
    decision_update(&state, &result);

    printf("label=%s streak=%lu total=%lu anomalies=%lu\n",
           app_decision_state_to_string(state.label),
           (unsigned long)state.anomaly_streak,
           (unsigned long)state.total_windows,
           (unsigned long)state.anomaly_windows);

    TEST_ASSERT(state.label == APP_DECISION_STATE_ANOMALY,
                "consecutive anomalies move decision state to ANOMALY");
}

void app_test_run_all(void)
{
    s_test_total = 0U;
    s_test_failed = 0U;

    printf("\n=== app_test_run_all ===\n");

    test_feature_extract_basic();
    test_feature_extract_increasing();
    test_inference_run_low();
    test_inference_run_high();
    test_decision_update_normal();
    test_decision_update_single_anomaly();
    test_decision_update_consecutive_anomaly();

    printf("\n=== test summary ===\n");
    printf("total=%u failed=%u passed=%u\n",
           s_test_total,
           s_test_failed,
           s_test_total - s_test_failed);
}
