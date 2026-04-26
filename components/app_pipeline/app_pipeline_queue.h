#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "app_sensor.h"

void app_pipeline_queue_init(void);
QueueHandle_t app_pipeline_queue_get(void);
