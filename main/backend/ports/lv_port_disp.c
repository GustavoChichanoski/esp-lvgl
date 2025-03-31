/**********************
 *      INCLUDES
 *********************/
#include "./lv_port_disp.h"

#include "display/lv_display.h"
#include "driver/spi_common.h"
#include "esp_err.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_st7789.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "hal/spi_types.h"
#include "misc/lv_types.h"

/**********************
 *      DEFINES
 *********************/


#define CONFIG_LCD_HOST       (SPI2_HOST)

#define BOARD_LCD_MISO        (19)
#define BOARD_LCD_MOSI        (23)
#define BOARD_LCD_SCK         (18)
#define BOARD_LCD_CS          (15)
#define BOARD_LCD_RST         (4)
#define BOARD_LCD_DC          (2)

#define CONFIG_LCD_H_RES      (240)
#define CONFIG_LCD_V_RES      (320)
#define CONFIG_LCD_FREQ       (10 * 1000 * 1000)
#define CONFIG_LCD_CMD_BITS   (8)
#define CONFIG_LCD_PARAM_BITS (8)

#define LVGL_TICK_PERIOD_MS   (5)
#define LVGL_DRAW_BUF_LINES   (20)

#define BYTE_PER_PIXEL        (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /*will be 2 for RGB565 */
#define BUFFER_SIZE           (CONFIG_LCD_H_RES * LVGL_DRAW_BUF_LINES)
/**********************
 *      VARIABLES
 **********************/
esp_lcd_panel_handle_t panel_handle = NULL;

/**********************
 * STATIC PROTOTYPES
 **********************/

static void disp_init(esp_lcd_panel_io_handle_t* io_handle);

static void disp_flush(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map);

/**********************
 * GLOBAL FUNCTIONS
 **********************/
// Add this at the top of the file
#define DEBUG_TAG "[LVGL PORT]"

/**
 * @brief Callback to notify LVGL that a flush operation is complete.
 *
 * This function is called when a color transfer is done, signaling LVGL
 * that the display flush operation is complete. It marks the display as
 * ready for the next rendering operation by calling `lv_display_flush_ready`.
 *
 * @param panel_io Handle to the panel IO.
 * @param edata Event data associated with the panel IO operation.
 * @param user_ctx User context, expected to be a pointer to the LVGL display.
 * @return false to indicate no further action is required by the panel IO.
 */
static bool lvgl_flush_ready_notify(esp_lcd_panel_io_handle_t panel_io,
                                    esp_lcd_panel_io_event_data_t* edata, void* user_ctx) {
    lv_display_flush_ready((lv_display_t*)user_ctx);
    return false;
}

/**
 * @brief LVGL tick callback function.
 *
 * This function is called by the esp_timer callback to increase the LVGL tick
 * counter. It is used to keep track of time elapsed since the last call to
 * lv_tick_inc() and is used to schedule LVGL tasks.
 */
static void increase_lvgl_tick(void* arg) {
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

/**
 * @brief Allocate buffers for the display driver.
 *
 * This function allocates two buffers in SPIRAM, each of size @ref BUFFER_SIZE
 * bytes. The buffers are used by the display driver to render the screen.
 *
 * @param[in] display Pointer to the LVGL display.
 *
 * @note The buffers are allocated with the @ref LV_ATTRIBUTE_MEM_ALIGN attribute
 *       to ensure that they are aligned to the display's requirements.
 */
static void lv_port_allocate_buffer(lv_display_t* display) {
    ESP_LOGI(DEBUG_TAG, "Allocating buffers in SPIRAM...");
    ESP_LOGI(DEBUG_TAG, "Buffer size: %d bytes", BUFFER_SIZE);
    LV_ATTRIBUTE_MEM_ALIGN

    size_t draw_buffer_sz = BUFFER_SIZE * sizeof(lv_color16_t);

    void* buf1 = spi_bus_dma_memory_alloc(CONFIG_LCD_HOST, draw_buffer_sz, 0);
    void* buf2 = spi_bus_dma_memory_alloc(CONFIG_LCD_HOST, draw_buffer_sz, 0);
    LV_ATTRIBUTE_MEM_ALIGN
    // uint8_t *buf2 = (uint8_t *)malloc(BUFFER_SIZE);

    if (!buf1 || !buf2) {
        ESP_LOGE(DEBUG_TAG, "Failed to allocate display buffers!");
        abort();
    }

    ESP_LOGI(DEBUG_TAG, "Buffers allocated: buf1@%p buf2@%p", buf1, buf2);
    lv_display_set_buffers(display, buf1, buf2, draw_buffer_sz, LV_DISPLAY_RENDER_MODE_PARTIAL);
}

void lv_port_disp_init(void) {
    esp_lcd_panel_io_handle_t io_handle = NULL;
    ESP_LOGI(DEBUG_TAG, "Initializing display...");
    disp_init(&io_handle);

    ESP_LOGI(DEBUG_TAG, "Creating LVGL display...");
    lv_display_t* disp = lv_display_create(CONFIG_LCD_H_RES, CONFIG_LCD_V_RES);
    LV_ASSERT_MALLOC(disp);

    // Allocate buffers to lvgl
    lv_port_allocate_buffer(disp);

    // associate the mipi panel handle to the display
    lv_display_set_user_data(disp, panel_handle);
    // set color depth
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    // set the callback which can copy the rendered image to an area of the
    // display
    lv_display_set_flush_cb(disp, disp_flush);

    ESP_LOGI(DEBUG_TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {.callback = &increase_lvgl_tick,
                                                          .name     = "lvgl_tick"};

    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));

    ESP_LOGI(DEBUG_TAG, "Register io panel event callback for LVGL flush ready notification");
    const esp_lcd_panel_io_callbacks_t cbs = {
        .on_color_trans_done = lvgl_flush_ready_notify,
    };
    /* Register done callback */
    ESP_ERROR_CHECK(esp_lcd_panel_io_register_event_callbacks(io_handle, &cbs, disp));
}

/**
 * @brief Initialize the display.
 *
 * This function initializes the SPI bus and LCD display device.
 *
 * @param io_handle[inout] Pointer to the SPI bus handle.
 *
 * @return esp_err_t ESP_OK on success or an appropriate error code on failure.
 *
 * @note This function is called by lv_port_disp_init and should not be called
 *       directly. It is intended to be used internally by the LVGL port.
 */
static void disp_init(esp_lcd_panel_io_handle_t* io_handle) {
    spi_bus_config_t bus_cfg = {.sclk_io_num     = BOARD_LCD_SCK,
                                .mosi_io_num     = BOARD_LCD_MOSI,
                                .miso_io_num     = BOARD_LCD_MISO,
                                .quadwp_io_num   = -1,
                                .quadhd_io_num   = -1,
                                .max_transfer_sz = (100 * 100 * sizeof(uint16_t))};
    ESP_ERROR_CHECK(
        spi_bus_initialize((spi_host_device_t)CONFIG_LCD_HOST, &bus_cfg, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_spi_config_t io_cfg = {.dc_gpio_num       = BOARD_LCD_DC,
                                            .cs_gpio_num       = BOARD_LCD_CS,
                                            .pclk_hz           = CONFIG_LCD_FREQ,
                                            .lcd_cmd_bits      = CONFIG_LCD_CMD_BITS,
                                            .lcd_param_bits    = CONFIG_LCD_PARAM_BITS,
                                            .spi_mode          = 0,
                                            .trans_queue_depth = 10};
    ESP_ERROR_CHECK(
        esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)CONFIG_LCD_HOST, &io_cfg, io_handle));

    esp_lcd_panel_dev_config_t panel_cfg = {.reset_gpio_num = BOARD_LCD_RST,
                                            .rgb_ele_order  = LCD_RGB_ELEMENT_ORDER_RGB,
                                            .bits_per_pixel = 16,
                                            .data_endian    = LCD_RGB_DATA_ENDIAN_LITTLE};
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(*io_handle, &panel_cfg, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, false));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, false));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, false, false));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, false));
}

/**
 * @brief Callback to update the display rotation.
 *
 * @param[in] disp Pointer to the initialized display.
 *
 * This callback is called when the display rotation changes.
 * It updates the display rotation of the LCD panel.
 */
static void lvgl_port_update_callback(lv_display_t* disp) {
    // esp_lcd_panel_handle_t panel_handle = lv_display_get_user_data(disp);
    // lv_display_rotation_t rotation      = lv_display_get_rotation(disp);

    // switch (rotation) {
    //     case LV_DISPLAY_ROTATION_0:
    //         // Rotate LCD display
    //         esp_lcd_panel_swap_xy(panel_handle, false);
    //         esp_lcd_panel_mirror(panel_handle, true, false);
    //         break;
    //     case LV_DISPLAY_ROTATION_90:
    //         // Rotate LCD display
    //         esp_lcd_panel_swap_xy(panel_handle, true);
    //         esp_lcd_panel_mirror(panel_handle, true, true);
    //         break;
    //     case LV_DISPLAY_ROTATION_180:
    //         // Rotate LCD display
    //         esp_lcd_panel_swap_xy(panel_handle, false);
    //         esp_lcd_panel_mirror(panel_handle, false, true);
    //         break;
    //     case LV_DISPLAY_ROTATION_270:
    //         // Rotate LCD display
    //         esp_lcd_panel_swap_xy(panel_handle, true);
    //         esp_lcd_panel_mirror(panel_handle, false, false);
    //         break;
    // }
}

/**
 * @brief Flush the display.
 *
 * @param[in] display Pointer to the initialized display.
 * @param[in] area The area that should be updated.
 * @param[in] px_map Pixel map to be rendered.
 */
static void disp_flush(lv_display_t* display, const lv_area_t* area, uint8_t* px_map) {
    lvgl_port_update_callback(display);

    int offset_x1 = area->x1;
    int offset_x2 = area->x2;
    int offset_y1 = area->y1;
    int offset_y2 = area->y2;

    // lv_draw_sw_rgb565_swap(px_map, (offset_x2 + 1 - offset_x1) * (offset_y2 + 1
    // - offset_y1));

    // Draw the bitmap on the specified area of the display
    esp_lcd_panel_draw_bitmap(panel_handle, offset_x1, offset_y1, offset_x2 + 1, offset_y2 + 1,
                              (void*)px_map);
}