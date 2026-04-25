#include "app_decision.h"

void app_decision_init(app_decision_state_t *state)
{
    if (state == NULL) {
        return;
    }

    state->label = APP_DECISION_STATE_NORMAL;
    state->total_windows = 0U;
    state->anomaly_windows = 0U;
    state->anomaly_streak = 0U;
}

void decision_update(app_decision_state_t *state, const app_inference_result_t *result)
{
    if (state == NULL || result == NULL) {
        return;
    }

    state->total_windows++;

    if (result->is_anomaly) {
        state->anomaly_windows++;
        state->anomaly_streak++;
    } else {
        state->anomaly_streak = 0U;
    }

    if (result->anomaly_score >= 0.80f || state->anomaly_streak >= 2U) {
        state->label = APP_DECISION_STATE_ANOMALY;
    } else if (result->anomaly_score >= 0.45f || result->is_anomaly) {
        state->label = APP_DECISION_STATE_MONITOR;
    } else {
        state->label = APP_DECISION_STATE_NORMAL;
    }
}

const char *app_decision_state_to_string(app_decision_label_t label)
{
    switch (label) {
    case APP_DECISION_STATE_NORMAL:
        return "normal";

    case APP_DECISION_STATE_MONITOR:
        return "monitor";

    case APP_DECISION_STATE_ANOMALY:
        return "anomaly";

    default:
        return "unknown";
    }
}
