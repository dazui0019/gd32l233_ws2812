#include "ws2812_thread.h"
#include "basic_os.h"
#include "SEGGER_RTT.h"

static ws2812b_handle_t ws2812b;
static uint8_t temp_buffer[1024];  // 临时缓冲区

static uint32_t blue_gradient[] = {
    0x0000FF,    // 100%
    0x0000F3,    // 95%
    0x0000E6,    // 90%
    0x0000DA,    // 85%
    0x0000CC,    // 80%
    0x0000C0,    // 75%
    0x0000B3,    // 70%
    0x0000A6,    // 65%
    0x000099,    // 60%
    0x00008C,    // 55%
    0x000080,    // 50%
    0x000073,    // 45%
    0x000066,    // 40%
    0x00005A,    // 35%
    0x00004D,    // 30%
    0x000040,    // 25%
    0x000033,    // 20%
    0x000040,    // 25%
    0x00004D,    // 30%
    0x00005A,    // 35%
    0x000066,    // 40%
    0x000073,    // 45%
    0x000080,    // 50%
    0x00008C,    // 55%
    0x000099,    // 60%
    0x0000A6,    // 65%
    0x0000B3,    // 70%
    0x0000C0,    // 75%
    0x0000CC,    // 80%
    0x0000DA,    // 85%
    0x0000E6,    // 90%
    0x0000F3,    // 95%
};

static void task_entry_ws2812(void *parameter)
{
    uint8_t brightness = 0;
    uint8_t increasing = 1;
    
    // 初始化WS2812B
    if (hw_ws2812b_config(&ws2812b, temp_buffer, 1024) != 0) {
        return;
    }
    
    for(;;) {
        // 呼吸效果
        hw_ws2812b_set_color_with_brigh(0x0000FF, 12, brightness);  // 使用蓝色
        
        // 更新亮度
        if (increasing) {
            brightness++;
            if (brightness >= 128) {
                increasing = 0;
            }
        } else {
            brightness--;
            if (brightness == 10) {
                increasing = 1;
            }
        }

        bos_delay_ms(10);
    }
}
bos_task_export(ws2812, task_entry_ws2812, BOS_MAX_PRIORITY, NULL);
