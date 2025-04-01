#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "freertos/idf_additions.h"
#include "soc/gpio_num.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    kButtonEventNone = 0,
    kButtonEventPressed,
    kButtonEventLongPressed,
    kButtonEventReleased,
} ButtonEvent;

typedef void (*button_event_cb)(ButtonEvent event);

typedef struct {
    gpio_num_t pin;
    uint8_t pressed      : 1;
    uint8_t step_pressed : 7;
    uint32_t pressed_start_time;
    uint8_t listeners;
    button_event_cb on_event[10];
    TimerHandle_t debounce_timer;
} Button;

/**
 * @brief Add a listener to the button's event callbacks.
 *
 * @param button Button structure containing a list of event callbacks.
 * @param event_cb Callback to be called when a button event occurs.
 *
 * @return esp_err_t ESP_OK on success or ESP_ERR_NO_MEM if the listener list is full.
 */
esp_err_t buttonAddListener(Button* button, button_event_cb event_cb);

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
esp_err_t buttonRemoveListener(Button* button, button_event_cb event_cb);

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
esp_err_t buttonNotify(Button* button, ButtonEvent event);

/**
 * @brief Task to handle button events, including debouncing and event notifications.
 *
 * This task initializes the GPIO configuration for the button pin, sets up a debounce timer,
 * and installs an ISR handler for the button. It processes button press and release events
 * by calling registered event callbacks.
 *
 * @param args Pointer to a Button structure containing button configuration and state.
 */
void buttonTask(void* args);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
