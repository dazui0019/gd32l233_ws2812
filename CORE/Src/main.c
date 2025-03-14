#include "gd32l23x.h"
#include "systick.h"
#include "main.h"
#include "gd32l23x_eval.h"
#include "basic_os.h"
#include <stddef.h>
#include "SEGGER_RTT.h"

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
    SEGGER_RTT_printf(0, "Hello World!\r\n");
    /* configure systick */
    systick_config();
    /* 启动BasicOS */
    basic_os_init(stack, sizeof(stack));
    basic_os_run();

    return 0;
}
