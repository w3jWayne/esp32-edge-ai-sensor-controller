#pragma once

#include <stdint.h>

#include "app_inference.h"

typedef enum {
    APP_DECISION_STATE_NORMAL = 0,
    APP_DECISION_STATE_MONITOR,
    APP_DECISION_STATE_ANOMALY,
} app_decision_label_t;

typedef struct {
    app_decision_label_t label;
    uint32_t total_windows;
    uint32_t anomaly_windows;
    uint32_t anomaly_streak;
} app_decision_state_t;

void app_decision_init(app_decision_state_t *state);
void decision_update(app_decision_state_t *state, const app_inference_result_t *result);
const char *app_decision_state_to_string(app_decision_label_t label);
