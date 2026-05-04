#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "app_event.h"
#include "app_mqtt.h"

static QueueHandle_t s_app_event_queue = NULL;
static const char *TAG = "app_event_task";
static StaticQueue_t s_app_event_queue_struct;
static uint8_t s_app_event_queue_storage[10 * sizeof(app_event_t)];
static StaticTask_t s_app_event_task_buffer;
static StackType_t s_app_event_task_stack[4096];
static bool s_app_event_task_started = false;

void app_event_init(void)
{
    if (s_app_event_queue == NULL) {
        s_app_event_queue = xQueueCreateStatic(
            10,
            sizeof(app_event_t),
            s_app_event_queue_storage,
            &s_app_event_queue_struct);
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
            case APP_EVENT_WIFI_GOT_IP:
                ESP_LOGI(TAG, "APP_EVENT_WIFI_GOT_IP");
                app_mqtt_start();
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
    if (!s_app_event_task_started) {
        xTaskCreateStatic(
            app_event_task,
            "app_event_task",
            sizeof(s_app_event_task_stack) / sizeof(s_app_event_task_stack[0]),
            NULL,
            5,
            s_app_event_task_stack,
            &s_app_event_task_buffer);
        s_app_event_task_started = true;
    }
}
