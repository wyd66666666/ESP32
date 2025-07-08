#include  "LCD.h"
#include "driver/gpio.h"

esp_lcd_panel_handle_t panel_handle = NULL;
extern esp_lcd_panel_io_handle_t io_handle;

void vLCDInit(void)
{
    /* 创建 LCD 设备 */

    esp_lcd_panel_dev_config_t panel_config = {
    .reset_gpio_num = PIN_NUM_RST,
    .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
    .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // 2. 初始化 LCD 设备
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    //esp_lcd_panel_disp_on_off(panel_handle, false);
    esp_lcd_panel_disp_on_off(panel_handle, true);
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    ESP_ERROR_CHECK(gpio_set_level(PIN_NUM_BK_LIGHT, BK_LIGHT_ON_LEVEL));

}
