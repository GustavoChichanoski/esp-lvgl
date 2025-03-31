#include "esp_err.h"
#include "esp_log.h"

// Project includes
#include "./widget_thermometer.h"
#include "frontend/pallets/color_gradient.h"
#include "widgets/bar/lv_bar.h"

#define THERMOMETER_WIDTH  200
#define THERMOMETER_HEIGHT 20
#define TEMP_MIN           0
#define TEMP_MAX           100

const char* TAG_WIDGET_THERMOMETER = "[WIDGET THERMOMETER]";

lv_color_t get_thermometer_color(int temp) {
    temp = temp > TEMP_MAX ? TEMP_MAX : temp;
    temp = temp < TEMP_MIN ? TEMP_MIN : temp;

    temp = ((temp - TEMP_MIN) * COLOR_GRADIENT_SIZE) / (TEMP_MAX - TEMP_MIN);
    ESP_LOGI(TAG_WIDGET_THERMOMETER, "Gradient: %d", temp);
    return lv_color_hex(COLOR_GRADIENT[temp]);
}

esp_err_t widget_thermometer_set_temperature(WidgetThermometer* widget, int temp) {
    lv_bar_set_value(widget->thermometer, temp, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(widget->thermometer, get_thermometer_color(temp), LV_PART_INDICATOR);
    return ESP_OK;
}

esp_err_t widget_thermometer_create(WidgetThermometer** widget, lv_obj_t* parent, int temp) {
    /* Create the bar widget */
    lv_obj_t* thermometer = lv_bar_create(parent);
    lv_obj_set_size(thermometer, THERMOMETER_WIDTH, THERMOMETER_HEIGHT);
    lv_obj_align(thermometer, LV_ALIGN_CENTER, 0, 0);
    ESP_LOGI(TAG_WIDGET_THERMOMETER, "Created thermometer");

    /* Set the range for temperature values */
    lv_bar_set_range(thermometer, TEMP_MIN, TEMP_MAX);
    ESP_LOGI(TAG_WIDGET_THERMOMETER, "Set thermometer range");

    /* Optionally, customize styles for a thermometer look */
    lv_obj_set_style_bg_color(thermometer, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(thermometer, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(thermometer, get_thermometer_color(temp), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(thermometer, LV_OPA_COVER, LV_PART_INDICATOR);
    ESP_LOGI(TAG_WIDGET_THERMOMETER, "Set thermometer styles");

    /* Set the current temperature */
    lv_bar_set_value(thermometer, temp, LV_ANIM_ON);
    (*widget)              = malloc(sizeof(WidgetThermometer));
    (*widget)->thermometer = thermometer;
    ESP_LOGI(TAG_WIDGET_THERMOMETER, "Set thermometer value");
    return ESP_OK;
}

esp_err_t widget_thermometer_destroy(WidgetThermometer* widget) {
    if (!widget) return ESP_ERR_INVALID_STATE;
    if (widget->thermometer) lv_obj_delete(widget->thermometer);
    free(widget);
    return ESP_OK;
}
