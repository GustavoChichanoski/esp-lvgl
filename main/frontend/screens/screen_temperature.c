#include "./screen_temperature.h"
#include "display/lv_display.h"
#include "esp_log.h"
#include "misc/lv_area.h"
#include "utils/logs.h"
#include "widgets/bar/lv_bar.h"

#define THERMOMETER_WIDTH  200
#define THERMOMETER_HEIGHT 20
#define TEMP_MIN           0
#define TEMP_MAX           100

const char* TAG_SCREEN_TEMPERATURE = "screen_temperature";

esp_err_t screen_temperature_destroy(ScreenInterface* self);
esp_err_t screen_temperature_draw(ScreenInterface* self);
esp_err_t screen_temperature_create(ScreenInterface* screen);

/**
 * @brief Create a thermometer widget.
 *
 * @param parent The parent LVGL object (e.g., the active screen).
 * @param temp   The initial temperature value.
 * @return lv_obj_t* Pointer to the created thermometer widget.
 */
lv_obj_t* create_thermometer(lv_obj_t* parent, int temp) {
    /* Create the bar widget */
    lv_obj_t* thermometer = lv_bar_create(parent);
    lv_obj_set_size(thermometer, THERMOMETER_WIDTH, THERMOMETER_HEIGHT);
    lv_obj_align(thermometer, LV_ALIGN_CENTER, 0, 0);

    /* Set the range for temperature values */
    lv_bar_set_range(thermometer, TEMP_MIN, TEMP_MAX);

    /* Optionally, customize styles for a thermometer look */
    lv_obj_set_style_bg_color(thermometer, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(thermometer, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(thermometer, lv_color_hex(0xFF0000), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(thermometer, LV_OPA_COVER, LV_PART_INDICATOR);

    /* Set the current temperature */
    lv_bar_set_value(thermometer, temp, LV_ANIM_ON);

    return thermometer;
}

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
esp_err_t screen_temperature_init(ScreenInterface** self, ScreenInterface* previous) {
    ScreenInterface* new_screen = (ScreenInterface*)malloc(sizeof(ScreenInterface));
    if (NULL == new_screen) {
        ESP_LOGE(TAG_SCREEN_TEMPERATURE, "Failed to allocate memory for screen interface");
        return ESP_ERR_NO_MEM;
    }
    new_screen->create   = screen_temperature_create;
    new_screen->draw     = screen_temperature_draw;
    new_screen->destroy  = screen_temperature_destroy;
    new_screen->previous = previous;
    new_screen->next     = NULL;

    ScreenTemperature* screen_temperature = (ScreenTemperature*)malloc(sizeof(ScreenTemperature));
    if (NULL == screen_temperature) {
        ESP_LOGE(TAG_SCREEN_TEMPERATURE, "Failed to allocate memory for ScreenTemperature");
        return ESP_ERR_NO_MEM;
    }
    new_screen->context = screen_temperature;
    esp_err_t error     = screen_temperature_create(new_screen);
    if (ESP_OK != error) {
        ESP_LOGE(TAG_SCREEN_TEMPERATURE, "Failed to create screen_temperature");
        screen_temperature_destroy(new_screen);
        return error;
    }
    *self = new_screen;
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
    self->thermometer = create_thermometer(screen_active, 50);
    if (NULL == self->thermometer) {
        ESP_LOGE(TAG_SCREEN_TEMPERATURE, "Failed to create thermometer");
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

esp_err_t screen_temperature_draw(ScreenInterface* self) { return ESP_OK; }

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
    if (NULL == screen_temperature) {
        log_error_lv_del(TAG_SCREEN_TEMPERATURE, "ScreenTemperature");
        return ESP_ERR_INVALID_STATE;
    }
    if (NULL == screen_temperature->label) {
        log_error_lv_del(TAG_SCREEN_TEMPERATURE, "Label");
        return ESP_ERR_NOT_FOUND;
    }
    if (NULL == screen_temperature->thermometer) {
        log_error_lv_del(TAG_SCREEN_TEMPERATURE, "Thermometer");
        return ESP_ERR_NOT_FOUND;
    }

    lv_obj_delete(screen_temperature->thermometer);
    lv_obj_delete(screen_temperature->label);
    widget_button_destroy(screen_temperature->button);
    free(screen_temperature);
    return ESP_OK;
}
