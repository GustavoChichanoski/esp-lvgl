#include <stdio.h>

#include "esp_log.h"

#include "freertos/idf_additions.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "./backend/tasks/task_gui.h"

// GUI Task Settings
#define GUI_STACK_SIZE (10 * 1024)
#define ADS_STEP_UV    (125)
#define ADS_DATA_SIZE  (2)

const char TAG_MAIN[] = "[MAIN]";

// Main Function (app_main)
void app_main(void) {

    // Create GUI Task
    xTaskCreatePinnedToCore(task_gui, "task_gui", GUI_STACK_SIZE, NULL, 5, NULL, 1);

    while (1) {
        ESP_LOGI(TAG_MAIN, "System running...");
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
