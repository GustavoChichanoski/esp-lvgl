#include <stdlib.h>

#include "./screen_encoder.h"
#include "backend/devices/rotary_encoder.h"
#include "configs/project_types.h"
#include "core/lv_obj_tree.h"
#include "display/lv_display.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "frontend/widgets/widget_button.h"
#include "screen.h"
#include "widgets/label/lv_label.h"

const char* tag_screen_encoder = "[SCREEN ENCODER]";

esp_err_t screenEncoderCreate(ScreenInterface* screen);

esp_err_t screenEncoderInit(ScreenInterface** self, StackScreen* stack_screen) {
    ScreenInterface* new_screen = (ScreenInterface*)malloc(sizeof(ScreenInterface));
    if (!new_screen) {
        ESP_LOGE(tag_screen_encoder, "Failed to allocate memory for screen interface");
        return ESP_ERR_NO_MEM;
    }
    ScreenEncoder* screen_encoder = (ScreenEncoder*)malloc(sizeof(ScreenEncoder));
    if (!screen_encoder) {
        ESP_LOGE(tag_screen_encoder, "Failed to allocate memory for ScreenTemperature");
        return ESP_ERR_NO_MEM;
    }

    new_screen->create   = screenEncoderCreate;
    new_screen->draw     = screenEncoderDraw;
    new_screen->destroy  = screenEncoderDestroy;
    new_screen->previous = (stack_screen->current) ? stack_screen->current : NULL;
    new_screen->next     = NULL;

    screen_encoder->title        = NULL;
    screen_encoder->position     = NULL;
    screen_encoder->encoder      = encoderGet();
    screen_encoder->stack_screen = stack_screen;
    new_screen->context          = screen_encoder;

    esp_err_t error = screenEncoderCreate(new_screen);
    if (error) {
        ESP_LOGE(tag_screen_encoder, "Failed to create screen_temperature");
        screenEncoderDestroy(new_screen);
        return error;
    }
    *self = new_screen;
    return ESP_OK;
}

esp_err_t screenEncoderTitle(ScreenInterface* screen, lv_obj_t* parent, const char* text) {
    ScreenEncoder* self = (ScreenEncoder*)screen->context;
    self->title         = lv_label_create(parent);
    if (!self->title) {
        screenEncoderDestroy(screen);
        ESP_LOGE(tag_screen_encoder, "Failed to create title label");
        return ESP_ERR_NO_MEM;
    }

    lv_label_set_text(self->title, text);
    lv_obj_set_style_text_font(self->title, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_align(self->title, LV_ALIGN_TOP_MID, 0, 10);
    return ESP_OK;
}

esp_err_t screenEncoderPosition(ScreenInterface* screen, lv_obj_t* parent, Point2D offset) {
    ScreenEncoder* self = (ScreenEncoder*)screen->context;
    self->position      = lv_label_create(parent);
    if (!self->position) {
        screenEncoderDestroy(screen);
        ESP_LOGE(tag_screen_encoder, "Failed to create position label");
        return ESP_ERR_NO_MEM;
    }
    lv_label_set_text_fmt(self->position, "Position: %d", 0);
    lv_obj_set_style_text_font(self->position, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_align(self->position, LV_ALIGN_BOTTOM_MID, offset.x, offset.y);
    return ESP_OK;
}

esp_err_t screenEncoderCreate(ScreenInterface* screen) {
    WidgetButton* next_button = NULL;

    ScreenEncoder* self = (ScreenEncoder*)screen->context;
    if (!self) {
        ESP_LOGE(tag_screen_encoder, "ScreenTemperature is NULL during create");
        return ESP_ERR_NOT_FOUND;
    }
    lv_obj_t* screen_active = lv_screen_active();  // Use lv_scr_act() for active screen
    if (!screen_active) {
        screenEncoderDestroy(screen);
        ESP_LOGE(tag_screen_encoder, "Failed to get active screen");
        return ESP_ERR_INVALID_STATE;
    }

    ESP_ERROR_CHECK(screenEncoderTitle(screen, screen_active, "KY-040 Encoder"));

    esp_err_t error =
        widgetButtonCreate(&next_button, screen_active, "Back", NULL, (Point2D){0, 0});
    if (error) {
        screenEncoderDestroy(screen);
        ESP_LOGE(tag_screen_encoder, "Failed to create back button");
        return error;
    }
    self->next_button = next_button;

    ESP_ERROR_CHECK(screenEncoderPosition(screen, screen_active, (Point2D){0, -20}));
    return ESP_OK;
}

esp_err_t screenEncoderDraw(ScreenInterface* self) {
    ScreenEncoder* screen_encoder = self->context;
    if (!screen_encoder) {
        ESP_LOGE(tag_screen_encoder, "ScreenEncoder is NULL during draw");
        return ESP_ERR_INVALID_STATE;
    }

    EncoderFlags encoder_flags;
    if (pdFALSE == xQueueReceive(screen_encoder->encoder->on_change_position, &encoder_flags, 0)) {
        return ESP_OK;
    }
    return ESP_OK;
}

esp_err_t screenEncoderDestroy(ScreenInterface* self) {
    if (!self) return ESP_ERR_INVALID_STATE;
    ScreenEncoder* screen_encoder = self->context;

    if (!screen_encoder) return ESP_ERR_INVALID_STATE;
    if (screen_encoder->position) lv_obj_delete(screen_encoder->position);
    if (screen_encoder->title) lv_obj_delete(screen_encoder->title);
    if (screen_encoder->next_button) widgetButtonDestroy(screen_encoder->next_button);

    free(screen_encoder);
    return ESP_OK;
}
