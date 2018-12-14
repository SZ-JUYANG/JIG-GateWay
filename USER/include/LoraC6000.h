#ifndef __LoraC6000_H
#define __LoraC6000_H
void Uart4_SendByte(unsigned char byte) ;
unsigned short int ReadLoraC6000ID(void);
unsigned int RquirePLC_ID(unsigned short int uiLoraID);
unsigned char ConfirmPLC_ID(unsigned short int uiLoraID,unsigned char ucResult);
#endif
