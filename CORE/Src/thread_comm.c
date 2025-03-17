#include "thread_comm.h"
#include <stddef.h>
#include <stdbool.h>
#include "lwrb.h"
#include "basic_os.h"

#define LOG_TAG "app.comm"
#include "elog.h"

#define COMM_BUFF_LENGTH   64

static void comm_dma_init(void);

/**
 * @brief   中断源
 */
enum {
    S_UART = 1,
    S_DMA = 0,
};

/* ring buffer for uart */
static lwrb_t recv_rb;
static uint8_t lwrb_buffer[COMM_BUFF_LENGTH]  = { 0x00 };
__IO static uint8_t rx_cplt_flag = false;
/* dma buffer */
static uint8_t dma_buff[COMM_BUFF_LENGTH] = { 0x00 };

static uint8_t rx_data = 0;
static COMM_RX_DATA comm_rx_data = {.state = STATUS_IDLE};
static uint8_t rx_length = 0;
static void task_entry_comm_receive(void *parameter)
{
    log_d("comm receive task start.");
    for(;;){
        bos_delay_ms(50);
        rx_length = lwrb_get_full(&recv_rb);
        while(rx_length > 0){
            lwrb_read(&recv_rb, &rx_data, 1);
            switch (comm_rx_data.state)
            {
            case STATUS_IDLE:
                if(rx_data == 0xAA){
                    comm_rx_data.frame_sum += rx_data;
                    comm_rx_data.state = STATUS_HEAD1;
                }else{
                    log_e("head1 error: 0x%02X.", rx_data);
                    comm_rx_data.frame_sum = 0;
                    comm_rx_data.state = STATUS_IDLE;
                }
                break;
            case STATUS_HEAD1:
                if(rx_data == 0x55){
                    comm_rx_data.frame_sum += rx_data;
                    comm_rx_data.state = STATUS_HEAD;
                }else{
                    log_e("head2 error: 0x%02X.", rx_data);
                    comm_rx_data.frame_sum = 0;
                    comm_rx_data.state = STATUS_IDLE;
                }
                break;
            case STATUS_HEAD:
                comm_rx_data.frame_sum += rx_data;
                comm_rx_data.cmd = rx_data;
                comm_rx_data.state = STATUS_CMD;
                comm_rx_data.payloadCnt = 0;    // 重置payloadCnt计数
                break;
            case STATUS_CMD:
                if(0 == rx_data){   // 如果是0, 表示没有payload， 直接进入tail状态
                    comm_rx_data.state = STATUS_TAIL;
                    break;
                }
                if(0 < rx_data && rx_data <= PAYLOAD_MAX){
                    comm_rx_data.frame_sum += rx_data;
                    comm_rx_data.payloadLen = rx_data;
                    comm_rx_data.state = STATUS_DATA;
                }else{
                    log_e("length error: %d.", rx_data);
                    comm_rx_data.frame_sum = 0;
                    comm_rx_data.state = STATUS_IDLE;
                }
                break;
            case STATUS_DATA:
                if(comm_rx_data.payloadCnt < comm_rx_data.payloadLen){
                    comm_rx_data.frame_sum += rx_data;
                    comm_rx_data.payload[comm_rx_data.payloadCnt++] = rx_data;
                    if(comm_rx_data.payloadCnt == comm_rx_data.payloadLen){ // 最后一个数据
                        comm_rx_data.state = STATUS_TAIL;
                    }
                }
                break;
            case STATUS_TAIL:
                if(rx_data == (uint8_t)(comm_rx_data.frame_sum&0x00FF)){
                    switch (comm_rx_data.cmd)
                    {
                    case COMM_CMD_UPDATE_STATE:
                        log_d("update evse state.");
                        if(comm_rx_data.payloadLen == 0)
                            log_w("payload len is 0.");
                        break;
                    default:
                        log_e("unknown cmd: 0x%02X.", comm_rx_data.cmd);
                        break;
                    }
                }else{
                    log_e("checksum error: 0x%02X, should be: 0x%02X.", rx_data, (uint8_t)(comm_rx_data.frame_sum&0x00FF));
                }
                comm_rx_data.frame_sum = 0;
                comm_rx_data.state = STATUS_IDLE;
                break;
            default:
                log_e("unknown state: %d.", comm_rx_data.state);
                comm_rx_data.frame_sum = 0;
                comm_rx_data.state = STATUS_IDLE;
                break;
            }
            rx_length--;
        }
    }
}
bos_task_export(comm_receive, task_entry_comm_receive, BOS_MAX_PRIORITY, NULL);

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

    usart_flag_clear(USART0, USART_FLAG_TBE);
    usart_flag_clear(USART0, USART_FLAG_IDLE);
    usart_interrupt_flag_clear(USART0, USART_INT_FLAG_IDLE);
    /* enable USART interrupt */
    usart_interrupt_enable(USART0, USART_INT_IDLE);
    usart_flag_clear(USART0, USART_FLAG_TBE);
    usart_flag_clear(USART0, USART_FLAG_IDLE);
    usart_interrupt_flag_clear(USART0, USART_INT_FLAG_IDLE);
    nvic_irq_enable(USART0_IRQn, 0);

    usart_enable(USART0);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void comm_dma_init(void)
{
    dma_parameter_struct dma_init_struct;
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);

    /* initialize DMA channel 1 */
    dma_deinit(DMA_CH0);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.request      = DMA_REQUEST_USART0_RX;
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr  = (uint32_t)dma_buff;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number       = COMM_BUFF_LENGTH;
    dma_init_struct.periph_addr  = (uint32_t)(&USART_RDATA(USART0));
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH0, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_enable(DMA_CH0);
    dma_memory_to_memory_disable(DMA_CH0);
    /* disable the DMAMUX_MUXCH0 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH0);
    /* enable DMA channel 0 */
    dma_channel_enable(DMA_CH0);
    /* USART DMA enable for reception */
    usart_dma_receive_config(USART0, USART_RECEIVE_DMA_ENABLE);
    /* enable DMA channel 0 transfer complete interrupt */
    dma_interrupt_enable(DMA_CH0, DMA_INT_HTF|DMA_INT_FTF);
    /* enable nvic */
    nvic_irq_enable(DMA_Channel0_IRQn, 0);
    /* enable DMA channel 0 */
    dma_channel_enable(DMA_CH0);
}

void comm_init()
{
    lwrb_init(&recv_rb, lwrb_buffer, COMM_BUFF_LENGTH);
    comm_usart_init();
    comm_dma_init();
}

/**
 * @brief       串口空闲中断和DMA中断回调函数
 * @note        串口和DMA中断都会调用该函数, 用来处理DMA缓冲区里面的数据。
 * @param[in]   size dma缓冲区已经使用的大小
 * @param[in]   it_source 对于串口只开启超时中断或者空闲中断的应用来说, it_source可以作为帧结束的标识。
 *                  1: 表示本次是在串口空闲中断里面调用的;
 *                  0: 表示本次是在DMA中断里面调用的.
*/
void COMM_RxEventCallback(uint32_t Size, uint16_t it_source)
{
    static uint32_t dma_buf_pos = 0;    //  本次接收到的数据在dma缓冲区中的起始位置
    static uint32_t Rx_length = 0;      //  本次收到的数据长度

    Rx_length = Size - dma_buf_pos;

    if(Rx_length > 0)
        lwrb_write(&recv_rb, dma_buff+dma_buf_pos, (lwrb_sz_t)Rx_length);

    if(it_source == S_UART)
        rx_cplt_flag = true; // 空闲中断或者超时中断表示一帧结束

    dma_buf_pos += Rx_length;

    if (dma_buf_pos >= COMM_BUFF_LENGTH) dma_buf_pos = 0;
}

/**
 * @brief   串口中断服务函数, DMA中断服务函数, 该函数会调用Tuya_RxEventCallback(), it_source固定传1
 * @note    计量模块的串口设备只开启了空闲中断
 * @param   none
 * @retval  none
*/
void USART0_IRQHandler(void){
    if(usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE)){
        /* 清除 IDLE 标志位 */
        USART_STAT(USART0);
        USART_RDATA(USART0);
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_IDLE);
        usart_flag_clear(USART0, USART_FLAG_IDLE);
        /* dma缓冲区的总大小 - dma缓冲区已经使用的大小 = dma缓冲区剩余空间 */
        COMM_RxEventCallback(COMM_BUFF_LENGTH - dma_transfer_number_get(DMA_CH0), S_UART);
    }
}

/**
 * @brief   DMA中断服务函数DMA中断服务函数, 该函数会调用Tuya_RxEventCallback()。
 * @note    由于DMA开启的是半满和全满中断, 所以在传递DMA缓冲区已经使用的大小时, 
 *          直接传递缓冲区大小的一半或者缓冲区的大小即可。 中断源固定传递0。
 * @param   none
 * @retval  none
*/
void DMA_Channel0_IRQHandler(void)
{
    if(RESET != dma_interrupt_flag_get(DMA_CH0, DMA_INT_FLAG_FTF)){
        dma_interrupt_flag_clear(DMA_CH0, DMA_INT_FLAG_FTF);
        COMM_RxEventCallback(COMM_BUFF_LENGTH, S_DMA);
    }else if(RESET != dma_interrupt_flag_get(DMA_CH0, DMA_INT_FLAG_HTF)){
        dma_interrupt_flag_clear(DMA_CH0, DMA_INT_FLAG_HTF);
        COMM_RxEventCallback(COMM_BUFF_LENGTH/2, S_DMA);
    }
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
