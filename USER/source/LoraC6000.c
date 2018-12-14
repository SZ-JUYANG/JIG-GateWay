#include "stm32f10x.h"
#include "includes.h"
#include <string.h>
#include "main.h"

//#define C6000_RCV_NUM 20

//unsigned char g_ucC6000_RecBuf[C6000_RCV_NUM ];//���ڽ��ջ���
//unsigned char g_ucC6000_RecLen = 0;
//unsigned char  g_ucC6000_RcvFlag = 0;     //���ڽ��������ݱ�־

extern unsigned char g_ucC6000_RecBuf[C6000_RCV_NUM ];//���ڽ��ջ���
extern unsigned char g_ucC6000_RecLen ;
extern unsigned char g_ucC6000_RcvFlag ;     //���ڽ��������ݱ�־

/*******************************************************************************
  * @brief  Uart4Init
  * @param  Uart4 ��ʼ�� 115200
  * @retval 
*******************************************************************************/
void Uart4Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	  /*UART4  TXD*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);  
    /*UART4 RX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
    USART_DeInit(UART4);
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART4, &USART_InitStructure); 
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
    USART_Cmd(UART4, ENABLE);
	USART_ClearFlag(UART4, USART_FLAG_TC);
	USART_GetFlagStatus(UART4, USART_FLAG_TC);//��ʼ��ʱ����ȡUSART_SR����ֹ���ֽڶ�ʧ	
}

/*******************************************************************************
  * @brief  Uart4_SendByte
  * @param  Uart4 send a byte
  * @retval 
*******************************************************************************/
void Uart4_SendByte(unsigned char byte)   //
{	 
    USART_SendData(UART4, byte);        //
   	 while( USART_GetFlagStatus(UART4,USART_FLAG_TC)!= SET);
}
/*******************************************************************************
  * @brief  C6000_SendString
  * @param  �����ַ���
  * @retval 
*******************************************************************************/
void C6000_SendString(unsigned char *pString,unsigned char ucLen)
{
    unsigned char i;
	  USART_GetFlagStatus(UART4, USART_FLAG_TC);
	  for(i=0;i<ucLen;i++){
	      Uart4_SendByte(pString[i]);
	  }
}

/*******************************************************************************
  * @brief  CheckA5Data
  * @param  ���ģ�鷵�����ݵĸ�ʽ
  * @retval 00:ʧ�ܣ�����ֵ�ɹ�
*******************************************************************************/
unsigned char CheckA5Data(unsigned char *pucInputData,unsigned char ucDataLen)
{
    unsigned char ucLen = 0;
	  unsigned char ucCs = 0;
	  unsigned char i;
	  ucLen = pucInputData[1];     // Lenght
	  if(pucInputData[0] != 0xA5)  // Header SOF
		  return 0;
		if((ucLen + 4) != ucDataLen){ // Len
		  return 0;
		}
		if(pucInputData[ucDataLen-1] != 0x5A)  // EOF
		  return 0;
	  for(i=0;i<ucLen;i++){
		  ucCs += pucInputData[2+i];
		}
		if(ucCs != pucInputData[ucDataLen-2]) //cs
			return 0;
		return 1;
}
/*******************************************************************************
  * @brief  CheckA5Data
  * @param  ������������Ϣ
  * @retval 00:ʧ�ܣ�����ֵ�ɹ�
*******************************************************************************/
unsigned char CheckReceiveNetWorkData(unsigned char *pucInputData,unsigned char ucDataLen)
{
    unsigned char ucLen = 0;
	  unsigned char ucCs = 0;
	  unsigned char i;
	  ucLen = pucInputData[2];     // Lenght
	  if(pucInputData[0] != 0x68)  // Header SOF
		  return 0;
		if((ucLen + 5) != ucDataLen){ // Len
		  return 0;
		}
		if(pucInputData[ucDataLen-1] != 0x16)  // EOF
		  return 0;
	  for(i=0;i<(ucDataLen-2);i++){
		  ucCs += pucInputData[i];
		}
		if(ucCs != pucInputData[ucDataLen-2]) //cs
			return 0;
		return 1;
}


//send   A5 01 04 04 5A
//Rceive a5 03 84 01 00 85 5a  //ID 0x0001
/*******************************************************************************
  * @brief  ReadLoraC6000ID
  * @param  ��Loraģ���ID
  * @retval 00:��Ч
*******************************************************************************/ 
unsigned short int ReadLoraC6000ID(void)
{
    unsigned char ucReadID_Cmd[5] = {0xA5,0x01,0x04,0x04,0x5A};
	  unsigned char i,j;
		unsigned short int uiTempVal = 0;
    OS_ERR err;	
    for(j = 0;j < 3;j++){
			  memset(g_ucC6000_RecBuf,0,C6000_RCV_NUM); // g_ucC8000_RecBuf[C8000_RCV_NUM ];
		    g_ucC6000_RcvFlag = 0;
        C6000_SendString(ucReadID_Cmd,5);//
			  for(i = 0;i < 70;i++){
				    if(g_ucC6000_RcvFlag > 0){ //
                if(CheckA5Data(g_ucC6000_RecBuf,g_ucC6000_RecLen)>0){// ��������֡��ȷ
                  if(g_ucC6000_RecBuf[2] == 0x84){
									    uiTempVal = g_ucC6000_RecBuf[4];
										  uiTempVal = (uiTempVal<<8) + g_ucC6000_RecBuf[3];
										  g_ucC6000_RcvFlag = 0;
										  memset(g_ucC6000_RecBuf,0,g_ucC6000_RecLen);
										  return uiTempVal;
									} 
										
								}
								g_ucC6000_RcvFlag = 0;
								memset(g_ucC6000_RecBuf,0,g_ucC6000_RecLen);								
						}											
				    OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);	
				}			
		}
    return 0;	
}


/*******************************************************************************
  * @brief  RquirePLC_ID
  * @param  �������������ID
  * @retval 00:ʧ��
*******************************************************************************/ 
/**************************************************
[14:21:23.465]send A5 09 10 68 01 03 10 00 01 7D 16 20 5A (MCU->ģ��)
[14:21:23.582]rcv  A5 09 90 68 01 03 10 00 01 7D 16 A0 5A (ģ������OK,ignore)
[14:21:24.105]rcv  A5 02 00 01 01 5A                      (���ػ�ӦOK,ignore)
[14:21:25.080]rcv  68 81 05 10 01 00 00 02 01 16          (���ط���ID 0x01000002)
***************************************************/
unsigned int RquirePLC_ID(unsigned short int uiLoraID)
{
   //Byte7-8 ->LoraID                                                          cs1       cs2
   unsigned char ucRquireCmd[13]={0xA5,0x09,0x10,0x68,0x01,0x03,0x10,0x00,0x01,0x7D,0x16,0x20,0x5A};
   unsigned char i,j,cs = 0;
	 OS_ERR err;
	 unsigned int uiTempVal;
	 
	 ucRquireCmd[7] = (uiLoraID>>8)&0xff;
	 ucRquireCmd[8] = uiLoraID&0xff;
	 for(i=0;i<6;i++){
	   cs += ucRquireCmd[i+3];
	 }
	 ucRquireCmd[9] = cs;
   cs = 0;
	 for(i=0;i<9;i++){
	   cs += ucRquireCmd[i+2];
	 }
   ucRquireCmd[11] = cs;	
    	
    for(j = 0;j < 3;j++){
			  memset(g_ucC6000_RecBuf,0,C6000_RCV_NUM); 
		    g_ucC6000_RcvFlag = 0;
        C6000_SendString(ucRquireCmd,13);//
			  for(i = 0;i < 100;i++){
				    if(g_ucC6000_RcvFlag > 0){ //
                if(CheckReceiveNetWorkData(g_ucC6000_RecBuf,g_ucC6000_RecLen)>0){// ��������֡��ȷ
                  if((g_ucC6000_RecBuf[1] == 0x81)&&(g_ucC6000_RecBuf[3] == 0x10)){
									    uiTempVal = g_ucC6000_RecBuf[4];
										  uiTempVal = (uiTempVal<<8) + g_ucC6000_RecBuf[5];
										  uiTempVal = (uiTempVal<<8) + g_ucC6000_RecBuf[6];
										  uiTempVal = (uiTempVal<<8) + g_ucC6000_RecBuf[7];
										  g_ucC6000_RcvFlag = 0;
										  memset(g_ucC6000_RecBuf,0,g_ucC6000_RecLen);
										  return uiTempVal;
									} 
										
								}
								g_ucC6000_RcvFlag = 0;
								memset(g_ucC6000_RecBuf,0,g_ucC6000_RecLen);								
						}											
				    OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err);	
				}			
		}
    return 0;		 
}


/*******************************************************************************
  * @brief  ConfirmPLC_ID
  * @param  ������ȷ�Ϸ���ID
  * @retval 00:ʧ�� 01:�ɹ�
*******************************************************************************/ 
/*******************
* send A5 0A 10 68 A1 04 10 00 01 01 1F 16 64 5A (MCU->ģ��)
* rcv  A5 0A 90 68 A1 04 10 00 01 01 1F 16 E4 5A (ģ������OK,)
* rcv  A5 02 00 01 01 5A                         (���ػ�ӦOK,)
******************/
unsigned char ConfirmPLC_ID(unsigned short int uiLoraID,unsigned char ucResult)
{
    unsigned char ucConfirmCmd[14]={0xA5,0x0A,0x10,0x68,0xA1,0x04,0x10,0x00,0x01,0x01,0x1F,0x16,0x64,0x5A};
		//unsigned char ucRespondCmd[6] = {0xA5,0x02,0x00,0x01,0x01,0x5A};
			
    unsigned char i,j,cs = 0;
	  OS_ERR err;		
		
	  ucConfirmCmd[7] = (uiLoraID>>8)&0xff;
	  ucConfirmCmd[8] = uiLoraID&0xff;
		ucConfirmCmd[9] = ucResult&0x01;
		
	  for(i=0;i<7;i++){
	      cs += ucConfirmCmd[i+3];
	  }
	  ucConfirmCmd[10] = cs;
    cs = 0;
	  for(i=0;i<10;i++){
	      cs += ucConfirmCmd[i+2];
	  }
    ucConfirmCmd[12] = cs;	

    for(j = 0;j < 3;j++){
			  memset(g_ucC6000_RecBuf,0,C6000_RCV_NUM); 
		    g_ucC6000_RcvFlag = 0;
        C6000_SendString(ucConfirmCmd,14);//
			  for(i = 0;i < 100;i++){
				    if(g_ucC6000_RcvFlag > 0){ //
                if(CheckA5Data(g_ucC6000_RecBuf,g_ucC6000_RecLen)>0){// ��������֡��ȷ
                  if((g_ucC6000_RecBuf[2] == 0x00)&&(g_ucC6000_RecBuf[3] == 0x01)){
										  g_ucC6000_RcvFlag = 0;
										  memset(g_ucC6000_RecBuf,0,g_ucC6000_RecLen);
										  return 1;
									} 
										
								}
								g_ucC6000_RcvFlag = 0;
								memset(g_ucC6000_RecBuf,0,g_ucC6000_RecLen);								
						}											
				    OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err);	
				}			
		}
    return 0; 
}


/**************************
*����
*1��ReadLoraC6000ID(void)//��Loraģ��ID
*2��RquirePLC_ID(unsigned short int uiLoraID) //����������ID
*3��ConfirmPLC_ID(unsigned short int uiLoraID,unsigned char ucResult)//������ȷ��ID
*************************/

