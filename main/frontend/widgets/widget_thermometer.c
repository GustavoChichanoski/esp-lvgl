#include "widget_thermometer.h"
#include "widgets/bar/lv_bar.h"

#define THERMOMETER_WIDTH  200
#define THERMOMETER_HEIGHT 20
#define TEMP_MIN           0
#define TEMP_MAX           100

lv_obj_t* create_thermometer(lv_obj_t* parent, int temp) {
    /* Create the bar widget */
    lv_obj_t* thermometer = lv_bar_create(parent);
    lv_obj_set_size(thermometer, THERMOMETER_WIDTH, THERMOMETER_HEIGHT);
    lv_obj_align(thermometer, LV_ALIGN_CENTER, 0, 0);

    /* Set the range for temperature values */
    lv_bar_set_range(thermometer, TEMP_MIN, TEMP_MAX);

    /* Optionally, customize styles for a thermometer look */
    lv_obj_set_style_bg_color(thermometer, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(thermometer, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(thermometer, lv_color_hex(0xFF0000), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(thermometer, LV_OPA_COVER, LV_PART_INDICATOR);

    /* Set the current temperature */
    lv_bar_set_value(thermometer, temp, LV_ANIM_ON);

    return thermometer;
}

esp_err_t widget_thermometer_create(WidgetThermometer** widget, lv_obj_t* parent, int temp) {
    lv_obj_t* thermometer  = create_thermometer(parent, temp);
    (*widget)              = malloc(sizeof(WidgetThermometer));
    (*widget)->thermometer = thermometer;
    return ESP_OK;
}

esp_err_t widget_thermometer_destroy(WidgetThermometer* widget) {
    lv_obj_delete(widget->thermometer);
    free(widget);
    return ESP_OK;
}
