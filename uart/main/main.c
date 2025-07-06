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
 * @author   ����ԭ���Ŷ�(ALIENTEK)
 * @version  V1.0
 * @date     2023-08-26
 * @brief    ����ͨ��ʵ��
 * @license  Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ******************************************************************************
 * @attention
 * 
 * ʵ��ƽ̨:����ԭ�� ESP32-S3 ������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
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
 * @brief       �������
 * @param       ��
 * @retval      ��
 */
void app_main(void)
{
    esp_err_t ret;
    uint8_t len = 0;
    uint16_t times = 0;
    unsigned char data[1024] = {0};

    ret = nvs_flash_init();                                                             /* ��ʼ��NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    

    uart_init();                                                               /* ��ʼ������ */

    while(1)
    {
        uart_get_buffered_data_len(UART_NUM, (size_t*) &len);                           /* ��ȡ���λ��������ݳ��� */

        if (len > 0)                                                                    /* �ж����ݳ��� */
        {
            memset(data, 0, 1024);                                               /* �Ի��������� */
            printf("\n�����͵���ϢΪ:\n");
            uart_read_bytes(UART_NUM, data, len, 100);                                  /* ������ */
            uart_write_bytes(UART_NUM, (const char*)data, strlen((const char*)data));   /* д���� */
        }
        else
        {
            times++;

            if (times % 5000 == 0)
            {
                printf("\n����ԭ�� ATK-DNESP32-S3 ������ ����ʵ��\n");
                printf("����ԭ��@ALIENTEK\n\n\n");
            }

            if (times % 200 == 0)
            {
                printf("���������ݣ��Իس�������\n");
            }


            vTaskDelay(100);
            /*
            ESP_LOGE("HELLO","hello world!.ERROR.");   // ���������־
            ESP_LOGI("HELLO","hello world!.INFO.");   // �����Ϣ��־
            ESP_LOGW("HELLO","hello world!.WARNING.");// ���������־
            ESP_LOGD("HELLO","hello world!.DEBUG.");// ���������־
            ESP_LOGV("HELLO","hello world!.VERBOSE.");// �����ϸ��־
            */
        }
    }
}
