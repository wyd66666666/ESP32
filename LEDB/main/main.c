#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "LED.h"

#include "sdkconfig.h"  /* 包含kconfig配置文件 */

void app_main(void)
{
    led_init();                 /* 初始化LED */

    while(1)
    {
        LED0_TOGGLE();
        vTaskDelay(pdMS_TO_TICKS(CONFIG_BLINK_PERIOD));    /* 延时1s */
    }
}