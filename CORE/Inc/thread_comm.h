#pragma once

#include "gd32l23x.h"

// 数据帧结构
// 帧头1 帧头2 命令 数据长度 数据(当数据长度为0时，需要舍弃该位) 校验和(作为帧尾)
#define FRAME_HEAD1                 0xAA    // 帧头1
#define FRAME_HEAD2                 0x55    // 帧头2

/* 按键控制命令 */
#define COMM_CMD_KEY_PRESS          0x3A    // 按键功能
#define COMM_CMD_RFID               0x28    // 卡号
#define COMM_CMD_UPDATE_STATE       0x3B    // 更新充电桩状态

#define FRAME_LEN_MAX               32      // 最大帧长度

#define PAYLOAD_MAX                 4       // 最大数据长度

/**
* @brief  Status of received communication frame
*/
typedef enum
{
    STATUS_IDLE = 0,
    STATUS_HEAD1, /* Rx Head1=0xAA */
    STATUS_HEAD, /* Rx Head2=0x55 */
    STATUS_CMD, /* Rx Type */
    // STATUS_LENGTH, /* Data length */
    STATUS_DATA, /* Data filed */
    // STATUS_SUM, /* Check sum */
    STATUS_TAIL, /* Tail=0x0D */
    // STATUS_END, /* End of this frame */
} COMM_STATUS_TypeDef;

/**
* @brief  Data object for received communication frame
*/
typedef struct
{
    COMM_STATUS_TypeDef state;  /* Status */
    uint8_t payloadCnt; /* Count of 1 field */
    uint8_t payloadLen; /* Length of data field */
    uint8_t cmd;        /* Command */
    uint8_t frame[FRAME_LEN_MAX];   /* Received frame */
    uint8_t payload[PAYLOAD_MAX];   /* Received data */
    uint16_t frame_sum; /* Check sum */
} COMM_RX_DATA;

void comm_init(void);
void UART_Transmit(uint32_t usart_periph, const uint8_t *pData, uint16_t Size);
