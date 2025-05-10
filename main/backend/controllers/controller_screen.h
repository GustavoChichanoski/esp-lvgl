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

esp_err_t controllerScreenInit(StackScreen** stack_screen);
esp_err_t controllerScreenDraw(StackScreen* stack_screen);
esp_err_t controllerScreenPut(StackScreen* stack_screen, ScreenInterface* screen);
esp_err_t controllerScreenPop(StackScreen* stack_screen, ScreenInterface* screen);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif