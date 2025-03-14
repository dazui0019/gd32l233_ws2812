#include "ws2812_thread.h"
#include "basic_os.h"
#include "SEGGER_RTT.h"

static ws2812b_handle_t ws2812b;
static uint8_t temp_buffer[1024];  // 临时缓冲区

void task_entry_ws2812(void *parameter)
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
