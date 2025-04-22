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
    lv_obj_t* scale;
    lv_obj_t* bulb;
    Point2D size;
    Point2D offset;
} WidgetThermometer;

WidgetThermometer* widgetThermometerCreate(lv_obj_t* parent, int temp, Point2D size,
                                           Point2D offset);
void widgetThermometerSetSize(lv_obj_t* thermometer, Point2D size);
esp_err_t widgetThermometerDestroy(WidgetThermometer* widget);
esp_err_t widgetThermometerSetTemperature(WidgetThermometer* widget, int temp);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif