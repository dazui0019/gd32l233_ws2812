#include "gd32l23x.h"
#include "systick.h"
#include "main.h"
#include "gd32l23x_eval.h"
#include "basic_os.h"
#include <stddef.h>

/* Stack for BasicOS */
__attribute__((used)) uint8_t stack[10240];

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{

    /* configure systick */
    systick_config();
    /* 启动BasicOS */
    basic_os_init(stack, sizeof(stack));
    basic_os_run();

    return 0;
}

static void task_entry_blink(void *parameter)
{
    for(;;){
        bos_delay_ms(500);
    }
}
bos_task_export(blink, task_entry_blink, BOS_MAX_PRIORITY, NULL);
