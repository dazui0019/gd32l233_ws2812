#ifndef CPU_CARD_H
#define CPU_CARD_H 1

#include "reader_api.h"
#define FRAME_SIZE 64
struct ATS_STR
{
unsigned char Length;
unsigned char Ats_Data[255];
};
struct PPS_STR
{
unsigned char Length;
unsigned char Pps_Data[1];
};

struct CPU_CARD_STR
{
unsigned char PCB;
unsigned char CID;
unsigned char WTXM;
struct ATS_STR ATS;
struct PPS_STR PPS;
};

extern struct CPU_CARD_STR CPU_CARD;

extern void CPU_PCB(void);
extern unsigned char CPU_BaudRate_Set(unsigned char baudrate);
extern unsigned char CPU_TPDU(unsigned char send_len,unsigned char *send_buff,unsigned char *rece_len,unsigned char *rece_buff);
extern unsigned char CPU_Rats(unsigned char *ats_len,unsigned char *ats);
extern unsigned char CPU_PPS(unsigned char baudrate,unsigned char *pps_len,unsigned char *pps);
extern unsigned char CPU_Random(unsigned char *rece_len,unsigned char *buff);
extern unsigned char CPU_I_Block(unsigned char send_len,unsigned char *send_buff,unsigned char *rece_len,unsigned char *rece_buff);
extern unsigned char CPU_R_Block(unsigned char *rece_len,unsigned char *rece_buff);
extern unsigned char CPU_ACK(unsigned char pcb_byte,unsigned char *rece_len,unsigned char *rece_buff);
extern unsigned char CPU_APDU(transmission_struct *apdu);

#endif
