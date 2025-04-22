#ifndef __WIDGET_STACK_H__
#define __WIDGET_STACK_H__

#include "configs/project_types.h"
#include "esp_err.h"
#include "misc/lv_types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_obj_t* chart;
    lv_chart_series_t* series;
    lv_obj_t* scale;
    lv_obj_t* title;
} WidgetChart;

WidgetChart* widgetChartCreate(lv_obj_t* parent, Point2D size, Point2D position);

esp_err_t widgetChartAddPoint(WidgetChart* widget, int32_t value);

esp_err_t widgetChartDestroy(WidgetChart* widget);

#ifdef __cplusplus
}
#endif

#endif