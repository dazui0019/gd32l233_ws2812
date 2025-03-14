/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_ws2812b_interface_template.c
 * @brief     driver ws2812b interface template source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2021-11-13
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2021/11/13  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_ws2812b_interface.h"
#include "gd32l23x.h"
#include "basic_os.h"
#include "SEGGER_RTT.h"
#include <stdlib.h>
#include <stdarg.h>

/**
 * @brief  interface spi 10mhz bus init
 * @return status code
 *         - 0 success
 *         - 1 spi init 10mhz failed
 * @note   none
 */
uint8_t ws2812b_interface_spi_10mhz_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    /* configure SPI0 GPIO : SCK/PA5, MOSI/PA7 */
    gpio_af_set(GPIOA, GPIO_AF_5,  GPIO_PIN_5);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
    gpio_af_set(GPIOA, GPIO_AF_5,  GPIO_PIN_7);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);

    spi_parameter_struct spi_init_struct;
    
    rcu_periph_clock_enable(RCU_SPI0);
    /* deinitilize SPI and the parameters */
    spi_i2s_deinit(SPI0);
    spi_struct_para_init(&spi_init_struct);

    /* SPI0 parameter configuration */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_4;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

    /* configure SPI1 byte access to FIFO */
    spi_fifo_access_size_config(SPI0, SPI_BYTE_ACCESS);
    /* enable SPI0 */

    spi_bidirectional_transfer_config(SPI0, SPI_BIDIRECTIONAL_TRANSMIT);

    spi_enable(SPI0);
    
    return 0;
}

/**
 * @brief  interface spi bus deinit
 * @return status code
 *         - 0 success
 *         - 1 spi deinit failed
 * @note   none
 */
uint8_t ws2812b_interface_spi_deinit(void)
{
    spi_i2s_deinit(SPI0);
    rcu_periph_clock_disable(RCU_SPI0);
    return 0;
}

/**
 * @brief     interface spi bus write command
 * @param[in] *buf pointer to a data buffer
 * @param[in] len length of data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t ws2812b_interface_spi_write_cmd(uint8_t *pData, uint16_t Size)
{
    uint8_t errorcode = 0;
    uint16_t TxXferCount;
    uint8_t* pTxBuffPtr;

    if ((pData == NULL) || (Size == 0U))
    {
        errorcode = 1;
        goto error;
    }

    TxXferCount = Size;
    pTxBuffPtr = (uint8_t*)pData;

    while (TxXferCount > 0U){
        if(RESET != spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)){
            // SPI_DATA(SPI0) = (uint32_t)*pTxBuffPtr;
            spi_i2s_data_transmit(SPI0, *pTxBuffPtr);
            TxXferCount--;
            pTxBuffPtr++;
        }
    }

    /* 等待传输完成 */
    while (RESET != spi_i2s_flag_get(SPI0, SPI_FLAG_TRANS)){}
    // spi_i2s_flag_get(SPI0, SPI_FLAG_TRANS);

error :
    return errorcode;
}

/**
 * @brief     interface delay ms
 * @param[in] ms time
 * @note      none
 */
void ws2812b_interface_delay_ms(uint32_t ms)
{
    bos_delay_ms(ms);
}

/**
 * @brief     interface print format data
 * @param[in] fmt format data
 * @note      none
 */
void ws2812b_interface_debug_print(const char *const fmt, ...)
{
    va_list ParamList;
  
    va_start(ParamList, fmt);
    SEGGER_RTT_vprintf(0, fmt, &ParamList);
    va_end(ParamList);
}
