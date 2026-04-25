idf_component_register(
    SRCS "app_pipeline.c"
    INCLUDE_DIRS "."
    REQUIRES app_calibration app_window app_features app_inference app_decision
)
