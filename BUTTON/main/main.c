#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "LED.h"

#include "sdkconfig.h"  /* 包含kconfig配置文件 */

#include "esp_log.h"
#include "BUTTON.h"



void app_main(void)
{
    led_init();                 /* 初始化LED */
    vButtonInit();             /* 初始化按钮 */
    while(1)
    {
        LED0_TOGGLE();
        vTaskDelay(pdMS_TO_TICKS(500));    /* 延时1s */
    }
}