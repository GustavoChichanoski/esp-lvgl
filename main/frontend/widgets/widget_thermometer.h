#ifndef __WIDGET_THERMOMETER_H__
#define __WIDGET_THERMOMETER_H__

#include "esp_err.h"
#include "misc/lv_types.h"

#include "configs/project_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_obj_t* thermometer;
} WidgetThermometer;

esp_err_t widgetThermometerCreate(WidgetThermometer** widget, lv_obj_t* parent, int temp, Point2D offset);
esp_err_t widgetThermometerDestroy(WidgetThermometer* widget);
esp_err_t widgetThermometerSetTemperature(WidgetThermometer* widget, int temp);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif