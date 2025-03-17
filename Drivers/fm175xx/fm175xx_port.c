#include "fm175xx_port.h"
#include <string.h>
#include "basic_os.h"

#define LOG_TAG "drv.fm175xx"
#include "elog.h"

void rfid_spi_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOA);

    /* configure SPI1 GPIO : SCK/PB3 */
    gpio_af_set(GPIOB, GPIO_AF_6,  GPIO_PIN_3);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    /* configure SPI1 GPIO : NSS/PA15 */
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    /* configure SPI1 GPIO : MISO/PB4, MOSI/PB5 */
    gpio_af_set(GPIOB, GPIO_AF_6,  GPIO_PIN_4 | GPIO_PIN_5);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5);

    spi_parameter_struct spi_init_struct;
    
    rcu_periph_clock_enable(RCU_SPI1);
    /* deinitilize SPI and the parameters */
    spi_i2s_deinit(SPI1);
    spi_struct_para_init(&spi_init_struct);

    /* SPI1 parameter configuration */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_64;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spi_init_struct);
    
    /* enable SPI1 */
    spi_enable(SPI1);
}

void rfid_init(void)
{
    rfid_spi_init();
    rcu_periph_clock_enable(RCU_PORT_NRST);
    // PB1:FM175XX NPD
    gpio_mode_set(PORT_NRST, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_NRST);
    gpio_output_options_set(PORT_NRST, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN_NRST);
}

void GetReg(uint8_t reg_address,uint8_t *reg_data)
{
    FM175XX_SELECT();   //NSS = 0;
    reg_address = (reg_address << 1) | 0x80;
    
    spi_i2s_data_transmit(SPI1, reg_address);   /* Send SPI1 data */ 
    //while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE)==RESET);  /* Wait for SPI1 Tx buffer empty */ 
    while(spi_i2s_flag_get(SPI1, SPI_FLAG_RBNE)==RESET);

    *reg_data = spi_i2s_data_receive(SPI1); /* Read SPI1 received data */
    
    spi_i2s_data_transmit(SPI1, 0x00);   /* Send SPI1 data */
    //while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE)==RESET)
    while(spi_i2s_flag_get(SPI1, SPI_FLAG_RBNE)==RESET);
    
    *reg_data = spi_i2s_data_receive(SPI1); /* Read SPI1 received data */

    FM175XX_DESELECT(); //NSS = 1;  
}

void SetReg(uint8_t reg_address,uint8_t reg_data)
{   
    FM175XX_SELECT();   //NSS = 0;
    reg_address = (reg_address << 1) & 0x7F;
    spi_i2s_data_transmit(SPI1, reg_address);   /* Send SPI1 data */
    //while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE)==RESET);    /* Wait for SPI1 Tx buffer empty */
    while(spi_i2s_flag_get(SPI1, SPI_FLAG_RBNE)==RESET);

    spi_i2s_data_receive(SPI1);      /* Read SPI1 received data */;
    spi_i2s_data_transmit(SPI1, reg_data);   /* Send SPI1 data */
    //while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE)==RESET);
    while(spi_i2s_flag_get(SPI1, SPI_FLAG_RBNE)==RESET);

    spi_i2s_data_receive(SPI1);      /* Read SPI1 received data */

    FM175XX_DESELECT(); //NSS = 1;
}

void ModifyReg(uint8_t reg_address,uint8_t mask,uint8_t set)
{
    uint8_t reg_data;
    
    GetReg(reg_address,&reg_data);

    if(set){
        reg_data |= mask;
    }else{
        reg_data &= ~mask;
    }

    SetReg(reg_address,reg_data);
}

void FM175XX_HardReset(void)
{   
    FM175XX_DESELECT();     //NSS = 1
    FM_175XX_RST_LOW(); //NPD = 0    
    fms175xx_delay_ms(1);      
    FM_175XX_RST_HIGH();
    fms175xx_delay_ms(1);
}

uint8_t FM175XX_SoftReset(void)
{   
    uint8_t reg_data;
    SetReg(JREG_COMMAND, CMD_SOFT_RESET);
    fms175xx_delay_ms(1);
    GetReg(JREG_COMMAND, &reg_data);
    if(reg_data == 0x20)
        return FM175XX_SUCCESS;
    else
        log_w("FM175XX SoftReset Error.");
    return FM175XX_RESET_ERROR;
}

void SetCW(uint8_t cw_mode)
{
    if(cw_mode == TX1_TX2_CW_DISABLE){
        ModifyReg(JREG_TXCONTROL,JBIT_TX1RFEN|JBIT_TX2RFEN,RESET);
    }
    if(cw_mode == TX1_CW_ENABLE){
        ModifyReg(JREG_TXCONTROL,JBIT_TX1RFEN,SET);
        ModifyReg(JREG_TXCONTROL,JBIT_TX2RFEN,RESET);   
    }
    if(cw_mode == TX2_CW_ENABLE){
        ModifyReg(JREG_TXCONTROL,JBIT_TX1RFEN,RESET);
        ModifyReg(JREG_TXCONTROL,JBIT_TX2RFEN,SET); 
    }
    if(cw_mode == TX1_TX2_CW_ENABLE){
        ModifyReg(JREG_TXCONTROL,JBIT_TX1RFEN|JBIT_TX2RFEN,SET);
    }
    fms175xx_delay_ms(30);
}

uint8_t ReadFIFOData(uint8_t *fifo_data,uint8_t fifo_data_length)
{
    uint8_t i,fifo_level;
    GetReg(JREG_FIFOLEVEL,&fifo_level);
    fifo_level = fifo_level & 0x3F;
    if (fifo_data_length > fifo_level)
        return FM175XX_FIFO_ERROR;
    for (i=0;i<fifo_data_length;i++)
        GetReg(JREG_FIFODATA,&fifo_data[i]);
    return FM175XX_SUCCESS;
}

uint8_t WriteFIFOData(uint8_t *fifo_data,uint8_t fifo_data_length)
{
    uint8_t i,fifo_level;
    GetReg(JREG_FIFOLEVEL,&fifo_level);
    fifo_level = fifo_level & 0x3F;
    if (fifo_data_length > (64 - fifo_level))
        return FM175XX_FIFO_ERROR;
    for (i=0;i<fifo_data_length;i++)
        SetReg(JREG_FIFODATA,*(fifo_data + i));
    return FM175XX_SUCCESS; 
}

uint8_t FM175XX_SoftPowerdown(void)
{
    uint8_t reg_data;
    GetReg(JREG_COMMAND,&reg_data);
    if (reg_data & 0x10){
        ModifyReg(JREG_COMMAND, 0x10,RESET);
        return FM175XX_HP;
    }else ModifyReg(JREG_COMMAND, 0x10,SET);
    
    return FM175XX_DP;
}

uint8_t FM175XX_HardPowerdown(uint8_t mode)
{
    if (mode == 0){
        FM_175XX_RST_HIGH();
        fms175xx_delay_ms(20);
        return FM175XX_HP;
    }else if (mode == 1){
        FM_175XX_RST_LOW();
        fms175xx_delay_ms(20);
    }
    return FM175XX_DP;
}

void print_id(const uint8_t id[], uint8_t length){
    log_i("Card ID: ");
    for(uint8_t i = 0; i < length; i++) {
        elog_raw("%02X ", id[i]);
    }
    elog_raw("\r\n");
}

uint8_t frame[20] = {0x55, 0xAA};   // 报头固定为0x55 0xAA
void send_card(const uint8_t card_id[], uint8_t length){
    uint16_t sum = 0;
    frame[2] = 0x30;
    frame[3] = length;
    memcpy(frame + 4, card_id, length);
    for(int i=0; i<(4+length); i++){
        sum += frame[i];
    }
    frame[4+length] = sum % 256;
    UART_Transmit(USART0, frame, 4+length+1);
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

void fms175xx_delay_ms(uint32_t ms)
{
    bos_delay_ms(ms);
}
