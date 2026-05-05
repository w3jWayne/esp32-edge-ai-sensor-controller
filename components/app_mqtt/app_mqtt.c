#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "mqtt_client.h"

#include "app_mqtt.h"

#define APP_MQTT_PAYLOAD_BUFFER_SIZE 256

static const char *TAG = "app_mqtt";
static esp_mqtt_client_handle_t s_mqtt_client = NULL;
static bool s_mqtt_started = false;
static bool s_mqtt_connected = false;
static bool s_mqtt_disabled = false;

static size_t app_mqtt_copy_bytes(char *dest,
                                  size_t dest_size,
                                  const char *src,
                                  size_t src_len,
                                  bool *truncated)
{
    size_t copy_len = 0U;

    if (dest == NULL || dest_size == 0U) {
        return 0U;
    }

    if (truncated != NULL) {
        *truncated = false;
    }

    if (src != NULL && src_len > 0U) {
        copy_len = src_len;
        if (copy_len >= dest_size) {
            copy_len = dest_size - 1U;
            if (truncated != NULL) {
                *truncated = true;
            }
        }

        memcpy(dest, src, copy_len);
    }

    dest[copy_len] = '\0';
    return copy_len;
}

static bool app_mqtt_post_translated_event(const app_event_t *event)
{
    if (app_event_post(event)) {
        return true;
    }

    ESP_LOGW(TAG, "failed to enqueue app event type=%d", (int)event->type);
    return false;
}

static void app_mqtt_build_message_event(esp_mqtt_event_handle_t mqtt_event,
                                         app_event_t *event)
{
    app_event_mqtt_message_t *message = &event->data.mqtt_message;
    size_t topic_len = 0U;
    size_t data_len = 0U;

    memset(message, 0, sizeof(*message));

    if (mqtt_event == NULL) {
        return;
    }

    if (mqtt_event->topic_len > 0) {
        topic_len = (size_t)mqtt_event->topic_len;
    }

    if (mqtt_event->data_len > 0) {
        data_len = (size_t)mqtt_event->data_len;
    }

    message->topic_len = app_mqtt_copy_bytes(
        message->topic,
        sizeof(message->topic),
        mqtt_event->topic,
        topic_len,
        &message->topic_truncated);
    message->data_len = app_mqtt_copy_bytes(
        message->data,
        sizeof(message->data),
        mqtt_event->data,
        data_len,
        &message->data_truncated);
}

static void app_mqtt_build_error_event(esp_mqtt_event_handle_t mqtt_event,
                                       app_event_t *event)
{
    app_event_mqtt_error_t *error = &event->data.mqtt_error;

    memset(error, 0, sizeof(*error));
    error->connect_return_code = -1;

    if (mqtt_event == NULL || mqtt_event->error_handle == NULL) {
        return;
    }

    error->error_type = mqtt_event->error_handle->error_type;
    error->esp_tls_last_esp_err = mqtt_event->error_handle->esp_tls_last_esp_err;
    error->esp_tls_stack_err = mqtt_event->error_handle->esp_tls_stack_err;
    error->esp_transport_sock_errno = mqtt_event->error_handle->esp_transport_sock_errno;
    error->connect_return_code = mqtt_event->error_handle->connect_return_code;
}

static void app_mqtt_event_handler(void *handler_args,
                                   esp_event_base_t base,
                                   int32_t event_id,
                                   void *event_data)
{
    app_event_t event = { 0 };

    (void)handler_args;
    (void)base;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        event.type = APP_EVENT_MQTT_CONNECTED;
        app_mqtt_post_translated_event(&event);
        break;

    case MQTT_EVENT_DISCONNECTED:
        event.type = APP_EVENT_MQTT_DISCONNECTED;
        app_mqtt_post_translated_event(&event);
        break;

    case MQTT_EVENT_DATA:
        event.type = APP_EVENT_MQTT_MESSAGE;
        app_mqtt_build_message_event((esp_mqtt_event_handle_t)event_data, &event);
        app_mqtt_post_translated_event(&event);
        break;

    case MQTT_EVENT_ERROR:
        event.type = APP_EVENT_MQTT_ERROR;
        app_mqtt_build_error_event((esp_mqtt_event_handle_t)event_data, &event);
        app_mqtt_post_translated_event(&event);
        break;

    default:
        break;
    }
}

static void app_mqtt_handle_control_message(const app_event_mqtt_message_t *message)
{
    ESP_LOGI(TAG,
             "received MQTT message topic=%s payload=%s topic_truncated=%s data_truncated=%s",
             message->topic,
             message->data,
             message->topic_truncated ? "true" : "false",
             message->data_truncated ? "true" : "false");
}

void app_mqtt_init(void)
{
    esp_err_t err;
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_APP_MQTT_BROKER_URI,
        .credentials.client_id = CONFIG_APP_MQTT_CLIENT_ID,
    };

    if (s_mqtt_client != NULL || s_mqtt_disabled) {
        return;
    }

    if (strlen(CONFIG_APP_MQTT_BROKER_URI) == 0U) {
        s_mqtt_disabled = true;
        ESP_LOGW(TAG, "MQTT broker URI is empty; MQTT publishing disabled");
        return;
    }

    s_mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (s_mqtt_client == NULL) {
        ESP_LOGE(TAG, "failed to initialize MQTT client");
        return;
    }

    err = esp_mqtt_client_register_event(
        s_mqtt_client,
        ESP_EVENT_ANY_ID,
        app_mqtt_event_handler,
        NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to register MQTT event handler: %s", esp_err_to_name(err));
        esp_mqtt_client_destroy(s_mqtt_client);
        s_mqtt_client = NULL;
        return;
    }
}

void app_mqtt_start(void)
{
    esp_err_t err;

    if (s_mqtt_client == NULL) {
        app_mqtt_init();
    }

    if (s_mqtt_client == NULL || s_mqtt_started) {
        return;
    }

    err = esp_mqtt_client_start(s_mqtt_client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to start MQTT client: %s", esp_err_to_name(err));
        return;
    }

    s_mqtt_started = true;
    ESP_LOGI(TAG, "MQTT client started");
}

void app_mqtt_handle_connected(void)
{
    s_mqtt_connected = true;
    ESP_LOGI(TAG, "connected to broker");
}

void app_mqtt_handle_disconnected(void)
{
    s_mqtt_connected = false;
    ESP_LOGW(TAG, "disconnected from broker");
}

void app_mqtt_handle_message(const app_event_mqtt_message_t *message)
{
    if (message == NULL) {
        return;
    }

    /*
     * Keep command/message handling lightweight here. If MQTT commands grow
     * beyond simple routing or logging, hand them off to a dedicated worker.
     */
    app_mqtt_handle_control_message(message);
}

void app_mqtt_handle_error(const app_event_mqtt_error_t *error)
{
    if (error == NULL) {
        return;
    }

    ESP_LOGW(TAG,
             "MQTT client error type=%ld tls_last=%d tls_stack=%d sock_errno=%d connect_rc=%d",
             (long)error->error_type,
             error->esp_tls_last_esp_err,
             error->esp_tls_stack_err,
             error->esp_transport_sock_errno,
             error->connect_return_code);
}

bool app_mqtt_is_connected(void)
{
    return (s_mqtt_client != NULL) && s_mqtt_started && s_mqtt_connected;
}

bool app_mqtt_publish_decision(uint32_t sample_index,
                               const app_decision_state_t *decision_state,
                               const app_inference_result_t *inference_result)
{
    char payload[APP_MQTT_PAYLOAD_BUFFER_SIZE];
    const char *decision_label;
    const char *backend_name;
    int payload_len;
    int msg_id;

    if (decision_state == NULL || inference_result == NULL) {
        return false;
    }

    if (!app_mqtt_is_connected()) {
        return false;
    }

    decision_label = app_decision_state_to_string(decision_state->label);
    backend_name = (inference_result->backend_name != NULL) ? inference_result->backend_name : "unknown";

    payload_len = snprintf(
        payload,
        sizeof(payload),
        "{\"sample_index\":%lu,\"decision\":\"%s\",\"anomaly_score\":%.3f,\"is_anomaly\":%s,"
        "\"anomaly_streak\":%lu,\"total_windows\":%lu,\"backend\":\"%s\"}",
        (unsigned long)sample_index,
        decision_label,
        (double)inference_result->anomaly_score,
        inference_result->is_anomaly ? "true" : "false",
        (unsigned long)decision_state->anomaly_streak,
        (unsigned long)decision_state->total_windows,
        backend_name);

    if (payload_len < 0 || payload_len >= (int)sizeof(payload)) {
        ESP_LOGE(TAG, "decision payload exceeds MQTT buffer");
        return false;
    }

    msg_id = esp_mqtt_client_publish(
        s_mqtt_client,
        CONFIG_APP_MQTT_DECISION_TOPIC,
        payload,
        0,
        1,
        0);
    if (msg_id < 0) {
        ESP_LOGW(TAG, "failed to queue MQTT publish");
        return false;
    }

    ESP_LOGI(TAG, "published decision sample=%lu msg_id=%d", (unsigned long)sample_index, msg_id);
    return true;
}
