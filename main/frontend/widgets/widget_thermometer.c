#include "configs/project_types.h"
#include "esp_err.h"
#include "esp_log.h"

// Project includes
#include "./widget_thermometer.h"
#include "frontend/pallets/color_gradient.h"

// LVGL includes
#include "misc/lv_area.h"
#include "misc/lv_color.h"
#include "misc/lv_types.h"
#include "widgets/bar/lv_bar.h"
#include "widgets/scale/lv_scale.h"

#define THERMOMETER_WIDTH  20
#define THERMOMETER_HEIGHT 200
#define TEMP_MIN           0
#define TEMP_MAX           100

const char* tag_widget_thermometer = "[WIDGET THERMOMETER]";

lv_obj_t* widgetThermometerBulbCreate(lv_obj_t* parent, Point2D size, Point2D offset,
                                      lv_color_t color) {
    lv_obj_t* bulb = lv_obj_create(parent);
    lv_obj_set_size(bulb, size.x, size.y);
    lv_obj_align_to(bulb, parent, LV_ALIGN_OUT_BOTTOM_MID, offset.x, offset.y);
    lv_obj_set_style_radius(bulb, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(bulb, color, 0);
    lv_obj_set_style_border_width(bulb, 9, 0);
    lv_obj_set_style_border_color(bulb, lv_color_hex(0x10283f), 0);
    return bulb;
}

lv_obj_t* widgetThermometerLabel(lv_obj_t* parent, const char* text) {
    lv_obj_t* label = lv_label_create(parent);
    if (NULL == label) return label;
    lv_label_set_text(label, text);
    lv_obj_align_to(label, parent, LV_ALIGN_OUT_TOP_MID, 0, -10);
    return label;
}

lv_color_t getThermometerColor(int temp) {
    // Clamp the temperature to the defined range
    if (TEMP_MAX < temp) {
        temp = TEMP_MAX;
    } else if (TEMP_MIN > temp) {
        temp = TEMP_MIN;
    }

    // Normalize the temperature within the range
    int range           = TEMP_MAX - TEMP_MIN;
    int normalized_temp = temp - TEMP_MIN;

    // Calculate the gradient index
    int gradient_index = (normalized_temp * (COLOR_GRADIENT_SIZE - 1)) / range;

    ESP_LOGI(tag_widget_thermometer, "Temperature: %d, Gradient Index: %d", temp, gradient_index);

    // Retrieve and return the corresponding color
    return lv_color_hex(kGradientBlueRed[gradient_index]);
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
    if (temp < 50) color = lv_color_hex(0x333333);
    ESP_ERROR_CHECK(widgetThermometerSetShadow(thermometer, color));
    return ESP_OK;
}

void widgetThermometerSetBorder(lv_obj_t* thermometer, lv_color_t color) {
    lv_obj_set_style_border_color(thermometer, color, LV_PART_MAIN);       // Gray color
    lv_obj_set_style_border_width(thermometer, 1, LV_PART_MAIN);           // 1 pixel width
    lv_obj_set_style_border_opa(thermometer, LV_OPA_COVER, LV_PART_MAIN);  // Full opacity
    ESP_LOGI(tag_widget_thermometer, "Set thermometer border");
}

void widgetThermometerSetSize(lv_obj_t* thermometer, Point2D size) {
    lv_obj_set_size(thermometer, size.x, size.y);
    ESP_LOGI(tag_widget_thermometer, "Set thermometer size");
}

WidgetThermometer* widgetThermometerSetScale(WidgetThermometer* widget, lv_obj_t* parent,
                                             Point2D size, Point2D offset) {
    // Create the scale (graduation marks)
    widget->scale = lv_scale_create(parent);
    if (!widget->scale) {
        ESP_LOGE(tag_widget_thermometer, "Failed to create scale at %03d:%03d with size: %03d:%03d",
                 offset.x, offset.y, size.x, size.y);
        widgetThermometerDestroy(widget);
        return NULL;
    }
    lv_scale_set_mode(widget->scale, LV_SCALE_MODE_VERTICAL_RIGHT);
    lv_scale_set_range(widget->scale, TEMP_MIN + 10, TEMP_MAX - 10);
    lv_obj_set_size(widget->scale, size.x, size.y - 20);
    lv_obj_align(widget->scale, LV_ALIGN_BOTTOM_LEFT, offset.x, offset.y - 10);

    static lv_style_t scale_label_style;
    lv_style_init(&scale_label_style);
    lv_style_set_text_font(&scale_label_style, &lv_font_montserrat_10);
    lv_obj_add_style(widget->scale, &scale_label_style, LV_PART_INDICATOR);

    ESP_LOGI(tag_widget_thermometer, "Created scale");
    return widget;
}

void widgetThermometerMove(lv_obj_t* thermometer, Point2D offset) {}

WidgetThermometer* widgetThermometerCreate(lv_obj_t* parent, int temp, Point2D size,
                                           Point2D offset) {
    WidgetThermometer* widget = (WidgetThermometer*)malloc(sizeof(WidgetThermometer));
    /* Create the bar widget */
    widget->thermometer = lv_bar_create(parent);
    if (!widget->thermometer) {
        ESP_LOGE(tag_widget_thermometer, "Failed to create thermometer at %d:%d with size: %d:%d",
                 offset.x, offset.y, size.x, size.y);
        free(widget);
        return NULL;
    }
    lv_bar_set_orientation(widget->thermometer, LV_BAR_ORIENTATION_VERTICAL);
    lv_obj_set_size(widget->thermometer, size.x, size.y);
    lv_obj_align(widget->thermometer, LV_ALIGN_BOTTOM_LEFT, offset.x, offset.y);
    ESP_LOGI(tag_widget_thermometer, "Created thermometer");

    // Create the bulb (bottom part of the thermometer)
    widget->bulb =
        widgetThermometerBulbCreate(widget->thermometer, (Point2D){THERMOMETER_WIDTH, 20},
                                    (Point2D){0, 0}, lv_color_hex(0xFF0000));
    if (!widget->bulb) {
        ESP_LOGE(tag_widget_thermometer, "Failed to create bulb at %d:%d with size: %d:%d",
                 offset.x, offset.y, size.x, size.y);
        widgetThermometerDestroy(widget);
        return NULL;
    }
    widgetThermometerSetScale(widget, parent, size, offset);

    /* Set the range for temperature values */
    lv_bar_set_range(widget->thermometer, TEMP_MIN, TEMP_MAX);
    ESP_LOGI(tag_widget_thermometer, "Set thermometer range");

    /* Optionally, customize styles for a thermometer look */
    lv_obj_set_style_bg_color(widget->thermometer, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(widget->thermometer, LV_OPA_COVER, LV_PART_MAIN);
    ESP_LOGI(tag_widget_thermometer, "Set thermometer styles");

    // Set the shadow properties
    widgetThermometerSetShadow(widget->thermometer, lv_color_hex(0x000000));

    /* Set the border properties */
    widgetThermometerSetBorder(widget->thermometer, lv_color_hex(0x808080));

    /* Set the current temperature */
    lv_bar_set_value(widget->thermometer, temp, LV_ANIM_ON);

    widget->size   = size;
    widget->offset = offset;
    ESP_LOGI(tag_widget_thermometer, "Set thermometer value");
    return widget;
}

esp_err_t widgetThermometerDestroy(WidgetThermometer* widget) {
    if (!widget) return ESP_ERR_INVALID_STATE;
    if (widget->thermometer) lv_obj_delete(widget->thermometer);
    if (widget->scale) lv_obj_delete(widget->scale);
    free(widget);
    return ESP_OK;
}
