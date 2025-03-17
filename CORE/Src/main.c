#include "gd32l23x.h"
#include "systick.h"
#include "main.h"
#include "gd32l23x_eval.h"
#include "basic_os.h"
#include <stddef.h>
#include "printf.h"
#include "elog.h"
#include "ws2812_thread.h"
#include "thread_comm.h"

/* Stack for BasicOS */
__attribute__((used)) uint8_t stack[5120];

int main(void)
{
    elog_init();
    elog_start();
    comm_init();
    log_i("Hello, World!");
    /* configure systick */
    systick_config();
    /* 启动BasicOS */
    basic_os_init(stack, sizeof(stack));
    basic_os_run();

    return 0;
}

bos_task_export(ws2812, task_entry_ws2812, BOS_MAX_PRIORITY, NULL);
