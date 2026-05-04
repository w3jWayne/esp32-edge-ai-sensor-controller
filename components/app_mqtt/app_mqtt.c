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

static void app_mqtt_event_handler(void *handler_args,
                                   esp_event_base_t base,
                                   int32_t event_id,
                                   void *event_data)
{
    (void)handler_args;
    (void)base;
    (void)event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        s_mqtt_connected = true;
        ESP_LOGI(TAG, "connected to broker");
        break;

    case MQTT_EVENT_DISCONNECTED:
        s_mqtt_connected = false;
        ESP_LOGW(TAG, "disconnected from broker");
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGW(TAG, "MQTT client error");
        break;

    default:
        break;
    }
}

void app_mqtt_init(void)
{
    esp_err_t err;
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_APP_MQTT_BROKER_URI,
        .credentials.client_id = CONFIG_APP_MQTT_CLIENT_ID,
    };

    if (s_mqtt_client != NULL) {
        return;
    }

    if (strlen(CONFIG_APP_MQTT_BROKER_URI) == 0U) {
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
