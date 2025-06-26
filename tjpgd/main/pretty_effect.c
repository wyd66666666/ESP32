/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <math.h>                // 引入数学库，提供sin等函数
#include "pretty_effect.h"       // 引入本模块头文件
#include "sdkconfig.h"           // 引入SDK配置头文件
#include "decode_image.h"        // 引入图像解码头文件

uint16_t *pixels;                // 指向解码后像素数据的指针

//Grab a rgb16 pixel from the esp32_tiles image
//从esp32_tiles图像中抓取rgb16像素
static inline uint16_t get_bgnd_pixel(int x, int y) // 获取指定坐标的像素颜色
{
    //Get color of the pixel on x,y coords
    //获取x、y坐标上像素的颜色
    return (uint16_t) * (pixels + (y * IMAGE_W) + x); // 计算像素在数组中的位置并返回其值
}

//This variable is used to detect the next frame.
static int prev_frame = -1;      // 记录上一次处理的帧编号

//我们不计算每个像素的偏移量，而是在帧发生变化时预先计算值，然后重复使用
//当我们遍历帧中的所有像素时。这要快得多。
static int8_t xofs[240], yofs[240];     // 存储每个x/y的偏移量
static int8_t xcomp[240], ycomp[240];   // 存储每个x/y的补偿量

//计算一组行的像素数据（隐含行大小为240）。像素位于dest，line是像素的Y坐标
//要计算的第一行，linect是要计算的行数。每次整个图像增加一帧
//显示；这用于转到下一帧动画。
void pretty_effect_calc_lines(uint16_t *dest, int line, int frame, int linect)
{
    if (frame != prev_frame) {           // 如果当前帧与上一帧不同
        //我们需要计算一组新的偏移系数。取一些随机正弦作为偏移，使一切
        //看起来很漂亮，很流畅。
        for (int x = 0; x < 240; x++) {  // 遍历所有x坐标
            xofs[x] = sin(frame * 0.15 + x * 0.06) * 4; // 计算x方向偏移
        }
        for (int y = 0; y < 240; y++) {  // 遍历所有y坐标
            yofs[y] = sin(frame * 0.1 + y * 0.05) * 4;  // 计算y方向偏移
        }
        for (int x = 0; x < 240; x++) {  // 遍历所有x坐标
            xcomp[x] = sin(frame * 0.11 + x * 0.12) * 4; // 计算x方向补偿
        }
        for (int y = 0; y < 240; y++) {  // 遍历所有y坐标
            ycomp[y] = sin(frame * 0.07 + y * 0.15) * 4; // 计算y方向补偿
        }
        prev_frame = frame;              // 更新上一帧编号
    }
    for (int y = line; y < line + linect; y++) { // 遍历要处理的每一行
        for (int x = 0; x < 240; x++) {          // 遍历每一行的每个像素
            //获取每个点的颜色
            *dest++ = get_bgnd_pixel(x + yofs[y] + xcomp[x], y + xofs[x] + ycomp[y]); // 计算偏移后的位置并获取像素颜色
        }
    }
}

esp_err_t pretty_effect_init(void) // 初始化特效，解码图片
{
    return decode_image(&pixels);  // 调用解码函数，获取像素数据
}
