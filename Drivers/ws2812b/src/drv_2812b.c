#include "drv_2812b.h"

static ws2812b_handle_t* ws2812 = NULL;
static uint8_t *temp_buffer = NULL;
static uint32_t temp_len = 0;

uint8_t hw_ws2812b_config(ws2812b_handle_t* handle, uint8_t *buffer, uint32_t buffer_len)
{
    uint8_t res;

    if(handle == NULL){
        ws2812b_interface_debug_print("ws2812b: handle is NULL.\r\n");
        return 1;
    }
    ws2812 = handle;

    if(buffer == NULL){
        ws2812b_interface_debug_print("ws2812b: buffer is NULL.\r\n");
        return 1;
    }
    temp_buffer = buffer;

    if(buffer_len == 0){
        ws2812b_interface_debug_print("ws2812b: buffer_len is 0.\r\n");
        return 1;
    }
    temp_len = buffer_len;
    
    /* link interface function */
    DRIVER_WS2812B_LINK_INIT(handle, ws2812b_handle_t);
    DRIVER_WS2812B_LINK_SPI_10MHZ_INIT(handle, ws2812b_interface_spi_10mhz_init);
    DRIVER_WS2812B_LINK_SPI_DEINIT(handle, ws2812b_interface_spi_deinit);
    DRIVER_WS2812B_LINK_SPI_WRITE_COMMAND(handle, ws2812b_interface_spi_write_cmd);
    DRIVER_WS2812B_LINK_DELAY_MS(handle, ws2812b_interface_delay_ms);
    DRIVER_WS2812B_LINK_DEBUG_PRINT(handle, ws2812b_interface_debug_print);
    
    /* ws2812b initialization */
    res = ws2812b_init(handle);
    if (res != 0)
    {
        ws2812b_interface_debug_print("ws2812b: init failed.\r\n");
       
        return 1;
    }
    
    return 0;
}

uint8_t hw_ws2812b_set_color(uint32_t *color, uint32_t len)
{
    return ws2812b_write(ws2812, color, len, temp_buffer, temp_len);
}

uint8_t array_rotate_left(uint32_t *arr, size_t size, size_t shift)
{
    if (size <= 1 || shift == 0) {
        return 1;
    }
    
    // 确保shift在数组大小范围内
    shift = shift % size;
    
    // 临时存储需要移动的元素
    uint32_t temp[shift];
    for (size_t i = 0; i < shift; i++) {
        temp[i] = arr[i];
    }
    
    // 移动剩余元素
    for (size_t i = shift; i < size; i++) {
        arr[i - shift] = arr[i];
    }
    
    // 将临时存储的元素放到数组末尾
    for (size_t i = 0; i < shift; i++) {
        arr[size - shift + i] = temp[i];
    }
    return 0;
}

uint8_t hw_ws2812_flow_light(uint32_t *colors, uint8_t color_len, uint8_t led_num)
{
    // 安全检查
    if (led_num > 32 || colors == NULL || color_len == 0) {
        return 1;
    }
    
    // 循环左移颜色数组
    if (array_rotate_left(colors, led_num, 1) != 0) {
        return 1;
    }
    
    // 直接发送颜色数组到WS2812B
    return hw_ws2812b_set_color(colors, led_num);
}

uint8_t hw_ws2812b_set_color_with_brigh(uint32_t base_color, uint8_t led_num, uint8_t brightness)
{
    uint32_t led_buffer[32];
    uint32_t color;
    uint8_t r, g, b;
    
    if (led_num > 32) {
        return 1;
    }
    
    // 提取RGB分量
    r = (base_color >> 16) & 0xFF;
    g = (base_color >> 8) & 0xFF;
    b = base_color & 0xFF;
    
    // 根据亮度调整RGB值
    r = (r * brightness) / 255;
    g = (g * brightness) / 255;
    b = (b * brightness) / 255;
    
    // 合成新的颜色值
    color = (r << 16) | (g << 8) | b;
    
    // 设置所有LED为相同颜色
    for(uint8_t i = 0; i < led_num; i++) {
        led_buffer[i] = color;
    }
    
    return hw_ws2812b_set_color(led_buffer, led_num);
}
