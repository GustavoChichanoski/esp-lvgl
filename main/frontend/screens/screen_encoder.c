#include <stdlib.h>

#include "./screen_encoder.h"
#include "core/lv_obj_tree.h"
#include "display/lv_display.h"
#include "esp_log.h"
#include "frontend/widgets/widget_button.h"
#include "utils/logs.h"
#include "widgets/label/lv_label.h"

const char* TAG_SCREEN_ENCODER = "screen_encoder";

esp_err_t screen_encoder_create(ScreenInterface* screen);
esp_err_t screen_encoder_destroy(ScreenInterface* self);
esp_err_t screen_encoder_draw(ScreenInterface* self);

esp_err_t screen_encoder_init(ScreenInterface** self, StackScreen* stack_screen) {
    ScreenInterface* new_screen = (ScreenInterface*)malloc(sizeof(ScreenInterface));
    if (NULL == new_screen) {
        ESP_LOGE(TAG_SCREEN_ENCODER, "Failed to allocate memory for screen interface");
        return ESP_ERR_NO_MEM;
    }
    new_screen->create   = screen_encoder_create;
    new_screen->draw     = screen_encoder_draw;
    new_screen->destroy  = screen_encoder_destroy;
    new_screen->previous = stack_screen->current;
    new_screen->next     = NULL;

    ScreenEncoder* screen_encoder = (ScreenEncoder*)malloc(sizeof(ScreenEncoder));
    screen_encoder->encoder       = NULL;
    screen_encoder->title         = NULL;
    screen_encoder->position      = NULL;
    screen_encoder->stack_screen  = stack_screen;
    new_screen->context           = screen_encoder;

    if (NULL == screen_encoder) {
        ESP_LOGE(TAG_SCREEN_ENCODER, "Failed to allocate memory for ScreenTemperature");
        return ESP_ERR_NO_MEM;
    }
    esp_err_t error = screen_encoder_create(new_screen);
    if (ESP_OK != error) {
        ESP_LOGE(TAG_SCREEN_ENCODER, "Failed to create screen_temperature");
        screen_encoder_destroy(new_screen);
        return error;
    }
    *self = new_screen;
    return ESP_OK;
}

esp_err_t screen_encoder_create(ScreenInterface* screen) {
    WidgetButton* next_button = NULL;

    ScreenEncoder* self = (ScreenEncoder*)screen->context;
    if (NULL == self) {
        ESP_LOGE(TAG_SCREEN_ENCODER, "ScreenTemperature is NULL during create");
        return ESP_ERR_NOT_FOUND;
    }
    lv_obj_t* screen_active = lv_screen_active();  // Use lv_scr_act() for active screen
    if (NULL == screen_active) {
        ESP_LOGE(TAG_SCREEN_ENCODER, "Failed to get active screen");
        return ESP_ERR_INVALID_STATE;
    }

    self->title = lv_label_create(screen_active);
    if (NULL == self->title) {
        ESP_LOGE(TAG_SCREEN_ENCODER, "Failed to create title label");
        return ESP_ERR_NO_MEM;
    }

    lv_label_set_text(self->title, "KY-040 Encoder");
    lv_obj_set_style_text_font(self->title, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_align(self->title, LV_ALIGN_TOP_MID, 0, 10);

    esp_err_t error = widget_button_create(&next_button, screen_active, "Back", NULL);
    if (ESP_OK != error) {
        ESP_LOGE(TAG_SCREEN_ENCODER, "Failed to create back button");
        return error;
    }
    self->next_button = next_button;
    self->position    = lv_label_create(screen_active);
    if (NULL == self->position) {
        ESP_LOGE(TAG_SCREEN_ENCODER, "Failed to create position label");
        return ESP_ERR_NO_MEM;
    }
    lv_label_set_text_fmt(self->position, "Position: %d", self->encoder->position);
    lv_obj_align(self->position, LV_ALIGN_CENTER, 0, -20);

    lv_obj_set_style_text_font(self->position, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_align(self->position, LV_ALIGN_BOTTOM_MID, 0, -10);

    return ESP_OK;
}

esp_err_t screen_encoder_draw(ScreenInterface* self) {
    ScreenEncoder* screen_encoder = self->context;
    if (NULL == screen_encoder) {
        ESP_LOGE(TAG_SCREEN_ENCODER, "ScreenEncoder is NULL during draw");
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}

esp_err_t screen_encoder_destroy(ScreenInterface* self) {
    ScreenEncoder* screen_encoder = self->context;
    if (NULL == screen_encoder) {
        log_error_lv_del(TAG_SCREEN_ENCODER, "ScreenEncoder");
        return ESP_ERR_INVALID_STATE;
    }
    if (NULL == screen_encoder->position) {
        log_error_lv_del(TAG_SCREEN_ENCODER, "position");
        return ESP_ERR_NOT_FOUND;
    }
    lv_obj_delete(screen_encoder->position);
    if (NULL == screen_encoder->title) {
        log_error_lv_del(TAG_SCREEN_ENCODER, "Title");
        return ESP_ERR_NOT_FOUND;
    }
    lv_obj_delete(screen_encoder->title);
    free(screen_encoder);
    return ESP_OK;
}
