#ifndef __CONTROLLER_SCREEN_H__
#define __CONTROLLER_SCREEN_H__

#include "esp_err.h"

#include "../../frontend/screens/screen.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ScreenInterface* current;
    ScreenInterface* head;
} StackScreen;

esp_err_t controller_screen_init(StackScreen** stack_screen);
esp_err_t controller_screen_draw(StackScreen* stack_screen);
esp_err_t controller_screen_put(StackScreen* stack_screen, ScreenInterface* screen);
esp_err_t controller_screen_pop(StackScreen* stack_screen, ScreenInterface* screen);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif