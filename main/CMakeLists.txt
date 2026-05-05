idf_component_register(
    SRCS "app_main.c"
    INCLUDE_DIRS "."
    REQUIRES app_event app_http app_mqtt app_wifi app_sensor app_calibration app_window app_features app_inference app_decision app_pipeline app_test
)
