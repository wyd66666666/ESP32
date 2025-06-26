/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

/*
The image used for the effect on the LCD in the SPI master example is stored in flash
as a jpeg file. This file contains the decode_image routine, which uses the tiny JPEG
decoder library to decode this JPEG into a format that can be sent to the display.

Keep in mind that the decoder library cannot handle progressive files (will give
``Image decoder: jd_prepare failed (8)`` as an error) so make sure to save in the correct
format if you want to use a different image file.
*/

#include "decode_image.h"              // 引入解码图像的头文件
#include "jpeg_decoder.h"              // 引入 JPEG 解码器头文件
#include "esp_log.h"                   // 引入 ESP 日志库
#include "esp_check.h"                 // 引入 ESP 错误检查库
#include <string.h>                    // 引入字符串操作库
#include "freertos/FreeRTOS.h"         // 引入 FreeRTOS 头文件

//Reference the binary-included jpeg file
extern const uint8_t image_jpg_start[] asm("_binary_image_jpg_start"); // 声明嵌入式 JPEG 文件起始地址
extern const uint8_t image_jpg_end[] asm("_binary_image_jpg_end");     // 声明嵌入式 JPEG 文件结束地址
//Define the height and width of the jpeg file. Make sure this matches the actual jpeg
//dimensions.

const char *TAG = "ImageDec";          // 定义日志标签

//Decode the embedded image into pixel lines that can be used with the rest of the logic.
//将嵌入的图像解码为可以与其余逻辑一起使用的像素线。
esp_err_t decode_image(uint16_t **pixels) // 解码图像函数，输出像素数据
{
    *pixels = NULL;                    // 初始化像素指针为 NULL
    esp_err_t ret = ESP_OK;            // 初始化返回值为 ESP_OK

    //Alocate pixel memory. Each line is an array of IMAGE_W 16-bit pixels; the `*pixels` array itself contains pointers to these lines.
    *pixels = calloc(IMAGE_H * IMAGE_W, sizeof(uint16_t)); // 分配存储所有像素的内存
    ESP_GOTO_ON_FALSE((*pixels), ESP_ERR_NO_MEM, err, TAG, "Error allocating memory for lines"); // 检查内存分配是否成功，失败则跳转到 err

    //JPEG decode config
    esp_jpeg_image_cfg_t jpeg_cfg = {  // 配置 JPEG 解码参数
        .indata = (uint8_t *)image_jpg_start,                // 输入数据起始地址
        .indata_size = image_jpg_end - image_jpg_start,      // 输入数据大小
        .outbuf = (uint8_t*)(*pixels),                       // 输出缓冲区地址
        .outbuf_size = IMAGE_W * IMAGE_H * sizeof(uint16_t), // 输出缓冲区大小
        .out_format = JPEG_IMAGE_FORMAT_RGB565,              // 输出格式为 RGB565
        .out_scale = JPEG_IMAGE_SCALE_0,                     // 不缩放输出
        .flags = {
            .swap_color_bytes = 1,                           // 交换颜色字节顺序
        }
    };

    //JPEG decode
    esp_jpeg_image_output_t outimg;      // 定义 JPEG 解码输出结构体
    esp_jpeg_decode(&jpeg_cfg, &outimg); // 调用解码函数进行解码

    ESP_LOGI(TAG, "JPEG image decoded! Size of the decoded image is: %dpx x %dpx", outimg.width, outimg.height); // 打印解码后图像的尺寸

    return ret;                          // 返回解码结果
err:
    //Something went wrong! Exit cleanly, de-allocating everything we allocated.
    if (*pixels != NULL) {               // 如果像素内存已分配
        free(*pixels);                   // 释放像素内存
    }
    return ret;                          // 返回错误码
}
