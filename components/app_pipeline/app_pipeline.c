#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "app_sensor.h"
#include "app_pipeline_queue.h"
#include "app_calibration.h"
#include "app_window.h"
#include "app_features.h"
#include "app_inference.h"
#include "app_decision.h"
#include "app_config.h"
#include "app_mqtt.h"

static const char *TAG = "app_pipeline";
static StaticTask_t s_pipeline_task_buffer;
static StackType_t s_pipeline_task_stack[4096];

static app_sensor_mode_t app_pipeline_sensor_mode(void)
{
#if CONFIG_APP_SENSOR_MODE_HTTP_QUEUE
    return APP_SENSOR_MODE_HTTP_QUEUE;
#else
    return APP_SENSOR_MODE_SIMULATED;
#endif
}

static bool app_pipeline_read_http_sample(QueueHandle_t input_queue,
                                          app_sensor_sample_t *sample,
                                          uint32_t *sample_index)
{
    if (input_queue == NULL || sample == NULL || sample_index == NULL) {
        return false;
    }

    if (xQueueReceive(input_queue,
                      sample,
                      pdMS_TO_TICKS(APP_SENSOR_SAMPLE_PERIOD_MS)) != pdPASS) {
        return false;
    }

    sample->sample_index = *sample_index;
    (*sample_index)++;
    return true;
}

static void app_pipeline_task(void *arg)
{
    app_sensor_t sensor;
    app_calibration_t calibration;
    app_window_t window;
    app_decision_state_t decision_state;
    app_sensor_sample_t raw_sample;
    app_sensor_sample_t calibrated_sample;
    app_feature_vector_t feature_vector;
    app_inference_result_t inference_result;
    app_sensor_mode_t sensor_mode;
    QueueHandle_t input_queue = NULL;
    uint32_t http_sample_index = 0U;

    (void)arg;

    sensor_mode = app_pipeline_sensor_mode();
    if (sensor_mode == APP_SENSOR_MODE_SIMULATED) {
        app_sensor_init(&sensor, sensor_mode);
    } else {
        input_queue = app_pipeline_queue_get();
        if (input_queue == NULL) {
            ESP_LOGE(TAG, "pipeline queue not initialized");
            vTaskDelete(NULL);
            return;
        }
    }

    app_calibration_init(&calibration);
    app_window_init(&window);
    app_inference_init();
    app_decision_init(&decision_state);

    while (1) {
        if (sensor_mode == APP_SENSOR_MODE_HTTP_QUEUE) {
            if (!app_pipeline_read_http_sample(input_queue, &raw_sample, &http_sample_index)) {
                continue;
            }
        } else if (!app_sensor_read_sample(&sensor, &raw_sample)) {
            ESP_LOGE(TAG, "app_sensor_read_sample failed");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        // Every sample goes through: Raw Sample → Calibration Applied → Calibrated Sample → Window Buffer
        if (!calibration_apply(&calibration, &raw_sample, &calibrated_sample)) {
            ESP_LOGE(TAG, "calibration_apply failed");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        // A circular sliding buffer that collects calibrated sensor samples for feature extraction and ML inference.
        // Every second: one new calibrated sample pushed
        if (!window_push(&window, &calibrated_sample)) {
            ESP_LOGE(TAG, "window_push failed");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        // Check if the window buffer is full (contains enough samples for a complete feature extraction and inference cycle)
        if (app_window_is_full(&window)) {
            // Trigger the feature extraction and inference pipeline every second once the window is full
            if (!feature_extract(&window, &feature_vector)) {
                ESP_LOGE(TAG, "feature_extract failed");
            } else if (!inference_run(&feature_vector, &inference_result)) {
                ESP_LOGE(TAG, "inference_run failed");
            } else {
                decision_update(&decision_state, &inference_result);
                if (app_mqtt_is_connected()) {
                    app_mqtt_publish_decision(
                        calibrated_sample.sample_index,
                        &decision_state,
                        &inference_result);
                }
                ESP_LOGI(
                    TAG,
                    "sample=%lu score=%.2f decision=%s streak=%lu temp_mean=%.2f temp_range=%.2f pressure_mean=%.2f pressure_range=%.2f backend=%s",
                    (unsigned long)calibrated_sample.sample_index,
                    (double)inference_result.anomaly_score,
                    app_decision_state_to_string(decision_state.label),
                    (unsigned long)decision_state.anomaly_streak,
                    (double)feature_vector.values[APP_FEATURE_INDEX_TEMP_MEAN],
                    (double)feature_vector.values[APP_FEATURE_INDEX_TEMP_RANGE],
                    (double)feature_vector.values[APP_FEATURE_INDEX_PRESSURE_MEAN],
                    (double)feature_vector.values[APP_FEATURE_INDEX_PRESSURE_RANGE],
                    inference_result.backend_name);
            }
        }

        if (sensor_mode == APP_SENSOR_MODE_SIMULATED) {
            vTaskDelay(pdMS_TO_TICKS(APP_SENSOR_SAMPLE_PERIOD_MS));
        }
    }
}

void app_pipeline_start(void)
{
    TaskHandle_t handle;

    handle = xTaskCreateStatic(
                app_pipeline_task,
                "app_pipeline",
                sizeof(s_pipeline_task_stack) / sizeof(s_pipeline_task_stack[0]),
                NULL,
                5,
                s_pipeline_task_stack,
                &s_pipeline_task_buffer);
    
    if (handle == NULL) {
        ESP_LOGE(TAG, "failed to create pipeline task");
    }
}
