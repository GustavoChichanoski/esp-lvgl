// Standard includes
#include <stdbool.h>
#include <stdint.h>

// Esp-IDF includes
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"

// FreeRTOS includes
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "portmacro.h"

// Project includes
#include "backend/devices/button.h"
#include "configs/pinout.h"
#include "utils/utils_macros.h"

#define DEBOUNCE_TIME_MS (50)

/**
 * @brief GPIO interrupt service routine for button events.
 *
 * Resets the debounce timer and potentially wakes the button task.
 *
 * @param arg Pointer to the Button structure containing the debounce timer
 *            and task handle.
 */
static void IRAM_ATTR gpioIsrHandler(void* arg) {
    Button* button        = (Button*)arg;
    BaseType_t task_woken = pdFALSE;

    xTimerResetFromISR(button->debounce_timer, &task_woken);

    if (task_woken == pdTRUE) portYIELD_FROM_ISR();
}

/**
 * @brief Add a listener to the button's event callbacks.
 *
 * @param button Button structure containing a list of event callbacks.
 * @param event_cb Callback to be called when a button event occurs.
 *
 * @return esp_err_t ESP_OK on success or ESP_ERR_NO_MEM if the listener list is full.
 */
esp_err_t buttonAddListener(Button* button, button_event_cb event_cb) {
    if (button->listeners > 9) return ESP_ERR_NO_MEM;
    button->on_event[button->listeners] = event_cb;
    button->listeners++;
    return ESP_OK;
}

/**
 * @brief Remove a listener from the button's event callbacks.
 *
 * This function searches for the specified event callback in the button's
 * list of registered listeners and removes it if found. The list of listeners
 * is then compacted to fill the gap.
 *
 * @param button Pointer to the Button structure containing the list of listeners.
 * @param event_cb The event callback function to be removed from the list.
 *
 * @return esp_err_t ESP_OK if the listener was successfully removed or
 * ESP_ERR_NOT_FOUND if the listener was not found in the list.
 */
esp_err_t buttonRemoveListener(Button* button, button_event_cb event_cb) {
    for (int listener = 0; listener < button->listeners; listener++) {
        if (button->on_event[listener] != event_cb) continue;
        button->on_event[listener] = NULL;
        button->listeners--;
        return ESP_OK;
    }
    return ESP_ERR_NOT_FOUND;
}

/**
 * @brief Notify registered listeners about button events.
 *
 * This function iterates over the registered listeners and calls their
 * respective event callbacks, passing the specified event as an argument.
 *
 * @param button Button object containing the list of registered listeners.
 * @param event Button event to be notified to the listeners.
 *
 * @return esp_err_t ESP_OK on success or an appropriate error code on failure.
 */
esp_err_t buttonNotify(Button* button, ButtonEvent event) {
    for (uint8_t listener = 0; listener < button->listeners; listener++) {
        if (button->on_event[listener] == NULL) continue;
        button->on_event[listener](event);
    }
    return ESP_OK;
}

/**
 * @brief Timer callback to debounce button events.
 *
 * This callback is called when the debouncing timer expires. It checks the
 * current state of the button and if it is pressed, sets the button state and
 * notifies registered listeners about the button press event. If the button is
 * released, it notifies listeners about the button release event.
 *
 * @param xTimer Timer handle that triggered this callback.
 */
static void debounceTimerCallback(TimerHandle_t xTimer) {
    Button* button = (Button*)pvTimerGetTimerID(xTimer);
    if (NULL == button || 1 > button->listeners) return;

    int level = gpio_get_level(button->pin);

    if (1 == level && false == button->pressed) return;
    if (0 == level && false == button->pressed) {
        button->pressed            = true;
        button->pressed_start_time = xTaskGetTickCount();
        ESP_LOGI("BUTTON", "Button pressed");
        buttonNotify(button, kButtonEventPressed);
        return;
    }

    button->pressed         = false;
    uint32_t press_duration = xTaskGetTickCount() - button->pressed_start_time;
    if (press_duration > pdMS_TO_TICKS(1000)) {
        ESP_LOGI("BUTTON", "Button long pressed");
        buttonNotify(button, kButtonEventLongPressed);
        return;
    }

    ESP_LOGI("BUTTON", "Button released");
    buttonNotify(button, kButtonEventReleased);
}

/**
 * @brief Task to handle button events, including debouncing and event notifications.
 *
 * This task initializes the GPIO configuration for the button pin, sets up a debounce timer,
 * and installs an ISR handler for the button. It processes button press and release events
 * by calling registered event callbacks.
 *
 * @param args Pointer to a Button structure containing button configuration and state.
 */
void buttonTask(void* args) {
    Button* button = (Button*)args;

    gpio_config_t io_config = {.pin_bit_mask = PIN_SELECT(ENCODER_BUTTON),
                               .mode         = GPIO_MODE_INPUT,
                               .pull_down_en = GPIO_PULLDOWN_DISABLE,
                               .pull_up_en   = GPIO_PULLUP_ENABLE,
                               .intr_type    = GPIO_INTR_POSEDGE};

    ESP_ERROR_CHECK(gpio_config(&io_config));

    button->debounce_timer = xTimerCreate("button_debounce_timer", pdMS_TO_TICKS(DEBOUNCE_TIME_MS),
                                          pdFALSE, (void*)button, debounceTimerCallback);

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    gpio_isr_handler_add(button->pin, gpioIsrHandler, button);
    vTaskSuspend(NULL);
}