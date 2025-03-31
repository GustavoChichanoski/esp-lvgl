#ifndef __WIDGET_BUTTON_H__
#define __WIDGET_BUTTON_H__

#include "configs/project_types.h"
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

esp_err_t widgetButtonCreate(WidgetButton** widget, lv_obj_t* parent, char* text,
                             button_clicked_cb on_click, Point2D offset);
esp_err_t widgetButtonDestroy(WidgetButton* widget);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif