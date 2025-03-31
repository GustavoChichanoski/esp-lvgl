#include "./controller_screen.h"
#include "esp_log.h"

const char* stack_tag = "[Stack Screen]";

/**
 * @brief Initialize the stack screen controller.
 *
 * This function allocates memory for the stack screen data structure and
 * initializes the stack screen controller.
 *
 * @param stack_screen Pointer to the stack screen data structure to be
 * initialized.
 *
 * @return esp_err_t ESP_OK on success or an appropriate error code on failure.
 *                   ESP_ERR_NO_MEM if memory allocation failed.
 */
esp_err_t controllerScreenInit(StackScreen** stack_screen) {
    (*stack_screen) = calloc(1, sizeof(StackScreen));
    if (!(*stack_screen)) {
        ESP_LOGE(stack_tag, "Failed to allocate memory for stack_screen");
        return ESP_ERR_NO_MEM;
    }
    (*stack_screen)->current = NULL;
    (*stack_screen)->head    = NULL;
    ESP_LOGI(stack_tag, "Stack Screen initialized");
    return ESP_OK;
}

/**
 * @brief Add a screen to the stack.
 *
 * This function adds a new screen to the stack screen structure.
 * If the stack already has a head screen, the new screen is linked
 * to the current screen as its previous screen. Otherwise, the new
 * screen is set as the head of the stack. The current screen pointer
 * is updated to the new screen.
 *
 * @param stack_screen Pointer to the StackScreen structure.
 * @param screen Pointer to the ScreenInterface to be added to the stack.
 *
 * @return esp_err_t ESP_OK on success or ESP_ERR_INVALID_ARG if the
 * screen is invalid.
 */
esp_err_t controllerScreenAdd(StackScreen* stack_screen, ScreenInterface* screen) {
    if (!screen) {
        ESP_LOGE(stack_tag, "Invalid screen");
        return ESP_ERR_INVALID_ARG;
    }
    if (stack_screen->head != NULL) {
        screen->previous = stack_screen->current;
    } else {
        stack_screen->head = screen;
        screen->previous   = NULL;
    }
    stack_screen->current = screen;
    ESP_LOGI(stack_tag, "Stack Screen added");
    return ESP_OK;
}

/**
 * @brief Pop the current screen from the stack.
 *
 * This function removes the current screen from the stack and
 * moves the current screen pointer to the previous screen. If
 * no previous screen exists, it will return ESP_ERR_NOT_FOUND.
 *
 * @param stack_screen Pointer to the StackScreen structure.
 * @param screen Pointer to the ScreenInterface to be removed from the stack.
 *
 * @return esp_err_t ESP_OK on success or ESP_ERR_NOT_FOUND if no previous
 * screen exists.
 */
esp_err_t controllerScreenPop(StackScreen* stack_screen, ScreenInterface* screen) {
    if (stack_screen->current == NULL || stack_screen->current->previous == NULL ||
        stack_screen->head == stack_screen->current) {
        ESP_LOGE(stack_tag, "No previous screen");
        return ESP_ERR_NOT_FOUND;
    }
    screen = stack_screen->current->previous;
    stack_screen->current->destroy(stack_screen->current);
    stack_screen->current = screen;
    ESP_LOGI(stack_tag, "Back to previous screen");
    return ESP_OK;
}

/**
 * @brief Draw the current screen in the stack.
 *
 * This function attempts to draw the current screen in the provided
 * StackScreen structure. It verifies that there is a current screen
 * and that the current screen has a draw function. If successful, it
 * calls the draw function of the current screen interface.
 *
 * @param stack_screen Pointer to the StackScreen structure containing
 * the current screen to be drawn.
 * @return esp_err_t ESP_OK on success, ESP_ERR_NOT_FOUND if no current
 * screen exists, or ESP_ERR_INVALID_STATE if the current screen's draw
 * function is not set.
 */
esp_err_t controllerScreenDraw(StackScreen* stack_screen) {
    if (stack_screen->current == NULL) {
        ESP_LOGE(stack_tag, "No current screen to draw");
        return ESP_ERR_NOT_FOUND;
    }
    if (stack_screen->current->draw == NULL) {
        ESP_LOGE(stack_tag, "No draw function for current screen");
        return ESP_ERR_INVALID_STATE;
    }
    ESP_LOGI(stack_tag, "Drawing current screen");
    return stack_screen->current->draw(stack_screen->current);
}