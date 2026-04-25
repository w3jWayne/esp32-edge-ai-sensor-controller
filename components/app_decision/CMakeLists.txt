idf_component_register(
    SRCS "app_decision.c"
    INCLUDE_DIRS "."
    REQUIRES app_inference app_config
)
