#include "app_window.h"

void app_window_init(app_window_t *window)
{
    if (window == NULL) {
        return;
    }

    window->next_index = 0U;
    window->count = 0U;
}

bool window_push(app_window_t *window, const app_sensor_sample_t *sample)
{
    if (window == NULL || sample == NULL) {
        return false;
    }

    window->samples[window->next_index] = *sample;
    window->next_index = (window->next_index + 1U) % APP_WINDOW_CAPACITY;

    if (window->count < APP_WINDOW_CAPACITY) {
        window->count++;
    }

    return true;
}

bool app_window_is_full(const app_window_t *window)
{
    return (window != NULL) && (window->count == APP_WINDOW_CAPACITY);
}

size_t app_window_count(const app_window_t *window)
{
    if (window == NULL) {
        return 0U;
    }

    return window->count;
}

const app_sensor_sample_t *app_window_get(const app_window_t *window, size_t ordered_index)
{
    size_t start_index;
    size_t physical_index;

    if (window == NULL || ordered_index >= window->count) {
        return NULL;
    }

    start_index = app_window_is_full(window) ? window->next_index : 0U;
    physical_index = (start_index + ordered_index) % APP_WINDOW_CAPACITY;

    return &window->samples[physical_index];
}
