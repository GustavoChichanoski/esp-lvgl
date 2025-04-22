// Standart libraries
#include <stdlib.h>

// LVGL
#include "core/lv_obj.h"
#include "core/lv_obj_pos.h"
#include "core/lv_obj_tree.h"
#include "misc/lv_area.h"
#include "misc/lv_types.h"
#include "widgets/chart/lv_chart.h"

// Project includes
#include "configs/project_types.h"
#include "widget_chart.h"

WidgetChart* widgetChartCreate(lv_obj_t* parent, Point2D size, Point2D position) {
    WidgetChart* widget = calloc(1, sizeof(WidgetChart));
    if (!widget) { return NULL; }

    widget->chart = lv_chart_create(parent);
    if (!widget->chart) {
        free(widget);
        return NULL;
    }

    lv_chart_set_update_mode(widget->chart, LV_CHART_UPDATE_MODE_CIRCULAR);
    lv_obj_set_size(widget->chart, size.x, size.y);
    lv_obj_align(widget->chart, LV_ALIGN_TOP_LEFT, position.x, position.y);

    lv_chart_set_point_count(widget->chart, 100);

    widget->series =
        lv_chart_add_series(widget->chart, lv_color_hex(0xC00000), LV_CHART_AXIS_PRIMARY_Y);

    // Set the size of the points in the line chart
    lv_obj_set_style_pad_all(widget->chart, 0, LV_PART_INDICATOR);

    return widget;
}

esp_err_t widgetChartAddPoint(WidgetChart* widget, int32_t value) {
    if (!widget || !widget->series) return ESP_ERR_INVALID_STATE;

    lv_chart_set_next_value(widget->chart, widget->series, value);
    lv_chart_refresh(widget->chart);
    return ESP_OK;
}

esp_err_t widgetChartDestroy(WidgetChart* widget) {
    if (!widget) return ESP_ERR_INVALID_STATE;
    if (widget->chart) lv_obj_delete(widget->chart);
    if (widget->scale) lv_obj_delete(widget->scale);
    if (widget->title) lv_obj_delete(widget->title);
    free(widget);
    return ESP_OK;
}