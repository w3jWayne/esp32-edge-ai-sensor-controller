#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "app_event.h"

#define APP_EVENT_QUEUE_LENGTH 16U

static QueueHandle_t s_app_event_queue = NULL;
static const char *TAG = "app_event_task";
static StaticQueue_t s_app_event_queue_struct;
static uint8_t s_app_event_queue_storage[APP_EVENT_QUEUE_LENGTH * sizeof(app_event_t)];
static StaticTask_t s_app_event_task_buffer;
static StackType_t s_app_event_task_stack[4096];
static bool s_app_event_task_started = false;
static app_event_dispatch_handlers_t s_dispatch_handlers;

static const char *app_event_type_to_string(app_event_type_t type)
{
    switch (type) {
    case APP_EVENT_HTTP_SENSOR_SAMPLE:
        return "APP_EVENT_HTTP_SENSOR_SAMPLE";
    case APP_EVENT_MQTT_CONNECTED:
        return "APP_EVENT_MQTT_CONNECTED";
    case APP_EVENT_MQTT_DISCONNECTED:
        return "APP_EVENT_MQTT_DISCONNECTED";
    case APP_EVENT_MQTT_MESSAGE:
        return "APP_EVENT_MQTT_MESSAGE";
    case APP_EVENT_MQTT_ERROR:
        return "APP_EVENT_MQTT_ERROR";
    case APP_EVENT_WIFI_GOT_IP:
        return "APP_EVENT_WIFI_GOT_IP";
    case APP_EVENT_WIFI_DISCONNECTED:
        return "APP_EVENT_WIFI_DISCONNECTED";
    case APP_EVENT_WIFI_CONNECT_FAILED:
        return "APP_EVENT_WIFI_CONNECT_FAILED";
    default:
        return "APP_EVENT_UNKNOWN";
    }
}

void app_event_init(void)
{
    if (s_app_event_queue == NULL) {
        s_app_event_queue = xQueueCreateStatic(
            APP_EVENT_QUEUE_LENGTH,
            sizeof(app_event_t),
            s_app_event_queue_storage,
            &s_app_event_queue_struct);
    }
}

void app_event_register_dispatch_handlers(const app_event_dispatch_handlers_t *handlers)
{
    if (handlers == NULL) {
        memset(&s_dispatch_handlers, 0, sizeof(s_dispatch_handlers));
        return;
    }

    s_dispatch_handlers = *handlers;
}

bool app_event_post(const app_event_t *event)
{
    if (s_app_event_queue == NULL || event == NULL) {
        return false;
    }

    return (xQueueSend(s_app_event_queue, event, 0) == pdPASS);
}

bool app_event_get(app_event_t *event, TickType_t ticks_to_wait)
{
    if (s_app_event_queue == NULL || event == NULL) {
        return false;
    }

    return (xQueueReceive(s_app_event_queue, event, ticks_to_wait) == pdPASS);
}

static void app_event_dispatch_http_sensor_sample(const app_event_t *event)
{
    if (s_dispatch_handlers.post_sensor_sample == NULL) {
        ESP_LOGW(TAG, "dropping HTTP sample without registered pipeline sink");
        return;
    }

    if (!s_dispatch_handlers.post_sensor_sample(&event->data.sensor_sample, 0)) {
        ESP_LOGW(TAG, "failed to route HTTP sample to pipeline queue");
    }
}

static void app_event_dispatch_mqtt_connected(void)
{
    ESP_LOGI(TAG, "APP_EVENT_MQTT_CONNECTED");

    if (s_dispatch_handlers.handle_mqtt_connected != NULL) {
        s_dispatch_handlers.handle_mqtt_connected();
    }
}

static void app_event_dispatch_mqtt_disconnected(void)
{
    ESP_LOGW(TAG, "APP_EVENT_MQTT_DISCONNECTED");

    if (s_dispatch_handlers.handle_mqtt_disconnected != NULL) {
        s_dispatch_handlers.handle_mqtt_disconnected();
    }
}

static void app_event_dispatch_mqtt_message(const app_event_t *event)
{
    if (s_dispatch_handlers.handle_mqtt_message != NULL) {
        s_dispatch_handlers.handle_mqtt_message(&event->data.mqtt_message);
        return;
    }

    ESP_LOGW(TAG, "dropping MQTT message event without registered handler");
}

static void app_event_dispatch_mqtt_error(const app_event_t *event)
{
    if (s_dispatch_handlers.handle_mqtt_error != NULL) {
        s_dispatch_handlers.handle_mqtt_error(&event->data.mqtt_error);
        return;
    }

    ESP_LOGW(TAG, "dropping MQTT error event without registered handler");
}

static void app_event_dispatch(const app_event_t *event)
{
    switch (event->type) {
    case APP_EVENT_HTTP_SENSOR_SAMPLE:
        app_event_dispatch_http_sensor_sample(event);
        break;

    case APP_EVENT_MQTT_CONNECTED:
        app_event_dispatch_mqtt_connected();
        break;

    case APP_EVENT_MQTT_DISCONNECTED:
        app_event_dispatch_mqtt_disconnected();
        break;

    case APP_EVENT_MQTT_MESSAGE:
        app_event_dispatch_mqtt_message(event);
        break;

    case APP_EVENT_MQTT_ERROR:
        app_event_dispatch_mqtt_error(event);
        break;

    case APP_EVENT_WIFI_GOT_IP:
        ESP_LOGI(TAG, "APP_EVENT_WIFI_GOT_IP");
        if (s_dispatch_handlers.start_mqtt != NULL) {
            s_dispatch_handlers.start_mqtt();
        }
        break;

    case APP_EVENT_WIFI_DISCONNECTED:
        ESP_LOGI(TAG, "APP_EVENT_WIFI_DISCONNECTED");
        break;

    case APP_EVENT_WIFI_CONNECT_FAILED:
        ESP_LOGI(TAG, "APP_EVENT_WIFI_CONNECT_FAILED");
        break;

    default:
        ESP_LOGW(TAG, "unhandled event type %s", app_event_type_to_string(event->type));
        break;
    }
}

static void app_event_task(void *arg)
{
    app_event_t event;

    (void)arg;

    /*
     * Dispatcher only: this task routes already-parsed events to worker queues
     * or lightweight handlers. Calibration, feature extraction, inference,
     * decision logic, and blocking network work stay in their own components.
     */
    while (1) {
        if (app_event_get(&event, portMAX_DELAY)) {
            app_event_dispatch(&event);
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
