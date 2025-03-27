#ifndef __SCREEN_H__
#define __SCREEN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"

/**
 * @brief Interface for managing screens.
 *
 * The ScreenInterface structure provides function pointers for creating, drawing,
 * and destroying a screen. It also supports linking multiple screens in a chain
 * using the next and previous pointers.
 */
typedef struct ScreenInterface {
    /**
     * @brief Create the screen.
     *
     * This function initializes a new screen interface.
     *
     * @param self Double pointer where the new screen interface will be stored.
     * @param previous Pointer to the current screen interface.
     * @return esp_err_t ESP_OK on success or an appropriate error code on failure.
     */
    esp_err_t (*create)(struct ScreenInterface* self);

    /**
     * @brief Draw the screen.
     *
     * This function renders or updates the screen.
     *
     * @param self Double pointer to the screen interface to be drawn.
     * @return esp_err_t ESP_OK on success or an appropriate error code on failure.
     */
    esp_err_t (*draw)(struct ScreenInterface* self);

    /**
     * @brief Destroy the screen.
     *
     * This function releases resources associated with the screen interface.
     *
     * @param self Double pointer to the screen interface to be destroyed.
     * @return esp_err_t ESP_OK on success or an appropriate error code on failure.
     */
    esp_err_t (*destroy)(struct ScreenInterface* self);

    /**
     * @brief Update the screen.
     *
     * This function updates the screen interface.
     *
     * @param self Double pointer to the screen interface to be updated.
     * @return esp_err_t ESP_OK on success or an appropriate error code on failure.
     */
    esp_err_t (*update)(struct ScreenInterface* self);
    struct ScreenInterface* previous; /**< Pointer to the previous screen interface in the chain. */
    struct ScreenInterface* next;     /**< Pointer to the next screen interface in the chain. */

    void* context; /**< Pointer to screen-specific data or widgets. */
} ScreenInterface;

#ifdef __cplusplus
}
#endif

#endif /* __SCREEN_H__ */