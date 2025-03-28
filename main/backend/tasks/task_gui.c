#include "./task_gui.h"
#include "backend/controllers/controller_screen.h"
#include "backend/ports/lv_port_disp.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "frontend/screens/screen_encoder.h"
#include "lv_init.h"
#include "misc/lv_timer.h"

char* TAG = "[GUI_TASK]";

// GUI Task (handles LVGL updates)
void task_gui(void* args) {
    StackScreen* stack_screen       = NULL;
    ScreenInterface* screen_encoder = NULL;

    ESP_LOGI(TAG, "Initializing LVGL...");
    lv_init();
    ESP_LOGI(TAG, "LVGL initialized");
    lv_port_disp_init();
    ESP_LOGI(TAG, "Display port initialized");

    ESP_LOGI(TAG, "Creating UI...");
    ESP_ERROR_CHECK(controller_screen_init(&stack_screen));
    ESP_ERROR_CHECK(screen_encoder_init(&screen_encoder, stack_screen));
    stack_screen->head = screen_encoder;
    ESP_LOGI(TAG, "UI created");

    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    uint32_t time_next_ms       = 0;
    uint32_t time_threshold_msg = 1000 / CONFIG_FREERTOS_HZ;

    for (;;) {
        stack_screen->current->draw(stack_screen->current);
        time_next_ms = lv_timer_handler();  // LVGL update

        time_next_ms = (time_threshold_msg > time_next_ms) ? time_threshold_msg : time_next_ms;
        vTaskDelay(pdMS_TO_TICKS(time_next_ms));  // Delay 10ms
    }
}