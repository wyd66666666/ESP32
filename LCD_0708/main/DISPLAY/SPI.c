#include "SPI.h"
#include "driver/gpio.h"

//设置SPI句柄
esp_lcd_panel_io_handle_t io_handle = NULL;

void vSPIInit(void)
{
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << PIN_NUM_BK_LIGHT
    };
    // Initialize the GPIO of backlight
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    //初始化接口设备
    spi_bus_config_t buscfg = {
    .sclk_io_num = PIN_NUM_SCLK,  // 连接 LCD SCK（SCL） 信号的 IO 编号
    .mosi_io_num = PIN_NUM_MOSI,  // 连接 LCD MOSI（SDO、SDA） 信号的 IO 编号
    .miso_io_num = -1,  // 连接 LCD MISO（SDI） 信号的 IO 编号，如果不需要从 LCD 读取数据，可以设为 `-1`
    .quadwp_io_num = -1,                  // 必须设置且为 `-1`
    .quadhd_io_num = -1,                  // 必须设置且为 `-1`
    .max_transfer_sz = LCD_H_RES * 80 * sizeof(uint16_t), // 表示 SPI 单次传输允许的最大字节数上限，通常设为全屏大小即可
    };
    spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);
                                          // 第 1 个参数表示使用的 SPI 主机 ID，和后续创建接口设备时保持一致
                                          // 第 3 个参数表示使用的 DMA 通道号，默认设置为 `SPI_DMA_CH_AUTO` 即可

    //创建接口设备
    esp_lcd_panel_io_spi_config_t io_config = {
    .dc_gpio_num = PIN_NUM_DC,    // 连接 LCD DC（RS） 信号的 IO 编号，可以设为 `-1` 表示不使用
    .cs_gpio_num = PIN_NUM_CS,    // 连接 LCD CS 信号的 IO 编号，可以设为 `-1` 表示不使用
    .pclk_hz = LCD_PIXEL_CLOCK_HZ,    // SPI 的时钟频率（Hz），ESP 最高支持 80M（SPI_MASTER_FREQ_80M）
    .lcd_cmd_bits = LCD_CMD_BITS,     // 单位 LCD 命令的比特数，应为 8 的整数倍
    .lcd_param_bits = LCD_PARAM_BITS, // 单位 LCD 参数的比特数，应为 8 的整数倍
    .spi_mode = 0,                            // SPI 模式（0-3），需根据 LCD 驱动 IC 的数据手册以及硬件的配置确定（如 IM[3:0]）
    .trans_queue_depth = 10,                  // SPI 设备传输数据的队列深度，一般设为 10 即可
    //.on_color_trans_done = example_on_color_trans_dome,   // 单次调用 `esp_lcd_panel_draw_bitmap()` 传输完成后的回调函数
    //.user_ctx = &example_user_ctx,            // 传给回调函数的用户参数
    .flags = {    // 以下为 SPI 时序的相关参数，需根据 LCD 驱动 IC 的数据手册以及硬件的配置确定
        .sio_mode = 0,    // 通过一根数据线（MOSI）读写数据，0: Interface I 型，1: Interface II 型
        },
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));
    

}