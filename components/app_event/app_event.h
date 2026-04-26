#pragma once

#include <stdbool.h>

#include "freertos/FreeRTOS.h"

typedef enum {
    APP_EVENT_WIFI_CONNECTED,
    APP_EVENT_WIFI_DISCONNECTED,
    APP_EVENT_WIFI_CONNECT_FAILED,
} app_event_t;

void app_event_init(void);
bool app_event_post(app_event_t event);
bool app_event_get(app_event_t *event, TickType_t ticks_to_wait);
void app_event_start(void);
