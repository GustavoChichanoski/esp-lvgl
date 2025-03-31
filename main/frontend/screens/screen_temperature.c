#include "./screen_temperature.h"
#include "backend/controllers/controller_screen.h"
#include "core/lv_obj.h"
#include "display/lv_display.h"
#include "esp_log.h"
#include "frontend/widgets/widget_thermometer.h"
#include "misc/lv_area.h"
#include "utils/logs.h"
#include "widgets/label/lv_label.h"

const char* TAG_SCREEN_TEMPERATURE = "[SCREEN TEMPERATURE]";

uint8_t temperature = 0;

/**
 * @brief Create a new screen interface and initialize it.
 *
 * This function allocates memory for a new screen interface and
 * initializes it with the create, draw, and destroy functions.
 * It also allocates memory for the screen-specific data and
 * initializes it using the create function.
 *
 * @param self Pointer to the new screen interface to be created.
 * @param previous Pointer to the previous screen interface in the chain.
 *
 * @return esp_err_t ESP_OK on success or an appropriate error code on failure.
 *                   ESP_ERR_NO_MEM if memory allocation failed.
 *                   ESP_ERR_INVALID_STATE if the screen is invalid.
 */
esp_err_t screen_temperature_init(ScreenInterface** self, StackScreen* stack_screen) {
    ScreenInterface* new_screen = (ScreenInterface*)malloc(sizeof(ScreenInterface));
    if (!new_screen) {
        ESP_LOGE(TAG_SCREEN_TEMPERATURE, "Failed to allocate memory for screen interface");
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(TAG_SCREEN_TEMPERATURE, "Initializing screen_temperature");
    new_screen->create   = screen_temperature_create;
    new_screen->draw     = screen_temperature_draw;
    new_screen->destroy  = screen_temperature_destroy;
    new_screen->previous = stack_screen->current;
    new_screen->next     = NULL;

    ScreenTemperature* screen_temperature = (ScreenTemperature*)malloc(sizeof(ScreenTemperature));
    if (!screen_temperature) {
        ESP_LOGE(TAG_SCREEN_TEMPERATURE, "Failed to allocate memory for ScreenTemperature");
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(TAG_SCREEN_TEMPERATURE, "ScreenTemperature initialized");

    new_screen->context = screen_temperature;
    esp_err_t error = screen_temperature_create(new_screen);
    if (error) {
        ESP_LOGE(TAG_SCREEN_TEMPERATURE, "Failed to create screen_temperature");
        screen_temperature_destroy(new_screen);
        return error;
    }
    ESP_LOGI(TAG_SCREEN_TEMPERATURE, "ScreenTemperature created");

    new_screen->context = screen_temperature;
    *self               = new_screen;
    return ESP_OK;
}

/**
 * @brief Create the screen_temperature screen.
 *
 * This function initializes a new screen_temperature screen interface.
 *
 * @param self The screen_temperature screen interface to be created.
 *
 * @return esp_err_t ESP_OK on success or an appropriate error code on failure.
 *                   ESP_ERR_NO_MEM if memory allocation failed.
 *                   ESP_ERR_INVALID_STATE if the screen is invalid.
 */
esp_err_t screen_temperature_create(ScreenInterface* screen) {
    ScreenTemperature* self = (ScreenTemperature*)screen->context;
    lv_obj_t* screen_active = lv_screen_active();  // Use lv_scr_act() for active screen
    if (NULL == screen_active) return ESP_ERR_INVALID_STATE;

    // Set the background color using a defined macro or constant
    const lv_color_t bg_color = lv_color_hex(0x003a57);
    lv_obj_set_style_bg_color(screen_active, bg_color, LV_PART_MAIN);

    // Create the label and check for success
    self->label = lv_label_create(screen_active);
    if (NULL == self->label) {
        ESP_LOGE(TAG_SCREEN_TEMPERATURE, "Failed to create label");
        return ESP_ERR_NO_MEM;
    }

    lv_label_set_text(self->label, "Hello world");
    lv_obj_set_style_text_color(self->label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(self->label, LV_ALIGN_CENTER, 0, 0);

    // Create the thermometer and check for success
    widget_thermometer_create(&self->thermometer, screen_active, 99);
    if (!self->thermometer) {
        ESP_LOGE(TAG_SCREEN_TEMPERATURE, "Failed to create thermometer");
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

esp_err_t screen_temperature_draw(ScreenInterface* self) {
    ScreenTemperature *screen_temperature = self->context;
    if (!screen_temperature) {
        return ESP_ERR_INVALID_STATE;
    }
    temperature = (temperature + 1) % 101;
    widget_thermometer_set_temperature(screen_temperature->thermometer, temperature);
    return ESP_OK;
}

/**
 * @brief Destroy the screen_temperature screen.
 *
 * This function releases resources associated with the screen_temperature screen.
 *
 * @param self The screen interface to be destroyed.
 *
 * @return esp_err_t ESP_OK on success or an appropriate error code on failure.
 *                   ESP_ERR_INVALID_STATE if the screen interface is invalid.
 *                   ESP_ERR_NOT_FOUND if the label is not found.
 */
esp_err_t screen_temperature_destroy(ScreenInterface* self) {
    ScreenTemperature* screen_temperature = self->context;
    if (!screen_temperature) {
        log_error_lv_del(TAG_SCREEN_TEMPERATURE, "ScreenTemperature");
        return ESP_ERR_INVALID_STATE;
    }
    if (!screen_temperature->label) lv_obj_delete(screen_temperature->label);
    if (!screen_temperature->thermometer)
        widget_thermometer_destroy(screen_temperature->thermometer);
    free(screen_temperature);
    return ESP_OK;
}
