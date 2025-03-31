idf_component_register(
    SRCS
    "app_main.c"
    "backend/controllers/controller_screen.c"
    "backend/devices/rotary_encoder.c"
    "backend/devices/temperature.c"
    "backend/ports/lv_port_disp.c"
    "backend/tasks/task_gui.c"
    "frontend/screens/screen_encoder.c"
    "frontend/screens/screen_temperature.c"
    "frontend/screens/screen_simple.c"
    "frontend/widgets/widget_button.c"
    "frontend/widgets/widget_thermometer.c"
    "utils/logs.c"
    REQUIRES lvgl
    PRIV_REQUIRES esp_driver_spi esp_lcd
    INCLUDE_DIRS "."
)
