#include "stm32f10x.h"
#include "SIM900A.h"
#include "string.h" 
//#include "System_Initialize.h"
unsigned char g_ucUART1ReceiveTimeCNT = 10;
unsigned char g_ucReciveUart1Data[50]={0x00};
unsigned char g_ucUART1ReceiveFlag = 0;
unsigned char g_ucUART1ReceiveLen = 0;
unsigned short int g_uiSIMRespondTimeCNT = 0;

unsigned char PortArray[23]="AT+CLPORT=\"TCP\",\"";
unsigned char PortLen = 0;
unsigned char IpArray[43]="AT+CIPSTART=\"TCP\",\"";
unsigned char IpLen = 0;
void SIM900A_Send_String(char *s);
 /*******************************************************************************
* Function Name : SIM900A_Delayms
* Description   : delay N ms
*******************************************************************************/
void SIM900A_Delayms(unsigned short int n)
{
  unsigned short int uiNum,uiCount;
	for(uiNum=0;uiNum< n;uiNum++)
	  for(uiCount=0;uiCount<6000;uiCount++){
		  __nop();
	    __nop();
		}
}

void SIM900A_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;        //�������ݽṹ�� 

  /* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//PC6 SIM900A��λ��
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
  /* USART1 GPIO config */
	/* Configure USART1 Tx (PA.9) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
									
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
         
  USART_DeInit(USART1);
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure); 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1, ENABLE);
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_GetFlagStatus(USART1, USART_FLAG_TC);//��ʼ��ʱ����ȡUSART_SR����ֹ���ֽڶ�ʧ
	 
 // NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);//���ж�ʸ���ŵ�Flash��0��ַ
 // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//�������ȼ����õ�ģʽ
  //ʹ�ܴ����жϣ����������ȼ�
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;        //ռ��ʽ
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;                //��Ӧʽ
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
  NVIC_Init(&NVIC_InitStructure);        //���ṹ�嶪�����ú�������д�뵽��Ӧ�Ĵ�����	
//  SIM900A_NRESRT_L();	//
//	SIM900A_Send_String("AT");
//	SIM900A_Delayms(10);
//	SIM900A_Send_String("AT");
//	SIM900A_Delayms(10);
//	SIM900A_Send_String("AT+CGDCONT=1,\"IP\",\"CMNET\"\r"); //���ͼ����ֽ���SIM900����Ӧ������
//	SIM900A_Delayms(10);	
}
/*******************************************************************************
* Function Name : SIM900A_Send_Byte
* Description   : USART1����һ���ַ�
*******************************************************************************/
void SIM900A_Send_Byte(unsigned char byte)   //
{
	  USART_SendData(USART1, byte);        
		  while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);
}
/*******************************************************************************
* Function Name : SIM900A_Send_Array
* Description   : USART1��������
*******************************************************************************/
void SIM900A_Send_Array(unsigned char Array[],unsigned char len)   //
{
  unsigned char i;
	for(i=0;i<len;i++){
	  USART_SendData(USART1, Array[i]);        
		  while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);				
	}
}
/*******************************************************************************
* Function Name : SIM900A_Send_String
* Description   : USART1�����ַ��� 
*******************************************************************************/
void SIM900A_Send_String(char *s)   
{
  while((*s)!='\0'){
		SIM900A_Send_Byte(*s);
	  s++;
	}
}
/*******************************************************************************
* Function Name : SIM900A_Reset
* Description   : Reset SIM Moldue 
*******************************************************************************/
void SIM900A_Reset(void)   //
{
  SIM900A_NRESRT_H();
	SIM900A_Delayms(10);
	SIM900A_NRESRT_L();	
}
/*******************************************************************************
* Function Name : SIM900A_Check_CMD
* Description   : �������Ƿ���"OK" 
*******************************************************************************/
unsigned char SIM900A_Check_CMD(void)
{
  unsigned char ucCnt;
	for(ucCnt=0;ucCnt<g_ucUART1ReceiveLen;ucCnt++){
		if(g_ucReciveUart1Data[ucCnt] =='O'){
			if(g_ucReciveUart1Data[ucCnt+1] =='K'){
				return 0x01;
			}			
		}
	}
  return 0x0;
}

/*******************************************************************************
* Function Name : SIM900A_Check_CMD
* Description   : �������Ƿ���">"
*******************************************************************************/
unsigned char SIM900A_Check_Send(void)
{
  unsigned char ucCnt;
	for(ucCnt=0;ucCnt<g_ucUART1ReceiveLen;ucCnt++){
		if(g_ucReciveUart1Data[ucCnt] ==0x3E){
			//if(g_ucReciveUart1Data[ucCnt+1] =='K'){
				return 0x01;
			//}			
		}
	}
  return 0x0;
}

unsigned char SIM900A_Check_OK(char *s)
{
  unsigned char ucCnt;
	for(ucCnt=0;ucCnt<g_ucUART1ReceiveLen;ucCnt++){
		if(g_ucReciveUart1Data[ucCnt] ==*s){
			while((*s)!='\0'){
			  s++;
				if(g_ucReciveUart1Data[ucCnt+1] != *s){
					return 0x00;
				}	
			}
			return 0x01;
//			if(g_ucReciveUart1Data[ucCnt+1] =='K'){
//				return 0x01;
//			}			
		}
	}
	return 0x00;
}  	
	
/*******************************************************************************
* Function Name : SetIpPort
* Description   : ���� IP �� PORT�� 
*******************************************************************************/
void SetIpPort(char *ip,char *port)
{
  unsigned char i=0;
	unsigned char ucIpLen,ucPortLen;
	char TempPortArray[5];
	ucPortLen = strlen(port);
  while((*port)!='\0'){
		TempPortArray[i++] = *port;
	  port++;
	}	
  for(i=0;i<ucPortLen;i++){
		PortArray[17+i] = TempPortArray[i];
	}
	PortArray[17+ucPortLen] = '\"';
	PortArray[17+ucPortLen+1] = '\r';
	PortLen = 17+ucPortLen+1+1;          //����˿����鳤��
	
  ucIpLen = strlen(ip);     //���� Ip����
	for(i=0;i<ucIpLen;i++){   // ����IP
	  IpArray[19+i] = *ip;
		ip++;
	}
	IpArray[19+ucIpLen] = '\"';
	IpArray[19+ucIpLen+1] = ',';
	IpArray[19+ucIpLen+2] = '\"';
	
	for(i=0;i<ucPortLen;i++){    //����˿ں�
		IpArray[19+ucIpLen+3+i] = TempPortArray[i];
	}
	
	IpArray[19+ucIpLen+3+ucPortLen] = '\"';
	IpArray[19+ucIpLen+3+ucPortLen+1] = '\r';	
	
	IpLen = 19+ucIpLen+3+ucPortLen+1+1;  //����IP���鳤��
}
/*******************************************************************************
* Function Name : SIM900A_Connect
* Description   : SIM900A�����������ӣ�IP�Ͷ˿ں����ȷֱ��������IpArray[],PortArray[],
                  ���ӳɹ�����0x01 ���� 0x00 
*******************************************************************************/
unsigned char SIM900A_Connect(void) 
{	
	SIM900A_Send_Array(PortArray,PortLen);
	g_ucUART1ReceiveFlag = 0;
	g_uiSIMRespondTimeCNT =0;
  while(g_ucUART1ReceiveFlag==0){		
	  if(g_uiSIMRespondTimeCNT>=SIM900A_RESPOND_MAX_TIME)	//��ʱ
			return 0;
  }
  if(SIM900A_Check_CMD()==0)		
		return 0;

	SIM900A_Send_Array(IpArray,IpLen);
	g_ucUART1ReceiveFlag = 0;
		g_ucUART1ReceiveFlag = 0;
	g_uiSIMRespondTimeCNT =0;
  while(g_ucUART1ReceiveFlag==0){		
	  if(g_uiSIMRespondTimeCNT>=SIM900A_RESPOND_MAX_TIME)	//��ʱ
			return 0;
  }
  if(SIM900A_Check_CMD()==0)
		return 0;

	return 0x01;//	SIM900A_Delayms(1000);
}
/*******************************************************************************
* Function Name : SIM900A_SendDate
* Description   : SIM900A��������������ݣ����ͳɹ�����0x01 ���� 0x00 һ�η������1k byte                  
*******************************************************************************/
unsigned char SIM900A_SendDate(char *x) 
{	
  SIM900A_Send_String("AT+CIPSEND\r");
	g_ucUART1ReceiveFlag = 0;
	g_uiSIMRespondTimeCNT = 0;
  while(g_ucUART1ReceiveFlag==0){		
	  if(g_uiSIMRespondTimeCNT>=SIM900A_RESPOND_MAX_TIME)	//��ʱ
			return 0;		
  }
//	if(g_ucUART1ReceiveFlag==1){
//		g_ucUART1ReceiveFlag=0;
//		USART4_Send_Byte(g_ucUART1ReceiveLen);
//		UART4_Send_Array(g_ucReciveUart1Data,g_ucUART1ReceiveLen);		
//	}
  if(SIM900A_Check_Send()!=0){
    SIM900A_Send_String(x);
    SIM900A_Send_Byte(0x1a);
		return 0x01;
	}
		return 0;		
}	


