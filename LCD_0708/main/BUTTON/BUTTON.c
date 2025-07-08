#include "BUTTON.h"
#include "esp_log.h"
#include "button_gpio.h"   
#include "iot_button.h"
#include "LED.h"

static void button_single_click_cb(void *arg,void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_SINGLE_CLICK");
    LED0_TOGGLE();
}


void vButtonInit(void)
{
    // create gpio button
    const button_config_t btn_cfg = {0};
    const button_gpio_config_t btn_gpio_cfg = {
        .gpio_num = 0,
        .active_level = 0,
        .enable_power_save = true,     //低功耗支持
    };

    button_handle_t gpio_btn = NULL;
    esp_err_t ret = iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &gpio_btn);
    
    if(NULL == gpio_btn) 
    {
        ESP_LOGE(TAG, "Button create failed");
    }
    

    ret = iot_button_register_cb(gpio_btn, BUTTON_PRESS_DOWN, NULL, button_single_click_cb, NULL);
}
