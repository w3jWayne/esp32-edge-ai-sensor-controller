#pragma once

#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "app_sensor.h"

void app_pipeline_queue_init(void);
QueueHandle_t app_pipeline_queue_get(void);
bool app_pipeline_queue_post(const app_sensor_sample_t *sample, TickType_t ticks_to_wait);
bool app_pipeline_queue_receive(app_sensor_sample_t *sample, TickType_t ticks_to_wait);
