idf_component_register(
    SRCS "app_mqtt.c"
    INCLUDE_DIRS "."
    REQUIRES mqtt app_decision app_inference
)
