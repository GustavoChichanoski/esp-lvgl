#include "configs/project_types.h"
#include "esp_err.h"
#include "esp_log.h"

// Project includes
#include "./widget_thermometer.h"
#include "frontend/pallets/color_gradient.h"
#include "misc/lv_area.h"
#include "misc/lv_color.h"
#include "misc/lv_types.h"
#include "widgets/bar/lv_bar.h"

#define THERMOMETER_WIDTH  20
#define THERMOMETER_HEIGHT 200
#define TEMP_MIN           0
#define TEMP_MAX           100

const char* tag_widget_thermometer = "[WIDGET THERMOMETER]";

lv_color_t getThermometerColor(int temp) {
    temp = temp > TEMP_MAX ? TEMP_MAX : temp;
    temp = temp < TEMP_MIN ? TEMP_MIN : temp;

    temp = ((temp - TEMP_MIN) * COLOR_GRADIENT_SIZE) / (TEMP_MAX - TEMP_MIN);
    ESP_LOGI(tag_widget_thermometer, "Gradient: %d", temp);
    return lv_color_hex(kColorGradient[temp]);
}

esp_err_t widgetThermometerSetShadow(lv_obj_t* thermometer, lv_color_t color) {
    // Set the shadow properties
    lv_obj_set_style_shadow_width(thermometer, 10, LV_PART_MAIN);       // Shadow width
    lv_obj_set_style_shadow_color(thermometer, color, LV_PART_MAIN);    // Shadow color
    lv_obj_set_style_shadow_opa(thermometer, LV_OPA_50, LV_PART_MAIN);  // Shadow opacity
    lv_obj_set_style_shadow_offset_x(thermometer, 5, LV_PART_MAIN);     // X offset
    lv_obj_set_style_shadow_offset_y(thermometer, 5, LV_PART_MAIN);     // Y offset
    lv_obj_set_style_shadow_spread(thermometer, 0, LV_PART_MAIN);       // Shadow spread
    ESP_LOGI(tag_widget_thermometer, "Set thermometer shadow");
    return ESP_OK;
}

esp_err_t widgetThermometerSetTemperature(WidgetThermometer* widget, int temp) {
    lv_color_t color      = getThermometerColor(temp);
    lv_obj_t* thermometer = widget->thermometer;
    lv_bar_set_value(thermometer, temp, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(thermometer, color, LV_PART_INDICATOR);
    if (temp < 50) color = lv_color_hex(0xCCCCCC);
    ESP_ERROR_CHECK(widgetThermometerSetShadow(thermometer, color));
    return ESP_OK;
}

void widgetThermometerSetBorder(lv_obj_t* thermometer, lv_color_t color) {
    lv_obj_set_style_border_color(thermometer, color, LV_PART_MAIN);       // Gray color
    lv_obj_set_style_border_width(thermometer, 1, LV_PART_MAIN);           // 1 pixel width
    lv_obj_set_style_border_opa(thermometer, LV_OPA_COVER, LV_PART_MAIN);  // Full opacity
    ESP_LOGI(tag_widget_thermometer, "Set thermometer border");
}

esp_err_t widgetThermometerCreate(WidgetThermometer** widget, lv_obj_t* parent, int temp,
                                  Point2D offset) {
    /* Create the bar widget */
    lv_obj_t* thermometer = lv_bar_create(parent);
    lv_bar_set_orientation(thermometer, LV_BAR_ORIENTATION_VERTICAL);
    lv_obj_set_size(thermometer, THERMOMETER_WIDTH, THERMOMETER_HEIGHT);
    lv_obj_align(thermometer, LV_ALIGN_BOTTOM_LEFT, offset.x, offset.y);
    ESP_LOGI(tag_widget_thermometer, "Created thermometer");

    /* Set the range for temperature values */
    lv_bar_set_range(thermometer, TEMP_MIN, TEMP_MAX);
    ESP_LOGI(tag_widget_thermometer, "Set thermometer range");

    /* Optionally, customize styles for a thermometer look */
    lv_obj_set_style_bg_color(thermometer, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(thermometer, LV_OPA_COVER, LV_PART_MAIN);
    ESP_LOGI(tag_widget_thermometer, "Set thermometer styles");

    // Set the shadow properties
    widgetThermometerSetShadow(thermometer, lv_color_hex(0x000000));

    /* Set the border properties */
    widgetThermometerSetBorder(thermometer, lv_color_hex(0x808080));

    /* Set the current temperature */
    lv_bar_set_value(thermometer, temp, LV_ANIM_ON);
    (*widget)              = (WidgetThermometer*)malloc(sizeof(WidgetThermometer));
    (*widget)->thermometer = thermometer;
    ESP_LOGI(tag_widget_thermometer, "Set thermometer value");
    return ESP_OK;
}

esp_err_t widgetThermometerDestroy(WidgetThermometer* widget) {
    if (!widget) return ESP_ERR_INVALID_STATE;
    if (widget->thermometer) lv_obj_delete(widget->thermometer);
    free(widget);
    return ESP_OK;
}
