#include "logs.h"
#include "esp_log.h"

void log_error_lv_del(const char* tag, char* name) {
    ESP_LOGE(tag, "Failed to delete %s during destroy", name);
}
