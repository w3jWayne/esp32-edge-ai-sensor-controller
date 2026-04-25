#include <stddef.h>

#include "app_calibration.h"

void app_calibration_init(app_calibration_t *calibration)
{
    if (calibration == NULL) {
        return;
    }

    calibration->temperature_offset_c = -0.15f;
    calibration->pressure_offset_kpa = 0.40f;
    calibration->temperature_scale = 1.0f;
    calibration->pressure_scale = 1.0f;
}

bool calibration_apply(const app_calibration_t *calibration,
                       const app_sensor_sample_t *raw_sample,
                       app_sensor_sample_t *calibrated_sample)
{
    if (calibration == NULL || raw_sample == NULL || calibrated_sample == NULL) {
        return false;
    }

    *calibrated_sample = *raw_sample;
    calibrated_sample->temperature_c =
        (raw_sample->temperature_c * calibration->temperature_scale) +
        calibration->temperature_offset_c;
    calibrated_sample->pressure_kpa =
        (raw_sample->pressure_kpa * calibration->pressure_scale) +
        calibration->pressure_offset_kpa;

    return true;
}
