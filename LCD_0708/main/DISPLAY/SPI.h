#ifndef   __SPI_H
#define   __SPI_H

#include <stdio.h>
#include "esp_log.h" 
#include "driver/spi_master.h"            // 依赖的头文件
#include "esp_check.h"    
#include "esp_lcd_panel_io.h"       // 创建接口设备依赖的头文件

#define PIN_NUM_SCLK           21          // SPI SCK 引脚
#define PIN_NUM_MOSI           47          // SPI MOSI 引脚
#define LCD_H_RES              240         // LCD 水平分辨率 
#define LCD_V_RES              240         // LCD 垂直分辨率
#define LCD_PIXEL_CLOCK_HZ  (20 * 1000 * 1000) // SPI 时钟频率
#define LCD_CMD_BITS            8           // LCD 命令的比特数
#define LCD_PARAM_BITS          8           // LCD 参数的比特数

#define PIN_NUM_CS             41
#define PIN_NUM_DC             40
#define PIN_NUM_RST            45
#define PIN_NUM_BK_LIGHT       42


void vSPIInit(void);






#endif