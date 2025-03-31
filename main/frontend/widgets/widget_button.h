#ifndef __WIDGET_BUTTON_H__
#define __WIDGET_BUTTON_H__

#include "esp_err.h"
#include "misc/lv_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*button_clicked_cb)(lv_event_t* event);

typedef struct WidgetButton {
    button_clicked_cb on_click;
    lv_obj_t* button;
    lv_obj_t* label;
} WidgetButton;

esp_err_t widget_button_create(WidgetButton** widget, lv_obj_t* parent, char* text,
                               button_clicked_cb on_click, int x, int y);
esp_err_t widget_button_destroy(WidgetButton* widget);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif