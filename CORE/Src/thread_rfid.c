#include "thread_rfid.h"
#include "basic_os.h"
#include "thread_comm.h"

extern struct picc_a_struct PICC_A;
extern struct picc_b_struct PICC_B;

uint8_t TYPE_A_CARD_EVENT(void)
{
    uint8_t result;
    FM175XX_SoftReset();
    FM175XX_Initial_ReaderA();
    SetCW(TX1_TX2_CW_ENABLE);
    result = ReaderA_CardActivate();
    // if(result != FM175XX_SUCCESS){
    //     ;
    // }
    SetCW(TX1_TX2_CW_DISABLE);
    return result;
}

uint8_t TYPE_B_CARD_EVENT(void)
{
    uint8_t result;
    FM175XX_Initial_ReaderB();
    SetCW(TX1_TX2_CW_ENABLE);
    result = ReaderB_Wakeup();
//    result = ReaderB_Request();
    if(result != FM175XX_SUCCESS){
        SetCW(TX1_TX2_CW_DISABLE);
        return result;
    }
    result = ReaderB_Attrib();
    if(result != FM175XX_SUCCESS){
        SetCW(TX1_TX2_CW_DISABLE);
        return result;
    }

    result = ReaderB_GetUID();
    if(result != FM175XX_SUCCESS){
        SetCW(TX1_TX2_CW_DISABLE);
        return result;
    }

    SetCW(TX1_TX2_CW_DISABLE);
    return result;
}

void task_entry_rfid(void *parameter)
{
    uint8_t result;
    uint8_t polling_card;
    uint8_t card_detected_flag = RESET; // 记录是否检测到卡片(用来实现"只检测一次")

    rfid_init();
    FM175XX_HardReset();

    for(;;){
        bos_delay_ms(200);

        if(Reader_Polling(&polling_card) != FM175XX_SUCCESS){
            card_detected_flag = RESET;
            continue;
        }

        if(card_detected_flag == SET) continue;
        
        if(polling_card & BIT0){ // TYPE A
            result = TYPE_A_CARD_EVENT();
            if(FM175XX_SUCCESS == result){
                send_card(PICC_A.UID, 4);
                print_id(PICC_A.UID, 4);
                card_detected_flag = SET;
            }
        }else if(polling_card & BIT1){ // TYPE B
            result = TYPE_B_CARD_EVENT();
            if(FM175XX_SUCCESS == result){
                send_card(PICC_B.UID, 8);
                print_id(PICC_A.UID, 4);
                card_detected_flag = SET;
            }
        }
    }
}
bos_task_export(rfid, task_entry_rfid, BOS_MAX_PRIORITY, NULL);
