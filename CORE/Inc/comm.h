#pragma once

#include "gd32l23x.h"

void comm_usart_init(void);
void UART_Transmit(uint32_t usart_periph, const uint8_t *pData, uint16_t Size);
