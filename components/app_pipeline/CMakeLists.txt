idf_component_register(
    SRCS "app_pipeline.c"
         "app_pipeline_queue.c"
    INCLUDE_DIRS "."
    REQUIRES app_sensor app_calibration app_window app_features app_inference app_decision app_config
)
