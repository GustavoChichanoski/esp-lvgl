#include "./rotary_encoder.h"

#include "driver/gpio.h"
#include "esp_timer.h"

#include "configs/pinout.h"

const char* TAG_ENCODER = "[ENCODER]";
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
Encoder* get_encoder() { return &encoder; }

static uint8_t increment_encoder(uint8_t position) {
    return (0xFF == position) ? 0xFF : position + 1;
}

static uint8_t decrement_encoder(uint8_t position) { return (position) ? position - 1 : 0; }

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
void update_encoder_position(void* arg) {
    Encoder* encoder = (Encoder*)arg;
    int32_t b_level  = gpio_get_level(ENCODER_B);
    if (b_level) {
        encoder->direction = ENCODER_ROTATE_CLOCKWISE;
        encoder->position  = increment_encoder(encoder->position);
    } else {
        encoder->direction = ENCODER_ROTATE_ANTI_CLOCKWISE;
        encoder->position  = decrement_encoder(encoder->position);
    }
    int64_t now        = esp_timer_get_time();
    encoder->velocity  = (uint32_t)((now - encoder->last_time) / 1000);
    encoder->last_time = now;
}

void encoder_init(void* arg) {
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
    ESP_ERROR_CHECK(gpio_isr_handler_add(ENCODER_A, update_encoder_position, &encoder));
}