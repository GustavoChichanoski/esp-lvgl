#include "./screen_temperature.h"
#include "backend/controllers/controller_screen.h"
#include "configs/project_types.h"
#include "core/lv_obj.h"
#include "display/lv_display.h"
#include "esp_log.h"
#include "frontend/widgets/widget_thermometer.h"
#include "misc/lv_area.h"
#include "utils/logs.h"
#include "widgets/label/lv_label.h"

const char* tag_screen_temperature = "[SCREEN TEMPERATURE]";

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
esp_err_t screenTemperatureInit(ScreenInterface** self, StackScreen* stack_screen) {
    ScreenInterface* new_screen = (ScreenInterface*)malloc(sizeof(ScreenInterface));
    if (!new_screen) {
        ESP_LOGE(tag_screen_temperature, "Failed to allocate memory for screen interface");
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(tag_screen_temperature, "Initializing screen_temperature");
    new_screen->create   = screenTemperatureCreate;
    new_screen->draw     = screenTemperatureDraw;
    new_screen->destroy  = screenTemperatureDestroy;
    new_screen->previous = stack_screen->current;
    new_screen->next     = NULL;

    ScreenTemperature* screen_temperature = (ScreenTemperature*)malloc(sizeof(ScreenTemperature));
    if (!screen_temperature) {
        ESP_LOGE(tag_screen_temperature, "Failed to allocate memory for ScreenTemperature");
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(tag_screen_temperature, "ScreenTemperature initialized");
    new_screen->context = screen_temperature;

    esp_err_t error = screenTemperatureCreate(new_screen);
    if (error) {
        ESP_LOGE(tag_screen_temperature, "Failed to create screen_temperature");
        screenTemperatureDestroy(new_screen);
        return error;
    }
    ESP_LOGI(tag_screen_temperature, "ScreenTemperature created");

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
esp_err_t screenTemperatureCreate(ScreenInterface* screen) {
    ScreenTemperature* self = (ScreenTemperature*)screen->context;
    lv_obj_t* screen_active = lv_screen_active();  // Use lv_scr_act() for active screen
    if (NULL == screen_active) return ESP_ERR_INVALID_STATE;

    // Set the background color using a defined macro or constant
    const lv_color_t bg_color = lv_color_hex(0x003a57);
    lv_obj_set_style_bg_color(screen_active, bg_color, LV_PART_MAIN);

    // Create the label and check for success
    self->label = lv_label_create(screen_active);
    if (NULL == self->label) {
        ESP_LOGE(tag_screen_temperature, "Failed to create label");
        return ESP_ERR_NO_MEM;
    }

    lv_label_set_text(self->label, "Hello world");
    lv_obj_set_style_text_color(self->label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(self->label, LV_ALIGN_CENTER, 0, 0);

    Point2D offset = {10, 10};

    // Create the thermometer and check for success
    widgetThermometerCreate(&self->thermometer, screen_active, 99, offset);
    if (!self->thermometer) {
        ESP_LOGE(tag_screen_temperature, "Failed to create thermometer");
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

/**
 * @brief Update the screen_temperature screen with the current temperature.
 *
 * This function is the draw function for the screen_temperature screen interface.
 * It updates the thermometer widget with the current temperature. The temperature
 * is incremented by 1 each time the screen is drawn.
 *
 * @param self The screen_temperature screen interface to be drawn.
 *
 * @return esp_err_t ESP_OK on success or an appropriate error code on failure.
 *                   ESP_ERR_INVALID_STATE if the screen is invalid.
 */
esp_err_t screenTemperatureDraw(ScreenInterface* self) {
    ScreenTemperature* screen_temperature = self->context;
    if (!screen_temperature) { return ESP_ERR_INVALID_STATE; }
    temperature = (temperature + 1) % 101;
    widgetThermometerSetTemperature(screen_temperature->thermometer, temperature);
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
esp_err_t screenTemperatureDestroy(ScreenInterface* self) {
    ScreenTemperature* screen_temperature = self->context;
    if (!screen_temperature) {
        log_error_lv_del(tag_screen_temperature, "ScreenTemperature");
        return ESP_ERR_INVALID_STATE;
    }
    if (!screen_temperature->label) lv_obj_delete(screen_temperature->label);
    if (!screen_temperature->thermometer) widgetThermometerDestroy(screen_temperature->thermometer);
    free(screen_temperature);
    return ESP_OK;
}
