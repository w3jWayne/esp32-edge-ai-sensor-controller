// app_config.h

#pragma once

#define APP_SENSOR_SAMPLE_PERIOD_MS        1000U
#define APP_WINDOW_CAPACITY                16U

#define APP_CAL_TEMP_SCALE                 1.0f
#define APP_CAL_TEMP_OFFSET                0.0f
#define APP_CAL_PRESSURE_SCALE             1.0f
#define APP_CAL_PRESSURE_OFFSET            0.0f

#define APP_DECISION_MONITOR_SCORE         0.45f
#define APP_DECISION_ANOMALY_SCORE         0.80f
#define APP_DECISION_ANOMALY_STREAK        2U

#define APP_INFER_ANOMALY_THRESHOLD 0.50f