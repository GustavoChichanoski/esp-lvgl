#include "backend/devices/rotary_encoder.h"

// Esp-IDF includes
#include "driver/gpio.h"
#include "esp_timer.h"

// Project includes
#include "configs/pinout.h"

const char kTagEncoder[] = "[ENCODER]";
static Encoder encoder;

/**
 * @brief Get the pointer to the Encoder struct
 *
 * @return A pointer to the Encoder struct that contains the state of the
 * encoder.
 *
 * This function returns a pointer to the Encoder struct that contains the
 * state of the encoder. The user can use this pointer to access the position and
 * direction of the encoder.
 */
Encoder* encoderGet() { return &encoder; }

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

/**
 * @brief Read the current position of the encoder
 * @param encoder Pointer to the Encoder struct that contains the state of the
 * encoder
 * @return The current position of the encoder
 *
 * This function reads the current state of the encoder and returns its
 * position. It also updates the position and direction of the encoder in the
 * struct.
 *
 */
void encoderUpdatePosition(void* arg) {
    Encoder* encoder = (Encoder*)arg;
    int32_t b_level  = gpio_get_level(ENCODER_B);
    if (b_level) {
        encoder->direction = ENCODER_ROTATE_CLOCKWISE;
        encoder->position  = encoderIncrement(encoder->position);
    } else {
        encoder->direction = ENCODER_ROTATE_ANTI_CLOCKWISE;
        encoder->position  = encoderDecrement(encoder->position);
    }
    int64_t now        = esp_timer_get_time();
    encoder->velocity  = (uint32_t)((now - encoder->last_time) / 1000);
    encoder->last_time = now;
}

void encoderInit(void* arg) {
    (void)arg;
    encoder.position      = 0;
    encoder.last_position = 0;
    encoder.direction     = 0;
    encoder.last_a_level  = 0;
    encoder.last_b_level  = 0;
    encoder.last_time     = 0;
    encoder.velocity      = 0;

    gpio_config_t encoder_config = {.pull_down_en = GPIO_PULLDOWN_DISABLE,
                                    .pull_up_en   = GPIO_PULLUP_ENABLE,
                                    .mode         = GPIO_MODE_INPUT,
                                    .intr_type    = GPIO_INTR_POSEDGE,
                                    .pin_bit_mask = (1ULL << ENCODER_A)};
    ESP_ERROR_CHECK(gpio_config(&encoder_config));
    gpio_config_t encoder_b_config = {.pull_down_en = GPIO_PULLDOWN_DISABLE,
                                      .pull_up_en   = GPIO_PULLUP_ENABLE,
                                      .mode         = GPIO_MODE_INPUT,
                                      .intr_type    = GPIO_INTR_DISABLE,
                                      .pin_bit_mask = (1ULL << ENCODER_B)};
    ESP_ERROR_CHECK(gpio_config(&encoder_b_config));
    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_HIGH));
    ESP_ERROR_CHECK(gpio_isr_handler_add(ENCODER_A, encoderUpdatePosition, &encoder));
}