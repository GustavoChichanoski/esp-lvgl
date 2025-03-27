#include "./widget_button.h"
#include "esp_err.h"
#include "widgets/button/lv_button.h"
#include "widgets/label/lv_label.h"

esp_err_t widget_button_create(WidgetButton** widget, lv_obj_t* parent, char* text,
                               button_clicked_cb on_click) {
    lv_obj_t* button = lv_button_create(parent);
    if (!button) return ESP_ERR_NO_MEM;
    lv_obj_add_event_cb(button, on_click, LV_EVENT_CLICKED, button);

    lv_obj_t* label = lv_label_create(button);
    if (!label) {
        lv_obj_delete(button);
        return ESP_ERR_NO_MEM;
    }
    lv_label_set_text(label, text);
    lv_obj_center(label);

    (*widget)           = malloc(sizeof(WidgetButton));
    (*widget)->button   = button;
    (*widget)->label    = label;
    (*widget)->on_click = on_click;
    return ESP_OK;
}

esp_err_t widget_button_destroy(WidgetButton* widget) {
    lv_obj_delete(widget->button);
    lv_obj_delete(widget->label);
    free(widget);
    return ESP_OK;
}