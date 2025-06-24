#include <stdio.h>
#include <ws2812_control.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

void app_main(void)
{

     // 创建一个WS2812灯带
    ws2812_strip_t* WS2812=ws2812_create();
    while(1)
    {
        //点亮灯带 颜色(255,0,0)
        led_set_on(WS2812,COLOR_RGB(255,0,0));
        vTaskDelay(pdMS_TO_TICKS(500));
        led_set_on(WS2812,COLOR_RGB(0,0,255));
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
