#include "./screen_simple.h"
#include "display/lv_display.h"
#include "widgets/label/lv_label.h"

esp_err_t screen_simple_init(ScreenInterface** self, StackScreen* stack_screen) {
    ScreenSimple* screen = calloc(1, sizeof(ScreenSimple));
    if (screen == NULL) return ESP_FAIL;

    screen->stack_screen = stack_screen;

    *self = (ScreenInterface*)screen;

    screen->title = lv_label_create(lv_screen_active());
    lv_label_set_text(screen->title, "Screen Simple");

    return ESP_OK;
}

esp_err_t screen_simple_destroy(ScreenInterface* self) {
    ScreenSimple* screen = (ScreenSimple*)self;
    lv_obj_delete(screen->title);
    free(screen);
    return ESP_OK;
}