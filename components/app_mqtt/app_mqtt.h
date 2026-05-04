#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "app_decision.h"
#include "app_inference.h"

void app_mqtt_init(void);
void app_mqtt_start(void);
bool app_mqtt_is_connected(void);
bool app_mqtt_publish_decision(uint32_t sample_index,
                               const app_decision_state_t *decision_state,
                               const app_inference_result_t *inference_result);
