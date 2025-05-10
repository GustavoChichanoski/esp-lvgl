// Standard includes
#include <stdbool.h>
#include <stdint.h>

// Esp-IDF includes
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "hal/gpio_types.h"

// Project includes
#include "backend/devices/rotary_encoder.h"
#include "configs/pinout.h"
#include "configs/project_types.h"
#include "portmacro.h"
#include "utils/utils_macros.h"

// FreeRTOS includes
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"

#define ALARM_COUNT_MS (100)
const char kTagEncoder[] = "[ENCODER]";

static Encoder* encoder;

/**
 * @brief Increment the position of the encoder by one
 *
 * @param position The current position of the encoder
 * @return The new position of the encoder
 *
 * This function increments the position of the encoder by one. If the
 * position is already at its maximum value (0xFF), the function returns
 * 0xFF. Otherwise, the function returns the incremented position.
 */
static uint8_t encoderIncrement(uint8_t position) {
    return (0xFF == position) ? 0xFF : position + 1;
}

static uint8_t encoderDecrement(uint8_t position) { return (position) ? position - 1 : 0; }

Encoder* encoderGet() { return encoder; }
Encoder* encoderSet(Encoder* new_encoder) { return encoder = new_encoder; }

bool encoderFlagsUpdate(Encoder* encoder, uint64_t now) {
    int32_t a_level = gpio_get_level(ENCODER_A);
    if (a_level == encoder->flags.last_a_level) return false;
    encoder->flags.last_a_level = a_level;

    int32_t b_level    = gpio_get_level(ENCODER_B);
    uint32_t direction = 0;
    direction = (b_level != a_level) ? ENCODER_ROTATE_CLOCKWISE : ENCODER_ROTATE_ANTI_CLOCKWISE;
    if (direction != encoder->flags.direction) {
        encoder->flags.direction = direction;
        return true;
    }

    encoder->flags.position = (encoder->flags.direction == ENCODER_ROTATE_CLOCKWISE)
                                  ? encoderIncrement(encoder->flags.position)
                                  : encoderDecrement(encoder->flags.position);
    encoder->flags.velocity = now - encoder->last_time;
    encoder->last_time      = now;
    BaseType_t task_woken   = pdFALSE;
    xQueueSendFromISR(encoder->on_change_position, &encoder->flags, &task_woken);
    return pdTRUE == task_woken;
}

static void IRAM_ATTR gpioIsrHandler(void* arg) {
    Encoder* encoder = (Encoder*)arg;
    encoderFlagsUpdate(encoder, esp_timer_get_time());
    gpio_isr_handler_add(ENCODER_A, gpioIsrHandler, encoder);
    gpio_intr_enable(ENCODER_A);
}

/**
 * @brief Initialize the rotary encoder
 *
 * This function initializes the rotary encoder by setting all the initial
 * values of the Encoder struct to 0. It also sets up the interrupts for the
 * encoder pins.
 *
 * @param arg Not used
 */
void encoderTask(void* args) {
    Encoder* encoder = (Encoder*)args;
    encoderSet(encoder);

    encoder->flags.position     = 0;
    encoder->flags.direction    = 0;
    encoder->flags.last_a_level = 0;
    encoder->flags.last_b_level = 0;
    encoder->flags.velocity     = 0;

    encoder->last_time          = 0;
    encoder->on_change_position = xQueueCreate(1, sizeof(EncoderFlags));

    gpio_config_t encoder_config = {.pull_down_en = GPIO_PULLDOWN_DISABLE,
                                    .pull_up_en   = GPIO_PULLUP_ENABLE,
                                    .mode         = GPIO_MODE_INPUT,
                                    .intr_type    = GPIO_INTR_DISABLE,
                                    .pin_bit_mask =
                                        PIN_SELECT(ENCODER_BUTTON) | PIN_SELECT(ENCODER_B)};
    ESP_ERROR_CHECK(gpio_config(&encoder_config));

    gpio_config_t encoder_a_pin = {.pull_down_en = GPIO_PULLDOWN_DISABLE,
                                   .pull_up_en   = GPIO_PULLUP_ENABLE,
                                   .mode         = GPIO_MODE_INPUT,
                                   .intr_type    = GPIO_INTR_ANYEDGE,
                                   .pin_bit_mask = PIN_SELECT(ENCODER_A)};
    ESP_ERROR_CHECK(gpio_config(&encoder_a_pin));

    gpio_install_isr_service(0);
    gpio_isr_handler_add(ENCODER_A, gpioIsrHandler, encoder);
    gpio_intr_enable(ENCODER_A);

    while (true) {
        // encoderFlagsUpdate(encoder, 100);
        ESP_LOGI(kTagEncoder, "Position: %03d | Direction: %01d, Velocity: %04d",
                 encoder->flags.position, encoder->flags.direction, encoder->flags.velocity);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}