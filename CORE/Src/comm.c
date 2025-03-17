#include "comm.h"
#include <stddef.h>

void comm_usart_init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* connect port to USART TX */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9);
    /* connect port to USART RX */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_10);

    /* configure USART TX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9);

    /* configure USART RX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_10);

    /* USART configure */
    usart_deinit(USART0);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_baudrate_set(USART0, 115200U);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);

    usart_enable(USART0);
}

/**
 * @brief  Sends an amount of data in blocking mode.(不带超时检测)
*/
void UART_Transmit(uint32_t usart_periph, const uint8_t *pData, uint16_t Size)
{
    const uint8_t  *pdata8bits = pData;
    uint16_t count = Size;
    if ((pData == NULL) || (Size == 0U))
    {
        return;
    }
    while (count > 0U)
    {
        USART_TDATA(usart_periph) = USART_TDATA_TDATA & *pdata8bits;
        while(RESET == (USART_REG_VAL(usart_periph, USART_FLAG_TBE) & BIT(USART_BIT_POS(USART_FLAG_TBE))));
        pdata8bits++;
        count--;
    }
}
