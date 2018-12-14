#ifndef __SIM900A_H
#define __SIM900A_H


#define SIM900A_RCV_MAX_TIME 10
#define SIM900A_RESPOND_MAX_TIME 3000 //3s

#define SIM900A_NRESRT_H() GPIO_SetBits(GPIOC,GPIO_Pin_6)
#define SIM900A_NRESRT_L() GPIO_ResetBits(GPIOC,GPIO_Pin_6)

void SIM900A_Init(void);
void SIM900A_Send_Byte(unsigned char byte);
void SIM900A_Delayms(unsigned short int n);
void SIM900A_Reset(void); 
unsigned char SIM900A_Connect(void);
unsigned char SIM900A_SendDate(char *x);
void SetIpPort(char *ip,char *port);
#endif
