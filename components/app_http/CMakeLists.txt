idf_component_register(
    SRCS "app_http.c"
    INCLUDE_DIRS "."
    REQUIRES app_sensor esp_http_server
)
