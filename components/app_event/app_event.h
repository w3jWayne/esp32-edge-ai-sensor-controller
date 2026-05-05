#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "app_sensor.h"

typedef enum {
    APP_EVENT_HTTP_SENSOR_SAMPLE,
    APP_EVENT_MQTT_CONNECTED,
    APP_EVENT_MQTT_DISCONNECTED,
    APP_EVENT_MQTT_MESSAGE,
    APP_EVENT_MQTT_ERROR,
    APP_EVENT_WIFI_GOT_IP,
    APP_EVENT_WIFI_DISCONNECTED,
    APP_EVENT_WIFI_CONNECT_FAILED,
} app_event_type_t;

#define APP_EVENT_MQTT_TOPIC_MAX_LEN 64U
#define APP_EVENT_MQTT_DATA_MAX_LEN 128U

typedef struct {
    size_t topic_len;
    size_t data_len;
    bool topic_truncated;
    bool data_truncated;
    char topic[APP_EVENT_MQTT_TOPIC_MAX_LEN + 1U];
    char data[APP_EVENT_MQTT_DATA_MAX_LEN + 1U];
} app_event_mqtt_message_t;

typedef struct {
    int32_t error_type;
    int esp_tls_last_esp_err;
    int esp_tls_stack_err;
    int esp_transport_sock_errno;
    int connect_return_code;
} app_event_mqtt_error_t;

typedef struct {
    app_event_type_t type;
    union {
        app_sensor_sample_t sensor_sample;
        app_event_mqtt_message_t mqtt_message;
        app_event_mqtt_error_t mqtt_error;
    } data;
} app_event_t;

typedef struct {
    void (*start_mqtt)(void);
    bool (*post_sensor_sample)(const app_sensor_sample_t *sample, TickType_t ticks_to_wait);
    void (*handle_mqtt_connected)(void);
    void (*handle_mqtt_disconnected)(void);
    void (*handle_mqtt_message)(const app_event_mqtt_message_t *message);
    void (*handle_mqtt_error)(const app_event_mqtt_error_t *error);
} app_event_dispatch_handlers_t;

void app_event_init(void);
void app_event_register_dispatch_handlers(const app_event_dispatch_handlers_t *handlers);
bool app_event_post(const app_event_t *event);
bool app_event_get(app_event_t *event, TickType_t ticks_to_wait);
void app_event_start(void);
