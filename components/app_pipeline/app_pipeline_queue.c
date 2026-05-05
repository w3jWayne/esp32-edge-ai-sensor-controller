#include <stddef.h>

#include "esp_log.h"

#include "app_pipeline_queue.h"

#define APP_PIPELINE_QUEUE_LENGTH 8U

static const char *TAG = "app_pipeline_queue";
static StaticQueue_t s_pipeline_queue_struct;
static uint8_t s_pipeline_queue_storage[APP_PIPELINE_QUEUE_LENGTH * sizeof(app_sensor_sample_t)];
static QueueHandle_t s_pipeline_queue = NULL;

void app_pipeline_queue_init(void)
{
    if (s_pipeline_queue != NULL) {
        return;
    }

    s_pipeline_queue = xQueueCreateStatic(
        APP_PIPELINE_QUEUE_LENGTH,
        sizeof(app_sensor_sample_t),
        s_pipeline_queue_storage,
        &s_pipeline_queue_struct);

    if (s_pipeline_queue == NULL) {
        ESP_LOGE(TAG, "failed to create pipeline queue");
        return;
    }

    ESP_LOGI(TAG, "pipeline queue ready depth=%u", (unsigned int)APP_PIPELINE_QUEUE_LENGTH);
}

QueueHandle_t app_pipeline_queue_get(void)
{
    return s_pipeline_queue;
}

bool app_pipeline_queue_post(const app_sensor_sample_t *sample, TickType_t ticks_to_wait)
{
    if (s_pipeline_queue == NULL || sample == NULL) {
        return false;
    }

    return (xQueueSend(s_pipeline_queue, sample, ticks_to_wait) == pdPASS);
}

bool app_pipeline_queue_receive(app_sensor_sample_t *sample, TickType_t ticks_to_wait)
{
    if (s_pipeline_queue == NULL || sample == NULL) {
        return false;
    }

    return (xQueueReceive(s_pipeline_queue, sample, ticks_to_wait) == pdPASS);
}
