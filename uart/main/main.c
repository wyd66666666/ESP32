/*
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "uart.h"
#include "sdkconfig.h"
#include "string.h"

void app_main(void)
{
    uint8_t len = 1024;
    unsigned char data[1024] = {0};
    // Initialize UART
    uart_init();
    // Install UART driver using an event queue here
    //ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));
    while(1)
    {
        // Write data to UART.
        char* test_str = "This is a test string.\n";
        uart_write_bytes(UART_NUM_0, (const char*)test_str, strlen(test_str));
        uart_read_bytes(UART_NUM_0, data, len, 100);                                  
        uart_write_bytes(UART_NUM_0, (const char*)data, strlen((const char*)data));  
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second

    }
}
*/

/**
 ******************************************************************************
 * @file     main.c
 * @author   正点原子团队(ALIENTEK)
 * @version  V1.0
 * @date     2023-08-26
 * @brief    串口通信实验
 * @license  Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ******************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 ESP32-S3 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 ******************************************************************************
 */

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "uart.h"
#include "sdkconfig.h"
#include "string.h"

#include "esp_log.h"


/**
 * @brief       程序入口
 * @param       无
 * @retval      无
 */
void app_main(void)
{
    esp_err_t ret;
    uint8_t len = 0;
    uint16_t times = 0;
    unsigned char data[1024] = {0};

    ret = nvs_flash_init();                                                             /* 初始化NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    

    uart_init();                                                               /* 初始化串口 */

    while(1)
    {
        uart_get_buffered_data_len(UART_NUM, (size_t*) &len);                           /* 获取环形缓冲区数据长度 */

        if (len > 0)                                                                    /* 判断数据长度 */
        {
            memset(data, 0, 1024);                                               /* 对缓冲区清零 */
            printf("\n您发送的消息为:\n");
            uart_read_bytes(UART_NUM, data, len, 100);                                  /* 读数据 */
            uart_write_bytes(UART_NUM, (const char*)data, strlen((const char*)data));   /* 写数据 */
        }
        else
        {
            times++;

            if (times % 5000 == 0)
            {
                printf("\n正点原子 ATK-DNESP32-S3 开发板 串口实验\n");
                printf("正点原子@ALIENTEK\n\n\n");
            }

            if (times % 200 == 0)
            {
                printf("请输入数据，以回车键结束\n");
            }


            vTaskDelay(100);
            /*
            ESP_LOGE("HELLO","hello world!.ERROR.");   // 输出错误日志
            ESP_LOGI("HELLO","hello world!.INFO.");   // 输出信息日志
            ESP_LOGW("HELLO","hello world!.WARNING.");// 输出警告日志
            ESP_LOGD("HELLO","hello world!.DEBUG.");// 输出调试日志
            ESP_LOGV("HELLO","hello world!.VERBOSE.");// 输出详细日志
            */
        }
    }
}
