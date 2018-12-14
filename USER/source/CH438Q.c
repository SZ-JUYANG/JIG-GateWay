#include "CH438Q.h"

#include "stm32f10x.h"


void CH438Q_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5\
                                |GPIO_Pin_6|GPIO_Pin_8|GPIO_Pin_9	;  // DATA\ WD \RD    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;        // CH438 INT  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOE, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5\
                                |GPIO_Pin_6|GPIO_Pin_7;  //ADDRESS    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIOE->BRR = 0x7f;  //ADD  set to 0
	GPIOC->BRR = 0xff;  //Data set to 0
	CH438_WR_H();       //
  CH438_RD_H();
}

void CH438Q_Data_DIR(unsigned char ucDir)
{
  GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5\
                                |GPIO_Pin_6|GPIO_Pin_7;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if(ucDir){
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	}else{
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	}
  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void WriteCH438Data( unsigned char addr, unsigned char dat)
{
// GPIOE->BSRR = 0x80;// set 1   
// GPIOE->BRR = 0x80; // set 0
  CH438Q_Data_DIR(CH438_OUT); //set to output
	//GPIOE->BSRR = addr;
  GPIOE->BRR = 0x7f;  //ADD  set to 0
	GPIOC->BRR = 0xff;  //Data set to 0
	
	GPIOE->BSRR = 0x7f&addr;
	GPIOC->BSRR = dat;
	__nop();
	__nop();
	__nop();
	__nop();	
	CH438_WR_L();
	__nop();
	__nop();
	__nop();
	__nop();	
	__nop();
	__nop();	
	CH438_WR_H();
}

unsigned char ReadCH438Data( unsigned char addr )
{
  unsigned char dat;
//   CH438_ADDR_PORT = addr;
//	 CH438_DATA_PORT = 0xff;   	
//	 CH438_CS_WIRE = 0;
//	 CH438_RD_WIRE = 0;
//	 dat = CH438_DATA_PORT;
//	 CH438_RD_WIRE = 1;
//	 CH438_CS_WIRE = 1;
//	 CH438_ADDR_PORT = 0xff;     
//	 CH438_DATA_PORT = 0xff;	 
	CH438Q_Data_DIR(CH438_IN); // config input
	GPIOE->BRR = 0x7f;  //ADD  set to 0
	
	GPIOE->BSRR = 0x7f&addr; //set ADD
	__nop();
	__nop();
	__nop();
	__nop();	
	CH438_RD_L();
	__nop();
	__nop();
	__nop();
	__nop();
	dat = GPIO_ReadInputData(GPIOC)&0xff;
	CH438_RD_H();
	return (dat);

}

void WriteCH438Block( unsigned char mAddr, unsigned char mLen, unsigned char *mBuf )   
{
  while ( mLen -- ) 	
	  WriteCH438Data( mAddr, *mBuf++ );
}
