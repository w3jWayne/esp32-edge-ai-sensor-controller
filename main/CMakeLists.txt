idf_component_register(
    SRCS "app_main.c"
    INCLUDE_DIRS "."
    REQUIRES app_event app_wifi app_sensor app_calibration app_window app_features app_inference app_decision
)
