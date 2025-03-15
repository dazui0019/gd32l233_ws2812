#ifndef __FM175XX_H
#define __FM175XX_H

#include "gd32l23x.h"
#include "systick.h"
#include "fm175xx_reg.h"

#ifndef true
#define true				TRUE
#endif

#ifndef false
#define false				FALSE
#endif

#ifndef True
#define True				TRUE
#define False				FALSE
#endif

#ifndef uchar
#define uchar               u8
#define uint				u16
#endif

#define BIT0                0x01
#define BIT1                0x02
#define BIT2                0x04
#define BIT3                0x08
#define BIT4                0x10
#define BIT5                0x20
#define BIT6                0x40
#define BIT7                0x80

// FM175XX interrupt
#define PORT_IRQ            GPIOC
#define PIN_IRQ             GPIO_Pin_12

// FM175XX reset
#define RCU_PORT_NRST       RCU_GPIOB
#define PORT_NRST           GPIOB
#define PIN_NRST            GPIO_PIN_6

// FM175XX SPI
#define RCU_PORT_NSS        RCU_GPIOA
#define PORT_NSS            GPIOA
#define PIN_NSS             GPIO_PIN_15

#define TX1_TX2_CW_DISABLE      0
#define TX1_CW_ENABLE           1
#define TX2_CW_ENABLE           2
#define TX1_TX2_CW_ENABLE       3
#define FM175XX_SUCCESS         0x00
#define FM175XX_FIFO_ERROR		0xC1
#define FM175XX_RESET_ERROR		0xC2
#define FM175XX_DP              0xC3
#define FM175XX_HP              0xC4

#define FM175XX_SELECT()        gpio_bit_reset(PORT_NSS, PIN_NSS)
#define FM175XX_DESELECT()      gpio_bit_set(PORT_NSS, PIN_NSS)

#define FM_175XX_RST_LOW()      gpio_bit_reset(PORT_NRST, PIN_NRST)
#define FM_175XX_RST_HIGH()     gpio_bit_set(PORT_NRST, PIN_NRST)

void rfid_init(void);
void print_id(const uint8_t id[], uint8_t length);
void send_card(const uint8_t card_id[], uint8_t length);
void UART_Transmit(uint32_t usart_periph, const uint8_t *pData, uint16_t Size);

void FM175XX_HardReset(void);
uint8_t FM175XX_SoftReset(void);
void GetReg(uint8_t reg_address, uint8_t* reg_data);
void SetReg(uint8_t reg_address, uint8_t reg_data);
void ModifyReg(uint8_t reg_address, uint8_t mask, uint8_t set);
void SetCW(uint8_t mode);
uint8_t ReadFIFOData(uint8_t *fifo_data, uint8_t fifo_data_length);
uint8_t WriteFIFOData(uint8_t *fifo_data, uint8_t fifo_data_length);
uint8_t FM175XX_HardPowerdown(uint8_t mode);
uint8_t FM175XX_SoftPowerdown(void);

void fms175xx_delay_ms(uint32_t ms);

#endif