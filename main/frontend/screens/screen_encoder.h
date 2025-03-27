#ifndef __SCREEN_ENCODER_H__
#define __SCREEN_ENCODER_H__

#include "./screen.h"
#include "backend/controllers/controller_screen.h"
#include "configs/project_types.h"
#include "esp_err.h"
#include "frontend/widgets/widget_button.h"
#include "misc/lv_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_obj_t* title;
    lv_obj_t* position;
    Encoder* encoder;
    WidgetButton* next_button;
    StackScreen* stack_screen;
} ScreenEncoder;

#ifdef __cplusplus
} /*extern "C"*/
#endif

esp_err_t screen_encoder_init(ScreenInterface** self, StackScreen* stack_screen);
#endif
