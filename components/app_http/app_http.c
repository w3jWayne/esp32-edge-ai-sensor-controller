#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "esp_http_server.h"
#include "esp_log.h"

#include "app_event.h"
#include "app_http.h"

#define APP_HTTP_SENSOR_MAX_BODY_LEN 96

static const char *TAG = "app_http";
static httpd_handle_t s_http_server = NULL;

static bool app_http_parse_sensor_json(const char *body, app_sensor_sample_t *sample)
{
    float temperature_c;
    float pressure_kpa;
    char extra;

    if (sscanf(body,
               " { \"temperature_c\" : %f , \"pressure_kpa\" : %f } %c",
               &temperature_c,
               &pressure_kpa,
               &extra) != 2) {
        return false;
    }

    sample->sample_index = 0U;
    sample->temperature_c = temperature_c;
    sample->pressure_kpa = pressure_kpa;
    return true;
}

static bool app_http_parse_sensor_csv(const char *body, app_sensor_sample_t *sample)
{
    float temperature_c;
    float pressure_kpa;
    char extra;

    if (sscanf(body, " %f , %f %c", &temperature_c, &pressure_kpa, &extra) != 2) {
        return false;
    }

    sample->sample_index = 0U;
    sample->temperature_c = temperature_c;
    sample->pressure_kpa = pressure_kpa;
    return true;
}

static bool app_http_parse_sensor_payload(const char *body, app_sensor_sample_t *sample)
{
    return app_http_parse_sensor_json(body, sample) ||
           app_http_parse_sensor_csv(body, sample);
}

static esp_err_t app_http_read_body(httpd_req_t *req, char *body, size_t body_len)
{
    int received = 0;

    if (req->content_len <= 0) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "empty request body");
        return ESP_FAIL;
    }

    if ((size_t)req->content_len >= body_len) {
        httpd_resp_set_status(req, "413 Payload Too Large");
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_sendstr(req, "payload too large\n");
        return ESP_FAIL;
    }

    while (received < req->content_len) {
        int ret = httpd_req_recv(req, body + received, req->content_len - received);

        if (ret <= 0) {
            ESP_LOGW(TAG, "failed to read request body");
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "failed to read request");
            return ESP_FAIL;
        }

        received += ret;
    }

    body[received] = '\0';
    return ESP_OK;
}

static esp_err_t app_http_sensor_post_handler(httpd_req_t *req)
{
    char body[APP_HTTP_SENSOR_MAX_BODY_LEN];
    app_sensor_sample_t sample;
    app_event_t event = {
        .type = APP_EVENT_HTTP_SENSOR_SAMPLE,
    };

    if (app_http_read_body(req, body, sizeof(body)) != ESP_OK) {
        return ESP_OK;
    }

    if (!app_http_parse_sensor_payload(body, &sample)) {
        ESP_LOGW(TAG, "invalid sensor payload: %s", body);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "invalid sensor payload");
        return ESP_OK;
    }

    /*
     * Parse in the HTTP request context, then hand off to the centralized app
     * event dispatcher. The dispatcher stays lightweight and routes the sample
     * to the pipeline worker queue.
     */
    event.data.sensor_sample = sample;
    if (!app_event_post(&event)) {
        ESP_LOGW(TAG, "app event queue unavailable or full");
        httpd_resp_set_status(req, "503 Service Unavailable");
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_sendstr(req, "app event queue unavailable\n");
        return ESP_OK;
    }

    ESP_LOGI(TAG,
             "queued sample temp=%.2f pressure=%.2f",
             (double)sample.temperature_c,
             (double)sample.pressure_kpa);

    httpd_resp_set_type(req, "text/plain");
    httpd_resp_sendstr(req, "ok\n");
    return ESP_OK;
}

esp_err_t app_http_start(void)
{
    static const httpd_uri_t sensor_post = {
        .uri = "/sensor",
        .method = HTTP_POST,
        .handler = app_http_sensor_post_handler,
        .user_ctx = NULL,
    };
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    esp_err_t err;

    if (s_http_server != NULL) {
        return ESP_OK;
    }

    config.lru_purge_enable = true;

    err = httpd_start(&s_http_server, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to start HTTP server: %s", esp_err_to_name(err));
        return err;
    }

    err = httpd_register_uri_handler(s_http_server, &sensor_post);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to register /sensor handler: %s", esp_err_to_name(err));
        httpd_stop(s_http_server);
        s_http_server = NULL;
        return err;
    }

    ESP_LOGI(TAG, "HTTP server started, POST /sensor");
    return ESP_OK;
}
