/*********************************************************************
*                                                                    *
*   Copyright (c) 2010 Shanghai FuDan MicroElectronic Inc, Ltd.      *
*   All rights reserved. Licensed Software Material.                 *
*                                                                    *
*   Unauthorized use, duplication, or distribution is strictly       *
*   prohibited by law.                                               *
*                                                                    *
*********************************************************************/

/********************************************************************/
/*  ��Ҫ����:                                                                                                       */
/*      TYPE A �� TYPE B��ƬPolling����                                                 */
/*      ֧������֤UID��ȡ                                                                               */
/*  ����:��ҫ��                                                                                                         */
/*  ����ʱ��:2017��9��7��                                                                                       */
/*                                                                                                                                  */
/********************************************************************/
#include "string.h"
#include "systick.h"
#include "fm175xx_port.h"
#include "fm175xx_reg.h"
#include "reader_api.h"

struct picc_a_struct PICC_A; 
struct picc_b_struct PICC_B;


//*************************************
//����  ����FM175XX_Initial_ReaderA
//��ڲ�����
//���ڲ�����
//*************************************

void FM175XX_Initial_ReaderA(void)
{   
    SetReg(JREG_MODWIDTH,MODWIDTH_106); //MODWIDTH = 106kbps
    ModifyReg(JREG_TXAUTO,BIT6,SET);//Force 100ASK = 1
    SetReg(JREG_GSN,(GSNON_A<<4));//Config GSN; Config ModGSN   
    SetReg(JREG_CWGSP,GSP_A);//Config GSP
    SetReg(JREG_CONTROL,BIT4);//Initiator = 1
    SetReg(JREG_RFCFG,RXGAIN_A<<4);//Config RxGain
    SetReg(JREG_RXTRESHOLD,(MINLEVEL_A<<4) | COLLLEVEL_A);//Config MinLevel; Config CollLevel   
    return;
}
//*************************************
//����  ����FM175XX_Initial_ReaderB
//��ڲ�����
//���ڲ�����
//*************************************

void FM175XX_Initial_ReaderB(void)
{
    ModifyReg(JREG_STATUS2,BIT3,RESET);

    SetReg(JREG_MODWIDTH,MODWIDTH_106);//MODWIDTH = 106kbps 
    SetReg(JREG_TXAUTO,0);//Force 100ASK = 0        
    SetReg(JREG_GSN,(GSNON_B<<4)|MODGSNON_B);//Config GSN; Config ModGSN   
    SetReg(JREG_CWGSP,GSP_B);//Config GSP
    SetReg(JREG_MODGSP,MODGSP_B);//Config ModGSP
    SetReg(JREG_CONTROL,BIT4);//Initiator = 1
    SetReg(JREG_RFCFG,RXGAIN_B<<4);//Config RxGain
    SetReg(JREG_RXTRESHOLD,MINLEVEL_B<<4);//Config MinLevel;
    return;
}

//*************************************
//����  ����ReaderA_Halt
//��ڲ�����
//���ڲ�����FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
uint8_t ReaderA_Halt(void)
{   
    uint8_t reg_data;
    SetReg(JREG_TXMODE,0x80);//Enable TxCRC
    SetReg(JREG_RXMODE,0x80);//Enable RxCRC
    SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
    SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
    SetReg(JREG_FIFODATA,0x50);
    SetReg(JREG_FIFODATA,0x00);
    SetReg(JREG_COMMAND,CMD_TRANSMIT);//command = Transceive
    SetReg(JREG_BITFRAMING,0x80);//Start Send
    fms175xx_delay_ms(2);//Wait 2ms
    GetReg(JREG_FIFOLEVEL,&reg_data);

    if(reg_data == 0) return FM175XX_SUCCESS;
    return FM175XX_COMM_ERROR;
}
//*************************************
//����  ����ReaderA_Wakeup
//��ڲ�����
//���ڲ�����FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
uint8_t ReaderA_Wakeup(void)
{
    uint8_t reg_data;
    SetReg(JREG_TXMODE,0);//Disable TxCRC
    SetReg(JREG_RXMODE,0);//Disable RxCRC
    SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
    SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
    SetReg(JREG_FIFODATA,RF_CMD_WUPA);
    SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
    SetReg(JREG_BITFRAMING,0x87);//Start Send
    fms175xx_delay_ms(1);//Wait 1ms
    GetReg(JREG_FIFOLEVEL,&reg_data);//
    if(reg_data == 2){
        GetReg(JREG_FIFODATA,PICC_A.ATQA);
        GetReg(JREG_FIFODATA,PICC_A.ATQA+1);
        return FM175XX_SUCCESS;
    }
    return FM175XX_COMM_ERROR;
}
//*************************************
//����  ����ReaderA_Request
//��ڲ�����
//���ڲ�����FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
uint8_t ReaderA_Request(void)
{
    uint8_t reg_data;
    SetReg(JREG_TXMODE,0);//Disable TxCRC
    SetReg(JREG_RXMODE,0);//Disable RxCRC
    SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
    SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
    SetReg(JREG_FIFODATA,RF_CMD_REQA);
    SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
    SetReg(JREG_BITFRAMING,0x87);//Start Send
    fms175xx_delay_ms(2);//Wait 2ms
    GetReg(JREG_FIFOLEVEL,&reg_data);
    if(reg_data == 2){
        GetReg(JREG_FIFODATA,PICC_A.ATQA);
        GetReg(JREG_FIFODATA,PICC_A.ATQA+1);
        return FM175XX_SUCCESS;
    }
    return FM175XX_COMM_ERROR;  
}

//*************************************
//����  ����ReaderA_AntiColl
//��ڲ�����
//���ڲ�����FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
uint8_t ReaderA_AntiColl(void)
{
    uint8_t reg_data;
    SetReg(JREG_TXMODE,0);//Disable TxCRC
    SetReg(JREG_RXMODE,0);//Disable RxCRC
    SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
    SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
    SetReg(JREG_FIFODATA,RF_CMD_ANTICOL);
    SetReg(JREG_FIFODATA,0x20);
    SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
    SetReg(JREG_BITFRAMING,0x80);//Start Send
    fms175xx_delay_ms(2);//Wait 2ms
    GetReg(JREG_FIFOLEVEL,&reg_data);
    if(reg_data == 5){
        GetReg(JREG_FIFODATA,PICC_A.UID);
        GetReg(JREG_FIFODATA,PICC_A.UID+1);
        GetReg(JREG_FIFODATA,PICC_A.UID+2);
        GetReg(JREG_FIFODATA,PICC_A.UID+3);
        GetReg(JREG_FIFODATA,&PICC_A.BCC);
        if( (PICC_A.UID[0] ^ PICC_A.UID[1] ^ PICC_A.UID[2] ^ PICC_A.UID[3]) == PICC_A.BCC)
            return FM175XX_SUCCESS;
    }
    return FM175XX_COMM_ERROR;  
}

//*************************************
//����  ����ReaderA_Select
//��ڲ�����
//���ڲ�����FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
uint8_t ReaderA_Select(void)
{
    uint8_t reg_data;
    SetReg(JREG_TXMODE,0x80);//Enable TxCRC
    SetReg(JREG_RXMODE,0x80);//Enable RxCRC
    SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
    SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
    SetReg(JREG_FIFODATA,0x93);
    SetReg(JREG_FIFODATA,0x70);
    SetReg(JREG_FIFODATA,PICC_A.UID[0]);
    SetReg(JREG_FIFODATA,PICC_A.UID[1]);
    SetReg(JREG_FIFODATA,PICC_A.UID[2]);
    SetReg(JREG_FIFODATA,PICC_A.UID[3]);
    SetReg(JREG_FIFODATA,PICC_A.BCC);       
    SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
    SetReg(JREG_BITFRAMING,0x80);//Start Send
    fms175xx_delay_ms(2);//Wait 2ms
    GetReg(JREG_FIFOLEVEL,&reg_data);
    if(reg_data == 1){
        GetReg(JREG_FIFODATA,&PICC_A.SAK);      
        return FM175XX_SUCCESS;
    }
    return FM175XX_COMM_ERROR;  
}

//*************************************
//����  ����ReaderA_CardActivate
//��ڲ�����
//���ڲ�����FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
uint8_t ReaderA_CardActivate(void)
{
    uint8_t  result;  
    result = ReaderA_Wakeup();
//    result = ReaderA_Request();
    if (result != FM175XX_SUCCESS) return FM175XX_COMM_ERROR;
    result = ReaderA_AntiColl();
    if (result != FM175XX_SUCCESS) return FM175XX_COMM_ERROR;         
    result = ReaderA_Select();
    if (result != FM175XX_SUCCESS) return FM175XX_COMM_ERROR;                 

    return result;
}
//*************************************
//����  ����ReaderB_Wakeup
//��ڲ�����
//���ڲ�����FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
uint8_t ReaderB_Wakeup(void)
{
    uint8_t reg_data,i;
    SetReg(JREG_TXMODE,0x83);//Enable TxCRC
    SetReg(JREG_RXMODE,0x83);//Enable RxCRC
    SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
    SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
    SetReg(JREG_FIFODATA,0x05);
    SetReg(JREG_FIFODATA,0x00);
    SetReg(JREG_FIFODATA,0x08);
    
    SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
    SetReg(JREG_BITFRAMING,0x80);//Start Send
    fms175xx_delay_ms(10);//Wait 10ms
    GetReg(JREG_FIFOLEVEL,&reg_data);
    if(reg_data == 12){
        for(i = 0;i < 12;i++)
        GetReg(JREG_FIFODATA,&PICC_B.ATQB[i]);
        memcpy(PICC_B.PUPI,PICC_B.ATQB + 1,4);
        memcpy(PICC_B.APPLICATION_DATA,PICC_B.ATQB + 6,4);
        memcpy(PICC_B.PROTOCOL_INF,PICC_B.ATQB + 10,3);
        return FM175XX_SUCCESS;
    }
    return FM175XX_COMM_ERROR;  
}
//*************************************
//����  ����ReaderB_Request
//��ڲ�����
//���ڲ�����FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
uint8_t ReaderB_Request(void)
{   
    uint8_t reg_data,i;
    SetReg(JREG_TXMODE,0x83);//Enable TxCRC
    SetReg(JREG_RXMODE,0x83);//Enable RxCRC
    SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
    SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
    SetReg(JREG_FIFODATA,0x05);
    SetReg(JREG_FIFODATA,0x00);
    SetReg(JREG_FIFODATA,0x00);
    
    SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
    SetReg(JREG_BITFRAMING,0x80);//Start Send
    fms175xx_delay_ms(10);//Wait 10ms
    GetReg(JREG_FIFOLEVEL,&reg_data);
    if(reg_data == 12){
        for(i = 0;i < 12;i++)
        GetReg(JREG_FIFODATA,&PICC_B.ATQB[i]);
        memcpy(PICC_B.PUPI,PICC_B.ATQB + 1,4);
        memcpy(PICC_B.APPLICATION_DATA,PICC_B.ATQB + 6,4);
        memcpy(PICC_B.PROTOCOL_INF,PICC_B.ATQB + 10,3);
        return FM175XX_SUCCESS;
    }
    return FM175XX_COMM_ERROR;  
}
//*************************************
//����  ����ReaderB_Attrib
//��ڲ�����
//���ڲ�����FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
uint8_t ReaderB_Attrib(void)
{
    uint8_t reg_data;
    SetReg(JREG_TXMODE,0x83);//Enable TxCRC
    SetReg(JREG_RXMODE,0x83);//Enable RxCRC
    SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
    SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
    SetReg(JREG_FIFODATA,0x1D);
    SetReg(JREG_FIFODATA,PICC_B.PUPI[0]);
    SetReg(JREG_FIFODATA,PICC_B.PUPI[1]);
    SetReg(JREG_FIFODATA,PICC_B.PUPI[2]);
    SetReg(JREG_FIFODATA,PICC_B.PUPI[3]);
    SetReg(JREG_FIFODATA,0x00);
    SetReg(JREG_FIFODATA,0x08);
    SetReg(JREG_FIFODATA,0x01);
    SetReg(JREG_FIFODATA,0x01);
    SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
    SetReg(JREG_BITFRAMING,0x80);//Start Send
    fms175xx_delay_ms(10);//Wait 10ms
    GetReg(JREG_FIFOLEVEL,&reg_data);
    if(reg_data == 1){       
        GetReg(JREG_FIFODATA,PICC_B.ATTRIB);
        return FM175XX_SUCCESS;
    }
    return FM175XX_COMM_ERROR;  
}
//*************************************
//����  ����ReaderB_GetUID
//��ڲ�����
//���ڲ�����FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
uint8_t ReaderB_GetUID(void)
{
    uint8_t reg_data,i;
    SetReg(JREG_TXMODE,0x83);//Enable TxCRC
    SetReg(JREG_RXMODE,0x83);//Enable RxCRC
    SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
    SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
    SetReg(JREG_FIFODATA,0x00);
    SetReg(JREG_FIFODATA,0x36);
    SetReg(JREG_FIFODATA,0x00);
    SetReg(JREG_FIFODATA,0x00);
    SetReg(JREG_FIFODATA,0x08);
    SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
    SetReg(JREG_BITFRAMING,0x80);//Start Send
    fms175xx_delay_ms(10);//Wait 10ms
    GetReg(JREG_FIFOLEVEL,&reg_data);
    if(reg_data == 10){       
        for(i=0;i<8;i++)
            GetReg(JREG_FIFODATA,&PICC_B.UID[i]);
        return FM175XX_SUCCESS;
    }
    return FM175XX_COMM_ERROR;  
}
//*************************************
//����  ����Reader_Polling
//��ڲ�����
//���ڲ�����FM175XX_SUCCESS, FM175XX_COMM_ERR
//*************************************
uint8_t Reader_Polling(uint8_t *polling_card)
{
    uint8_t reg_data;
    uint8_t result;
    *polling_card = 0;
    FM175XX_SoftReset();
    SetCW(TX1_TX2_CW_ENABLE);
    
    FM175XX_Initial_ReaderA();
    result = ReaderA_Wakeup();
//    result = ReaderA_Request();
    if (result == FM175XX_SUCCESS){         
        *polling_card |= BIT0;
    }else{
        // Fault
    }
    fms175xx_delay_ms(100);
    FM175XX_Initial_ReaderB();
    result = ReaderB_Wakeup();
//    result = ReaderB_Request();
    if (result == FM175XX_SUCCESS){           
        *polling_card |= BIT1;
    }else{
        // Fault
    }
    SetCW(TX1_TX2_CW_DISABLE);

    if (*polling_card != 0) return FM175XX_SUCCESS; 
    
    return FM175XX_COMM_ERROR;
}

uint8_t Reader_TPDU(transmission_struct *tpdu)
{
    uint8_t result;
    SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
    SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
    result = WriteFIFOData(tpdu->pSendBuffer,tpdu->SendLength);
    if(result != FM175XX_SUCCESS) return result;
    SetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
    ModifyReg(JREG_BITFRAMING,0x80,SET);//Start Send
    fms175xx_delay_ms(tpdu->Timeout);//
    GetReg(JREG_FIFOLEVEL,&tpdu->ReceiveLength);
    
    if(tpdu->ReceiveLength > 0)
        result = ReadFIFOData(tpdu->pReceiveBuffer,tpdu->ReceiveLength);
    else
        result = FM175XX_TIMER_ERROR;//δ��timeoutʱ���ڽ��յ�����  
    
    return result;  
}
