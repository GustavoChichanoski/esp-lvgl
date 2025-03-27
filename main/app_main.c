#include <stdio.h>

#include "configs/pinout.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#include "freertos/idf_additions.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "./backend/tasks/task_gui.h"
#include "soc/clk_tree_defs.h"

// GUI Task Settings
#define GUI_STACK_SIZE (20 * 1024)
#define ADS_STEP_UV    (125)
#define ADS_DATA_SIZE  (2)

const char* TAG_MAIN = "MAIN";
SemaphoreHandle_t i2c_0_semaphore;

// Main Function (app_main)
void app_main(void) {

    // Create GUI Task
    xTaskCreatePinnedToCore(task_gui, "task_gui", GUI_STACK_SIZE, NULL, 5, NULL, 1);
    i2c_0_semaphore = xSemaphoreCreateBinaryWithCaps(1);

    if (i2c_0_semaphore == NULL) {
        ESP_LOGE(TAG_MAIN, "Semaphore creation failed: i2c_0_semaphore");
        return;
    }

    i2c_master_bus_handle_t i2c_bus        = NULL;
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source                   = I2C_CLK_SRC_DEFAULT,
        .i2c_port                     = ENCODER_I2C_PORT,
        .scl_io_num                   = ENCODER_I2C_SDA_PIN,
        .sda_io_num                   = ENCODER_I2C_SCL_PIN,
        .glitch_ignore_cnt            = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &i2c_bus));

    while (1) {
        ESP_LOGI(TAG_MAIN, "System running...");
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
