#include "./widget_button.h"
#include "esp_err.h"
#include "misc/lv_area.h"
#include "widgets/button/lv_button.h"
#include "widgets/label/lv_label.h"

/**
 * @brief Create a widget_button.
 *
 * This function creates a new widget_button with a label and attaches an event callback
 * for click events. The button is aligned based on the specified offset.
 *
 * @param widget[out] Pointer to where the created WidgetButton will be stored.
 * @param parent[in] The parent LVGL object to which the button will be attached.
 * @param text[in] The text to be displayed on the button's label.
 * @param on_click[callback] The callback function to be called when the button is clicked.
 * @param offset[in] The offset position for aligning the button relative to its parent.
 *
 * @return esp_err_t ESP_OK on success, or ESP_ERR_NO_MEM if memory allocation fails.
 */
esp_err_t widgetButtonCreate(WidgetButton** widget, lv_obj_t* parent, char* text,
                             button_clicked_cb on_click, Point2D position) {
    // Input validation
    if (!parent || !text) return ESP_ERR_INVALID_ARG;

    // Create the button
    lv_obj_t* button = lv_button_create(parent);
    if (!button) return ESP_ERR_NO_MEM;

    // Set button position
    lv_obj_align(button, LV_ALIGN_BOTTOM_LEFT, position.x, position.y);

    // Add event callback
    lv_obj_add_event_cb(button, on_click, LV_EVENT_CLICKED, button);

    // Create the label
    lv_obj_t* label = lv_label_create(button);
    if (!label) {
        lv_obj_delete(button);
        return ESP_ERR_NO_MEM;
    }
    lv_label_set_text(label, text);
    lv_obj_center(label);

    // Allocate memory for the widget
    (*widget)           = (WidgetButton*)malloc(sizeof(WidgetButton));
    (*widget)->button   = button;
    (*widget)->label    = label;
    (*widget)->on_click = on_click;
    return ESP_OK;
}

/**
 * @brief Destroy the widget_button.
 *
 * This function releases resources associated with the widget_button.
 *
 * @param widget The widget_button to be destroyed.
 *
 * @return esp_err_t ESP_OK on success or an appropriate error code on failure.
 *                   ESP_ERR_INVALID_STATE if the widget_button is invalid.
 */
esp_err_t widgetButtonDestroy(WidgetButton* widget) {
    if (!widget) return ESP_ERR_INVALID_STATE;

    if (widget->button) lv_obj_delete(widget->button);
    if (widget->label) lv_obj_delete(widget->label);

    free(widget);
    return ESP_OK;
}