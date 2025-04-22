#ifndef __PROJECT_TYPES_H__
#define __PROJECT_TYPES_H__

#include <stdint.h>

/// IDF
#include "driver/i2c_types.h"

/// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/semphr.h"

typedef struct {
    uint32_t direction    : 1;
    uint32_t last_a_level : 1;
    uint32_t last_b_level : 1;
    uint32_t button       : 5;
    uint32_t position     : 8;
    uint32_t velocity     : 16;
} EncoderFlags;

typedef enum { kNone = 0, kPressed = 1, kReleased = 2, kLongPressed = 3 } ButtonEvent;

typedef void (*encoder_event_cb)(EncoderFlags event);

typedef struct {
    EncoderFlags flags;
    uint64_t last_time;
    QueueHandle_t on_change_position;
} Encoder;

typedef union {
    struct {
        uint16_t os        : 1;
        uint16_t mux       : 3;
        uint16_t pga       : 3;
        uint16_t mode      : 1;
        uint16_t dr        : 3;
        uint16_t comp_mode : 1;
        uint16_t comp_pol  : 1;
        uint16_t comp_lat  : 1;
        uint16_t comp_que  : 2;
    } as_bits;
    uint16_t flags;
} AdcFlags;

typedef struct {
    uint16_t temperature;
    uint8_t address;
} SensorTemperature;

typedef struct {
    Encoder encoder;
    SensorTemperature sensor;
} Sensors;

typedef struct {
    SensorTemperature* sensor;
    i2c_master_bus_handle_t* i2c_bus;
    SemaphoreHandle_t* i2c_bus_mutex;
} TemperatureTaskArgs;

typedef struct {
    int16_t x;
    int16_t y;
} Point2D;

#endif