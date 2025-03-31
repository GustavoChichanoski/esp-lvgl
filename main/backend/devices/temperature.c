#include "./temperature.h"

// C Standard includes
#include "stdbool.h"

// Esp-IDF includes
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "hal/i2c_types.h"

// Project includes
#include "configs/project_types.h"

const char kTag[] = "[TEMPERATURE]";

AdcFlags adc_flags = {.flags = 0b1100110000011};

/**
 * @brief Scans the I2C bus for a connected ADS1115 temperature sensor.
 *
 * @param i2c Pointer to I2C bus handle.
 * @param sensor Pointer to SensorTemperature struct which will be filled with the
 *               discovered sensor's address.
 * @param i2c_bus_mutex Pointer to a binary semaphore that protects the I2C bus.
 *
 * @return true if a sensor is found, false otherwise.
 */
bool ads1115Find(i2c_master_bus_handle_t* i2c, SensorTemperature* sensor,
                 SemaphoreHandle_t* i2c_bus_mutex) {
    if (NULL == *i2c) {
        ESP_LOGE(kTag, "I2C not initialized");
        return false;
    }

    uint8_t addr = 0b1001000;

    sensor->address = 0;
    for (addr = 0b1001000; addr < 0b1001100; addr++) {
        if (pdFALSE == xSemaphoreTake(*i2c_bus_mutex, pdMS_TO_TICKS(10))) {
            addr--;
            continue;
        }
        if (i2c_master_probe(*i2c, addr, 10)) {
            xSemaphoreGive(*i2c_bus_mutex);
            continue;
        }
        xSemaphoreGive(*i2c_bus_mutex);
        sensor->address = addr;
        break;
    }

    if (0 == sensor->address) {
        ESP_LOGE(kTag, "No sensor found");
        return false;
    }

    return true;
}

bool ads1115Read(i2c_master_dev_handle_t* i2c, uint8_t channel, TemperatureTaskArgs* args) {
    uint8_t data[2];
    adc_flags.as_bits.mux = channel;

    while (pdFALSE == xSemaphoreTake((*args->i2c_bus_mutex), pdMS_TO_TICKS(10))) {}
    i2c_master_transmit(*i2c, (uint8_t*)&adc_flags.flags, 2, 0);
    i2c_master_receive(*i2c, data, 2, 0);
    xSemaphoreGive((*args->i2c_bus_mutex));

    args->sensor->temperature = (data[0] << 4) | (data[1] >> 4);
    return true;
}

void temperatureTask(void* args) {
    TemperatureTaskArgs* task_args = (TemperatureTaskArgs*)args;
    ESP_LOGI(kTag, "Temperature task started");

    i2c_master_dev_handle_t dev_handle = NULL;

    while (true) {
        if (!ads1115Find(task_args->i2c_bus, task_args->sensor, task_args->i2c_bus_mutex)) break;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    i2c_device_config_t dev_config = {.dev_addr_length = I2C_ADDR_BIT_LEN_7,
                                      .device_address  = task_args->sensor->address,
                                      .scl_speed_hz    = ADS1115_SPEED,
                                      .flags           = {.disable_ack_check = false},
                                      .scl_wait_us     = 0};

    ESP_ERROR_CHECK(i2c_master_bus_add_device(*task_args->i2c_bus, &dev_config, &dev_handle));
    ESP_LOGI(kTag, "Temperature task initialized");

    while (true) {
        ESP_LOGI(kTag, "Temperature task running");
        vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for 1 second
    }
}