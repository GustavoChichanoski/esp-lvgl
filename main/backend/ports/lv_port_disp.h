#ifndef LV_PORT_DISP_H
#define LV_PORT_DISP_H

/**********************
 *      INCLUDES
 *********************/

#include "esp_lcd_types.h"

/**********************
 *      VARIABLES
 **********************/

extern esp_lcd_panel_handle_t panel_handle;

/**
* @brief Initialize the display port.
*
* This function initializes the SPI bus and LCD display device, creates an LVGL
* display, allocates buffers to it, associates the panel handle with the display
* and sets the LVGL tick timer. It also registers an event callback to notify
* LVGL when a flush is ready.
*
* @note This function should not be called directly. It is intended to be used
*       internally by the LVGL port.
*/
void lvPortDispInit(void);

#endif /*LV_PORT_DISP_H*/