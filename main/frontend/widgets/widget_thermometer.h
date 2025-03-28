#ifndef __WIDGET_THERMOMETER_H__
#define __WIDGET_THERMOMETER_H__

#include "esp_err.h"
#include "misc/lv_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_obj_t* thermometer;
} WidgetThermometer;

esp_err_t widget_thermometer_create(WidgetThermometer** widget, lv_obj_t* parent, int temp);
esp_err_t widget_thermometer_destroy(WidgetThermometer* widget);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif