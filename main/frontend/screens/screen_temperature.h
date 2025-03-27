#ifndef __SCREEN_TEMPERATURE_H__
#define __SCREEN_TEMPERATURE_H__

#include "esp_err.h"

#include "misc/lv_types.h"

#include "./screen.h"

typedef struct {
    lv_obj_t* label;
    lv_obj_t* thermometer;
} ScreenTemperature;

esp_err_t screen_temperature_init(ScreenInterface** self, ScreenInterface* previous);

#endif
