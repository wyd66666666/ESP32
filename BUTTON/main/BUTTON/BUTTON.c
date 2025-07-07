#include "BUTTON.h"
#include "esp_log.h"
#include "button_gpio.h"   

void vButtonInit(void)
{
    // create gpio button
    const button_config_t btn_cfg = {0};
    const button_gpio_config_t btn_gpio_cfg = {
        .gpio_num = 0,
        .active_level = 0,
    };
    button_handle_t gpio_btn = NULL;
    esp_err_t ret = iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &gpio_btn);
    if(NULL == gpio_btn) 
    {
        ESP_LOGE(TAG, "Button create failed");
    }

}
