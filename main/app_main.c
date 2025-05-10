// Standard library
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


// ESP IDF
#include "esp_log.h"

// FreeRTOS
#include "freertos/idf_additions.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

// Project
#include "backend/devices/rotary_encoder.h"
#include "backend/tasks/task_gui.h"
#include "configs/project_types.h"

// GUI Task Settings
#define GUI_STACK_SIZE (10 * 1024)
#define ADS_STEP_UV    (125)
#define ADS_DATA_SIZE  (2)

const char kTagMain[] = "[MAIN]";

// Main Function (app_main)
void app_main(void) {
    Encoder* encoder = (Encoder*)malloc(sizeof(Encoder));
    xTaskCreate(encoderTask, "task_encoder", 2048, encoder, 3, NULL);
    // Create GUI Task
    xTaskCreatePinnedToCore(taskGui, "task_gui", GUI_STACK_SIZE, encoder, 5, NULL, 1);

    while (true) {
        ESP_LOGI(kTagMain, "System running...");
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
