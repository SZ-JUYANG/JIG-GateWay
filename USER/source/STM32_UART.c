
#include "stm32f10x.h"
#include "includes.h"
#include "delay.h"
//#define PLC_DelayMs(x)  OSTimeDlyHMSM(0,0,0,x,OS_OPT_TIME_HMSM_STRICT,&err);

void STM32_UART_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
   /*UART1 TXD*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
    /*UART1 RXD*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
 //************************************************************* 
//    /*UART2 TXD*/
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_Init(GPIOA, &GPIO_InitStructure); 
//    /*UART2 RXD*/
//   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//   GPIO_Init(GPIOA, &GPIO_InitStructure); 

//************************************************************* 
    /*UART3 TXD*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_Init(GPIOB, &GPIO_InitStructure);   
    /*UART3 RXD*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
 //************************************************************* 
    /*UART4  TXD*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_Init(GPIOC, &GPIO_InitStructure);  
    /*UART4 RX */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOC, &GPIO_InitStructure); 

//*************************************************************
     /*UART5  TXD*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_Init(GPIOC, &GPIO_InitStructure);   

    /*UART5 RX */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOD, &GPIO_InitStructure); 

//*************************************************************
   /*UART1*/
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
	USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_GetFlagStatus(USART1, USART_FLAG_TC);//初始化时，读取USART_SR，防止首字节丢失
   /*UART2*/
//  USART_DeInit(USART2);
//  USART_InitStructure.USART_BaudRate = 38400;
//  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//  USART_InitStructure.USART_StopBits = USART_StopBits_1;
//  USART_InitStructure.USART_Parity = USART_Parity_No ;
//  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//  USART_Init(USART2, &USART_InitStructure); 
//  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//  USART_Cmd(USART2, ENABLE);
//	USART_ClearFlag(USART2, USART_FLAG_TC);
//	USART_GetFlagStatus(USART2, USART_FLAG_TC);//初始化时，读取USART_SR，防止首字节丢失
   /*UART3*/
  USART_DeInit(USART3);
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART3, &USART_InitStructure); 
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART3, ENABLE);
	USART_ClearFlag(USART3, USART_FLAG_TC);
	USART_GetFlagStatus(USART3, USART_FLAG_TC);//初始化时，读取USART_SR，防止首字节丢失
	/*UART4*/
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
	USART_GetFlagStatus(UART4, USART_FLAG_TC);//初始化时，读取USART_SR，防止首字节丢失
   /*UART5*/
  USART_DeInit(UART5);
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART5, &USART_InitStructure); 
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
  USART_Cmd(UART5, ENABLE);
	USART_ClearFlag(UART5, USART_FLAG_TC);
	USART_GetFlagStatus(UART5, USART_FLAG_TC);//初始化时，读取USART_SR，防止首字节丢失	
	
   /*UART1*/
   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;        //占有式
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure); 
  /*UART2*/
//   NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;        //占有式
//   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//   NVIC_Init(&NVIC_InitStructure); 
   /*UART3*/
   NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;        //占有式
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);  
  /*UART4*/
   NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;        //占有式
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);   
  /*UART5*/
   NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;        //占有式
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);    
//*************************************************************
}


/*******************************************************************************
* Function Name : 
* Description   : 
*******************************************************************************/
void GC9200_SendByte(unsigned char byte)   //
{	
  USART_SendData(USART1, byte);        //
   	while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);
}
/*******************************************************************************
* Function Name : 
* Description   : 
*******************************************************************************/
void GC9200_SendString(unsigned char *pString,unsigned char ucLen)
{
  volatile unsigned char i;
	GPIO_SetBits(GPIOC, GPIO_Pin_6);   // Enable send
	USART_GetFlagStatus(USART1, USART_FLAG_TC);
	for(i=0;i< 20;i++){
	   ;
	}
	for(i=0;i<ucLen;i++){
	    GC9200_SendByte(pString[i]);
	}
	for(i=0;i< 20;i++){
	   ;
	}
	GPIO_ResetBits(GPIOC, GPIO_Pin_6); // Enable receive
}
void GC9200_SendArray(const unsigned char *pString,unsigned char ucLen)
{
  volatile unsigned char i;
	GPIO_SetBits(GPIOC, GPIO_Pin_6);   // Enable send
	USART_GetFlagStatus(USART1, USART_FLAG_TC);
	for(i=0;i< 20;i++){
	   ;
	}
	for(i=0;i<ucLen;i++){
	    GC9200_SendByte(pString[i]);
	}
	for(i=0;i< 20;i++){
	   ;
	}
	GPIO_ResetBits(GPIOC, GPIO_Pin_6); // Enable receive
}

/*******************************************************************************
* Function Name : 
* Description   : 
*******************************************************************************/
void IT8512_SendByte(unsigned char byte)   //
{	
  
  USART_SendData(UART4, byte);        //
   	while( USART_GetFlagStatus(UART4,USART_FLAG_TC)!= SET);
}
/*******************************************************************************
* Function Name : 
* Description   : 
*******************************************************************************/
void IT8512_SendString(unsigned char *pString,unsigned char ucLen)
{
    unsigned char i;
	  USART_GetFlagStatus(UART4, USART_FLAG_TC);
	  for(i=0;i<ucLen;i++){
	      IT8512_SendByte(pString[i]);
	  }
}


void IT8512_SendArray(const unsigned char *pString,unsigned char ucLen)
{
    unsigned char i;
	  USART_GetFlagStatus(UART4, USART_FLAG_TC);
	  for(i=0;i<ucLen;i++){
	      IT8512_SendByte(pString[i]);
	  }
}



/*******************************************************************************
* Function Name : 
* Description   : 
*******************************************************************************/
void PM9805_SendByte(unsigned char byte)   //
{	
  USART_SendData(USART3, byte);        //
   	while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);
}
/*******************************************************************************
* Function Name : 
* Description   : 
*******************************************************************************/
void PM9805_SendString(unsigned char *pString,unsigned char ucLen)
{
    unsigned char i;
	  USART_GetFlagStatus(USART3, USART_FLAG_TC);
	  for(i=0;i<ucLen;i++){
	      PM9805_SendByte(pString[i]);
	  }
}
/*******************************************************************************
* Function Name : 
* Description   : 
*******************************************************************************/
void GP9034T_SendByte(unsigned char byte)   //
{	
  USART_SendData(UART5, byte);        //
   	while( USART_GetFlagStatus(UART5,USART_FLAG_TC)!= SET);
}
/*******************************************************************************
* Function Name : 
* Description   : 
*******************************************************************************/
//void GP9034T_SendString(unsigned char *pString,unsigned char ucLen)
//{
//  unsigned char i;
//	USART_GetFlagStatus(UART5, USART_FLAG_TC);
//	for(i=0;i<ucLen;i++){
//	  GP9034T_SendByte(pString[i]);
//	}
//}
void GP9034T_SendArray(unsigned char *pString,unsigned char ucLen)
{
    unsigned char i;
	  USART_GetFlagStatus(UART5, USART_FLAG_TC);
	  for(i=0;i<ucLen;i++){
	      GP9034T_SendByte(pString[i]);
	  }
}

void C8000_SendString(unsigned char *pString,unsigned char ucLen)
{
    unsigned char i;
	  USART_GetFlagStatus(UART5, USART_FLAG_TC);
	  for(i=0;i<ucLen;i++){
	      GP9034T_SendByte(pString[i]);
	  }
}


