#ifndef __PROJECT_TYPES_H__
#define __PROJECT_TYPES_H__

#include <stdint.h>

/// IDF
#include "driver/i2c_types.h"

/// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef struct {
    uint8_t position;
    uint8_t last_position;
    uint8_t direction;
    uint8_t last_a_level;
    uint8_t last_b_level;
    uint64_t last_time;
    uint32_t velocity;
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
    float temperature;
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

#endif