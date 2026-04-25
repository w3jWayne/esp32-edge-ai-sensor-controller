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

/**
 * @brief Update decision state based on inference result
 *
 * This function translates inference outputs (anomaly score and classification)
 * into a higher-level system state with temporal smoothing and hysteresis.
 *
 * Key responsibilities:
 * - Track total processed windows
 * - Track anomaly occurrences and consecutive anomaly streaks
 * - Apply multi-level decision logic (NORMAL / MONITOR / ANOMALY)
 *
 * Design principles:
 * - Avoid reacting to single noisy samples (debounce using streak)
 * - Distinguish between mild anomaly and critical anomaly
 * - Provide stable system behavior via state-based classification
 *
 * @param state   Persistent decision state (updated in-place)
 * @param result  Output from inference stage
 */
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

    // --- Decision logic (multi-level classification) ---
    //
    // ANOMALY:
    //  - Very high confidence (score >= 0.80), OR
    //  - Persistent anomaly (streak >= 2)
    //
    // MONITOR:
    //  - Medium score (>= 0.45), OR
    //  - Inference flagged anomaly but not persistent yet
    //
    // NORMAL:
    //  - Low score and no anomaly indication
    //
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
