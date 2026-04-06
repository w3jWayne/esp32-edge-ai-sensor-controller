#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "app_event.h"

static QueueHandle_t s_app_event_queue = NULL;
static const char *TAG = "app_event_task";

void app_event_init(void)
{
    if (s_app_event_queue == NULL) {
        s_app_event_queue = xQueueCreate(10, sizeof(app_event_t));
    }
}

bool app_event_post(app_event_t event)
{
    if (s_app_event_queue == NULL) {
        return false;
    }

    return (xQueueSend(s_app_event_queue, &event, 0) == pdPASS);
}

bool app_event_get(app_event_t *event, TickType_t ticks_to_wait)
{
    if (s_app_event_queue == NULL || event == NULL) {
        return false;
    }

    return (xQueueReceive(s_app_event_queue, event, ticks_to_wait) == pdPASS);
}

static void app_event_task(void *arg)
{
    app_event_t event;

    while (1) {
        if (app_event_get(&event, portMAX_DELAY)) {
            switch (event) {
            case APP_EVENT_WIFI_CONNECTED:
                ESP_LOGI(TAG, "APP_EVENT_WIFI_CONNECTED");
                break;

            case APP_EVENT_WIFI_DISCONNECTED:
                ESP_LOGI(TAG, "APP_EVENT_WIFI_DISCONNECTED");
                break;

            case APP_EVENT_WIFI_CONNECT_FAILED:
                ESP_LOGI(TAG, "APP_EVENT_WIFI_CONNECT_FAILED");
                break;

            default:
                break;
            }
        }
    }
}

void app_event_start(void)
{
    xTaskCreate(app_event_task, "app_event_task", 4096, NULL, 5, NULL);
}