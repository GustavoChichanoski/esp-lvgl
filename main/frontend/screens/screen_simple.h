#ifndef __SCREEN_SIMPLE_H__
#define __SCREEN_SIMPLE_H__

#include "backend/controllers/controller_screen.h"
#include "esp_err.h"
#include "misc/lv_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_obj_t* title;
    StackScreen* stack_screen;
} ScreenSimple;

esp_err_t screen_simple_init(ScreenInterface** self, StackScreen* stack_screen);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
