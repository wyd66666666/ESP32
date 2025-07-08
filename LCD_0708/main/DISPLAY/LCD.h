#ifndef  __LCD_H
#define  __LCD_H

#include "esp_lcd_panel_vendor.h"   // 依赖的头文件
#include "esp_lcd_panel_ops.h"
#include "SPI.h"

#define  BK_LIGHT_ON_LEVEL     1

void vLCDInit(void);



#endif