#ifndef __SCREEN_TEMPERATURE_H__
#define __SCREEN_TEMPERATURE_H__

#include "backend/controllers/controller_screen.h"
#include "esp_err.h"

#include "frontend/widgets/widget_button.h"
#include "frontend/widgets/widget_thermometer.h"
#include "misc/lv_types.h"

#include "./screen.h"

typedef struct {
    lv_obj_t* label;
    WidgetThermometer* thermometer;
    WidgetButton* next_button;
    StackScreen* stack_screen;
} ScreenTemperature;

esp_err_t screenTemperatureInit(ScreenInterface** self, StackScreen* stack_screen);
esp_err_t screenTemperatureDestroy(ScreenInterface* self);
esp_err_t screenTemperatureDraw(ScreenInterface* self);
esp_err_t screenTemperatureCreate(ScreenInterface* screen);

#endif
