#pragma once

#include "driver_ws2812b.h"
#include "driver_ws2812b_interface.h"

uint8_t hw_ws2812b_config(ws2812b_handle_t* handle, uint8_t *buffer, uint32_t buffer_len);

/**
 * @brief  设置颜色
 * @param  color: 颜色数组
 * @param  len: 颜色数组长度
 * @return None
 */
uint8_t hw_ws2812b_set_color(uint32_t *color, uint32_t len);

/**
 * @brief  uint32_t 数组循环左移
 * @param  arr: 数组首地址
 * @param  size: 数组大小
 * @param  shift: 左移位数
 * @return None
 */
uint8_t array_rotate_left(uint32_t *arr, size_t size, size_t shift);

/**
 * @brief  设置颜色的同时，将颜色数组循环左移一位(用来实现流水灯效果)
 * @param  colors: 颜色数组
 * @param  color_len: 颜色数组长度
 * @param  led_num: 灯珠数量
 * @param  offset: 偏移量
 * @return None
 */
uint8_t hw_ws2812_flow_light(uint32_t *colors, uint8_t color_len, uint8_t led_num);

/**
 * @brief  设置颜色和亮度
 * @param  base_color: 基础颜色
 * @param  led_num: 灯珠数量
 * @param  brightness: 亮度
 * @return None
 */
uint8_t hw_ws2812b_set_color_with_brigh(uint32_t base_color, uint8_t led_num, uint8_t brightness);
