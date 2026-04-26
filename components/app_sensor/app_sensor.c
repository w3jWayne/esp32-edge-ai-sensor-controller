#include <stddef.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "app_sensor.h"
#include "app_config.h"

#define APP_SENSOR_PATTERN_LENGTH 8U
#define APP_SENSOR_ANOMALY_PERIOD 48U
#define APP_SENSOR_ANOMALY_LENGTH 6U
#define APP_SENSOR_QUEUE_LENGTH 12U

static const char *TAG = "app_sensor";

static const float s_temperature_pattern[APP_SENSOR_PATTERN_LENGTH] = {
    0.00f, 0.15f, -0.10f, 0.20f, -0.05f, 0.10f, -0.15f, 0.05f,
};

static const float s_pressure_pattern[APP_SENSOR_PATTERN_LENGTH] = {
    0.00f, -0.10f, 0.15f, -0.05f, 0.10f, -0.15f, 0.05f, -0.05f,
};

static StaticQueue_t s_sensor_queue_struct;
static uint8_t s_sensor_queue_storage[APP_SENSOR_QUEUE_LENGTH * sizeof(app_sensor_sample_t)];
static QueueHandle_t s_sensor_queue = NULL;
static portMUX_TYPE s_sensor_queue_lock = portMUX_INITIALIZER_UNLOCKED;

static const char *app_sensor_mode_to_string(app_sensor_mode_t mode)
{
    switch (mode) {
    case APP_SENSOR_MODE_SIMULATED:
        return "simulated";

    case APP_SENSOR_MODE_HTTP_QUEUE:
        return "http_queue";

    default:
        return "unknown";
    }
}

static QueueHandle_t app_sensor_get_queue(void)
{
    QueueHandle_t queue = s_sensor_queue;

    if (queue != NULL) {
        return queue;
    }

    taskENTER_CRITICAL(&s_sensor_queue_lock);

    if (s_sensor_queue == NULL) {
        s_sensor_queue = xQueueCreateStatic(
            APP_SENSOR_QUEUE_LENGTH,
            sizeof(app_sensor_sample_t),
            s_sensor_queue_storage,
            &s_sensor_queue_struct);
    }

    queue = s_sensor_queue;
    taskEXIT_CRITICAL(&s_sensor_queue_lock);

    return queue;
}

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
    sensor->queue = NULL;

    if (mode == APP_SENSOR_MODE_HTTP_QUEUE) {
        sensor->queue = app_sensor_get_queue();

        if (sensor->queue == NULL) {
            ESP_LOGE(TAG, "failed to initialize HTTP queue mode");
        }
    }

    ESP_LOGI(TAG, "sensor mode=%s", app_sensor_mode_to_string(mode));
}

bool app_sensor_submit_sample(const app_sensor_sample_t *sample)
{
    QueueHandle_t queue;

    if (sample == NULL) {
        return false;
    }

    queue = app_sensor_get_queue();
    if (queue == NULL) {
        ESP_LOGE(TAG, "sensor queue unavailable");
        return false;
    }

    if (xQueueSend(queue, sample, 0) != pdPASS) {
        ESP_LOGW(TAG, "sensor queue full");
        return false;
    }

    return true;
}

bool app_sensor_read_sample(app_sensor_t *sensor, app_sensor_sample_t *sample)
{
    if (sensor == NULL || sample == NULL) {
        return false;
    }

    if (sensor->mode == APP_SENSOR_MODE_SIMULATED) {
        return app_sensor_read_simulated_sample(sensor, sample);
    }

    if (sensor->mode == APP_SENSOR_MODE_HTTP_QUEUE) {
        if (sensor->queue == NULL) {
            sensor->queue = app_sensor_get_queue();
        }

        if (sensor->queue == NULL) {
            return false;
        }

        if (xQueueReceive(sensor->queue,
                  sample,
                  pdMS_TO_TICKS(APP_SENSOR_SAMPLE_PERIOD_MS)) != pdPASS) {
            return false;
        }

        sample->sample_index = sensor->sample_index;
        sensor->sample_index++;
        return true;
    }

    ESP_LOGE(TAG, "unsupported sensor mode=%d", (int)sensor->mode);
    return false;
}
