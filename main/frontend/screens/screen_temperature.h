#ifndef __SCREEN_TEMPERATURE_H__
#define __SCREEN_TEMPERATURE_H__

#include "backend/controllers/controller_screen.h"
#include "esp_err.h"

#include "frontend/widgets/widget_thermometer.h"
#include "misc/lv_types.h"

#include "./screen.h"

typedef struct {
    lv_obj_t* label;
    WidgetThermometer* thermometer;
    StackScreen* stack_screen;
} ScreenTemperature;

esp_err_t screen_temperature_init(ScreenInterface** self, StackScreen* stack_screen);
esp_err_t screen_temperature_destroy(ScreenInterface* self);
esp_err_t screen_temperature_draw(ScreenInterface* self);
esp_err_t screen_temperature_create(ScreenInterface* screen);

#endif
