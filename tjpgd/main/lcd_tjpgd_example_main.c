/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>                             // 标准输入输出头文件
#include "sdkconfig.h"                         // SDK 配置头文件
#include "freertos/FreeRTOS.h"                 // FreeRTOS 主头文件
#include "freertos/task.h"                     // FreeRTOS 任务头文件
#include "esp_lcd_panel_io.h"                  // ESP LCD 面板 IO 头文件
#include "esp_lcd_panel_vendor.h"              // ESP LCD 面板厂商相关头文件
#include "esp_lcd_panel_ops.h"                 // ESP LCD 面板操作头文件
#include "esp_heap_caps.h"                     // ESP 堆内存分配头文件
#include "driver/spi_master.h"                 // SPI 主机驱动头文件
#include "driver/gpio.h"                       // GPIO 驱动头文件
#include "pretty_effect.h"                     // 特效处理头文件

// Using SPI2 in the example, as it also supports octal modes on some targets
#define LCD_HOST       SPI2_HOST               // 使用 SPI2 作为 LCD 主机
// To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many.
// More means more memory use, but less overhead for setting up / finishing transfers. Make sure 240
// is dividable by this.
#define PARALLEL_LINES CONFIG_EXAMPLE_LCD_FLUSH_PARALLEL_LINES // 每次 SPI 传输的行数
// The number of frames to show before rotate the graph
#define ROTATE_FRAME   30                      // 每 ROTATE_FRAME 帧旋转一次画面

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ (20 * 1000 * 1000)        // LCD 像素时钟频率 20MHz
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  1                     // 背光点亮电平
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL // 背光关闭电平
#define EXAMPLE_PIN_NUM_DATA0          47                    // SPI MOSI 引脚
#define EXAMPLE_PIN_NUM_PCLK           21                    // SPI 时钟引脚
#define EXAMPLE_PIN_NUM_CS             41                    // SPI 片选引脚
#define EXAMPLE_PIN_NUM_DC             40                    // 数据/命令选择引脚
#define EXAMPLE_PIN_NUM_RST            45                    // LCD 复位引脚
#define EXAMPLE_PIN_NUM_BK_LIGHT       42                    // 背光控制引脚

// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES              240                   // LCD 水平分辨率
#define EXAMPLE_LCD_V_RES              240                   // LCD 垂直分辨率
// Bit number used to represent command and parameter
#define EXAMPLE_LCD_CMD_BITS           8                     // 命令位宽
#define EXAMPLE_LCD_PARAM_BITS         8                     // 参数位宽

#if CONFIG_EXAMPLE_LCD_SPI_8_LINE_MODE
#define EXAMPLE_PIN_NUM_DATA1    7                           // SPI 8线模式数据1
#define EXAMPLE_PIN_NUM_DATA2    8                           // SPI 8线模式数据2
#define EXAMPLE_PIN_NUM_DATA3    9                           // SPI 8线模式数据3
#define EXAMPLE_PIN_NUM_DATA4    10                          // SPI 8线模式数据4
#define EXAMPLE_PIN_NUM_DATA5    11                          // SPI 8线模式数据5
#define EXAMPLE_PIN_NUM_DATA6    12                          // SPI 8线模式数据6
#define EXAMPLE_PIN_NUM_DATA7    13                          // SPI 8线模式数据7
#endif // CONFIG_EXAMPLE_LCD_SPI_8_LINE_MODE

// Simple routine to generate some patterns and send them to the LCD. Because the
// SPI driver handles transactions in the background, we can calculate the next line
// while the previous one is being sent.
//相当于一个缓冲区
static uint16_t *s_lines[2];                                 // 用于存储两组像素数据的缓冲区
static void display_pretty_colors(esp_lcd_panel_handle_t panel_handle) // 显示特效主循环
{
    int frame = 0;                                           // 帧计数器
    // Indexes of the line currently being sent to the LCD and the line we're calculating
    int sending_line = 0;                                    // 当前发送的缓冲区索引
    int calc_line = 0;                                       // 当前计算的缓冲区索引

    // After ROTATE_FRAME frames, the image will be rotated
    while (frame <= ROTATE_FRAME) {                          // 循环 ROTATE_FRAME 次
        frame++;                                             // 帧数递增
        for (int y = 0; y < EXAMPLE_LCD_V_RES; y += PARALLEL_LINES) { // 每次处理 PARALLEL_LINES 行
            // Calculate a line
            pretty_effect_calc_lines(s_lines[calc_line], y, frame, PARALLEL_LINES); // 计算特效像素数据
            sending_line = calc_line;                        // 设置当前发送缓冲区
            calc_line = !calc_line;                          // 切换计算缓冲区
            // Send the calculated data
            esp_lcd_panel_draw_bitmap(panel_handle, 0, y, 0 + EXAMPLE_LCD_H_RES, y + PARALLEL_LINES, s_lines[sending_line]); // 发送像素数据到 LCD
        }
    }
}

void app_main(void)                                          // 主入口函数
{
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,                            // 设置为输出模式
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT     // 设置背光引脚
    };
    // Initialize the GPIO of backlight
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));           // 初始化背光 GPIO

    spi_bus_config_t buscfg = {
        .sclk_io_num = EXAMPLE_PIN_NUM_PCLK,                 // SPI 时钟引脚
        .mosi_io_num = EXAMPLE_PIN_NUM_DATA0,                // SPI MOSI 引脚
        .miso_io_num = -1,                                   // 不使用 MISO
        .quadwp_io_num = -1,                                 // 不使用 WP
        .quadhd_io_num = -1,                                 // 不使用 HD
        .max_transfer_sz = PARALLEL_LINES * EXAMPLE_LCD_H_RES * 2 + 8 // 最大传输大小
    };
#if CONFIG_EXAMPLE_LCD_SPI_8_LINE_MODE
    buscfg.data1_io_num = EXAMPLE_PIN_NUM_DATA1;             // 8线模式数据1
    buscfg.data2_io_num = EXAMPLE_PIN_NUM_DATA2;             // 8线模式数据2
    buscfg.data3_io_num = EXAMPLE_PIN_NUM_DATA3;             // 8线模式数据3
    buscfg.data4_io_num = EXAMPLE_PIN_NUM_DATA4;             // 8线模式数据4
    buscfg.data5_io_num = EXAMPLE_PIN_NUM_DATA5;             // 8线模式数据5
    buscfg.data6_io_num = EXAMPLE_PIN_NUM_DATA6;             // 8线模式数据6
    buscfg.data7_io_num = EXAMPLE_PIN_NUM_DATA7;             // 8线模式数据7
    buscfg.flags = SPICOMMON_BUSFLAG_OCTAL;                  // 设置为八线模式
#endif
    // Initialize the SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO)); // 初始化 SPI 总线

    esp_lcd_panel_io_handle_t io_handle = NULL;              // LCD IO 句柄
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = EXAMPLE_PIN_NUM_DC,                   // 数据/命令引脚
        .cs_gpio_num = EXAMPLE_PIN_NUM_CS,                   // 片选引脚
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,               // 像素时钟频率
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,                // 命令位宽
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,            // 参数位宽
        .spi_mode = 0,                                      // SPI 模式0
        .trans_queue_depth = 10,                            // 事务队列深度
    };
#if CONFIG_EXAMPLE_LCD_SPI_8_LINE_MODE
    io_config.spi_mode = 3;                                 // 8线模式下使用 SPI 模式3
    io_config.flags.octal_mode = 1;                         // 启用八线模式
#endif
    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle)); // 创建 LCD IO

    esp_lcd_panel_handle_t panel_handle = NULL;              // LCD 面板句柄
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_RST,               // 复位引脚
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,          // RGB 顺序
        .bits_per_pixel = 16,                                // 每像素位数
    };
    // Initialize the LCD configuration
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle)); // 初始化 LCD 面板

    // Turn off backlight to avoid unpredictable display on the LCD screen while initializing
    // the LCD panel driver. (Different LCD screens may need different levels)
    ESP_ERROR_CHECK(gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL)); // 初始化时关闭背光

    // Reset the display
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));      // 复位 LCD 面板

    // Initialize LCD panel
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));       // 初始化 LCD 面板

    // Turn on the screen
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true)); // 打开 LCD 显示
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true)); // 反转颜色

    // Swap x and y axis (Different LCD screens may need different options)
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, false)); // 不交换 XY 轴

    // Turn on backlight (Different LCD screens may need different levels)
    ESP_ERROR_CHECK(gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL)); // 打开背光

    // Initialize the effect displayed
    ESP_ERROR_CHECK(pretty_effect_init());                   // 初始化特效（解码图片）

    // "Rotate or not" flag
    bool is_rotated = false;                                 // 是否旋转标志

    // Allocate memory for the pixel buffers
    for (int i = 0; i < 2; i++) {                           // 分配两个缓冲区
        s_lines[i] = heap_caps_malloc(EXAMPLE_LCD_H_RES * PARALLEL_LINES * sizeof(uint16_t), MALLOC_CAP_DMA); // 分配 DMA 内存
        assert(s_lines[i] != NULL);                          // 检查分配是否成功
    }

    // Start and rotate
    while (1) {                                              // 无限循环
        // Set driver configuration to rotate 180 degrees each time
        ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, is_rotated, is_rotated)); // 设置镜像（旋转）
        // Display
        display_pretty_colors(panel_handle);                 // 显示特效
        is_rotated = !is_rotated;                            // 每次切换旋转状态
    }
}
