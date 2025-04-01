#include "logs.h"
#include "esp_log.h"

void logErrorLvDel(const char* tag, char* name) {
    ESP_LOGE(tag, "Failed to delete %s during destroy", name);
}
