idf_component_register(
    SRCS "test_main.c"
    INCLUDE_DIRS "."
    REQUIRES app_window app_features app_inference app_decision
)
