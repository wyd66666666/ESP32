/**
 ******************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2025-01-01
 * @brief       ADC 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
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
#include "freertos/task.h"
#include "nvs_flash.h"
#include "led.h"
#include "adc.h"
#include <stdio.h>


/**
 * @brief       程序入口
 * @param       无
 * @retval      无
 */
void app_main(void)
{
    esp_err_t ret;
    uint16_t adc_data = 0;
    float adc_vol = 0;

    ret = nvs_flash_init();     /* 初始化NVS */
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    led_init();                 /* LED初始化 */
    adc_init();                 /* ADC初始化 */


    while (1)
    {
        adc_data = adc_get_result_average(ADC_CHAN, 10);    /* 获取ADC值 */
        ESP_LOGI("ADC","adc_data = %d", adc_data); 

        adc_vol = (float)adc_data * 3.3 / 4095;
        ESP_LOGI("ADC","adc_vol = %.3f", adc_vol);

        LED0_TOGGLE();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
