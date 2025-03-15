#ifndef _READERAPI_H_
#define _READERAPI_H_

#include <stddef.h>
#include <stdint.h>

static const uint8_t RF_CMD_REQA = 0x26;
static const uint8_t RF_CMD_WUPA	= 0x52;
static const uint8_t RF_CMD_ANTICOL = 0x93;
static const uint8_t RF_CMD_SELECT = 0x93;

//�����������
#define MODWIDTH_106 0x26 //106KbpsΪ0x26
#define MODWIDTH_212 0x13 //212kbpsΪ0x13
#define MODWIDTH_424 0x09 //424kbpsΪ0x09
#define MODWIDTH_848 0x04 //848kbpsΪ0x04
//���ղ�������
//TYPE A
#define RXGAIN_A		4		//���÷�Χ0~7
#define GSNON_A			15			//���÷�Χ0~15
#define GSP_A 			31			//���÷�Χ0~63
#define COLLLEVEL_A 4	//���÷�Χ0~7
#define MINLEVEL_A  5	//���÷�Χ0~15
//TYPE B
#define RXGAIN_B		6		//���÷�Χ0~7
#define GSNON_B			15			//���÷�Χ0~15
#define MODGSNON_B 	6 	//���÷�Χ0~15
#define GSP_B 			31			//���÷�Χ0~63
#define MODGSP_B 		10		//���÷�Χ0~63
#define MINLEVEL_B  4	//���÷�Χ0~15

#define RXWAIT 		4		//���÷�Χ0~63
#define UARTSEL 	2		//Ĭ������Ϊ2  ���÷�Χ0~3 0:�̶��͵�ƽ 1:TIN�����ź� 2:�ڲ������ź� 3:TIN�����ź�

typedef struct 
{
	uint8_t SendLength;
	uint8_t *pSendBuffer;
	uint8_t ReceiveLength;
	uint8_t *pReceiveBuffer;
	unsigned int Timeout;
}transmission_struct;

struct picc_b_struct
{
uint8_t ATQB[12];
uint8_t	PUPI[4];
uint8_t	APPLICATION_DATA[4];
uint8_t	PROTOCOL_INF[3];
uint8_t ATTRIB[10];
uint8_t UID[8];
};

extern struct picc_b_struct PICC_B; 

struct picc_a_struct
{
uint8_t ATQA[2];
uint8_t UID[4];
uint8_t BCC;
uint8_t SAK;
};

extern struct picc_a_struct PICC_A; 


#define FM175XX_PARAM_ERROR 		0xF1	//�����������
#define FM175XX_TIMER_ERROR			0xF2	//���ճ�ʱ
#define FM175XX_COMM_ERROR			0xF3	//ͨ�Ŵ���
#define FM175XX_COLL_ERROR			0xF4	//��ͻ����
#define FM175XX_CRC_ERROR				0xF5
#define FM175XX_PARITY_ERROR		0xF6
#define FM175XX_PROTOCOL_ERROR	0xF7

#define FM175XX_AUTH_ERROR	0xE1

#define FM175XX_RATS_ERROR 	0xD1
#define FM175XX_PPS_ERROR 	0xD2
#define FM175XX_PCB_ERROR 	0xD3

typedef struct
{
	uint8_t Cmd;                 	// �������
	uint8_t SendCRCEnable;
	uint8_t ReceiveCRCEnable;
	uint8_t nBitsToSend;					//׼�����͵�λ��	
	uint8_t nBytesToSend;        	//׼�����͵��ֽ���
	uint8_t nBitsToReceive;					//׼�����յ�λ��	
	uint8_t nBytesToReceive;			//׼�����յ��ֽ���	
	uint8_t nBytesReceived;      	// �ѽ��յ��ֽ���
	uint8_t nBitsReceived;       	// �ѽ��յ�λ��
	uint8_t *pSendBuf;						//�������ݻ�����
	uint8_t *pReceiveBuf;					//�������ݻ�����
	uint8_t CollPos;             	// ��ײλ��
	uint8_t Error;								// ����״̬
	uint8_t Timeout;							//��ʱʱ��
} command_struct;


extern void FM175XX_Initial_ReaderA(void);
extern void FM175XX_Initial_ReaderB(void);
extern uint8_t ReaderA_Halt(void);
extern uint8_t ReaderA_Request(void);
extern uint8_t ReaderA_Wakeup(void);

extern uint8_t ReaderA_AntiColl(void);
extern uint8_t ReaderA_Select(void);
extern uint8_t ReaderA_CardActivate(void);

extern uint8_t ReaderB_Request(void);
extern uint8_t ReaderB_Wakeup(void);
extern uint8_t ReaderB_Attrib(void);
extern uint8_t ReaderB_GetUID(void);
extern uint8_t Reader_Polling(uint8_t *polling_card);
extern uint8_t Reader_TPDU(transmission_struct *tpdu);
#endif

