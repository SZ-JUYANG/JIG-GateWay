/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "System_Initialize.h"
#include "main.h"
#include "STM32_UART.h"
#include "LoraC8000.h"
//#include "LCD12864G.h"
//#include "MENU.h"
//#include "SIM900A.h"
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern unsigned char  g_ucPM9805_RcvTime ; //
extern unsigned char  g_ucIT8512_RcvTime ;
extern unsigned char  g_ucGP9034_RcvTime ;
extern unsigned char  g_ucGC9200_RcvTime ;
extern unsigned char  g_ucC8000_RcvTime ;
extern unsigned char  g_ucC6000_RcvTime ;

extern unsigned char  g_ucPM9805_RcvFlag ; //
extern unsigned char  g_ucIT8512_RcvFlag ;
extern unsigned char  g_ucGP9034_RcvFlag ;
extern unsigned char  g_ucGC9200_RcvFlag ;
extern unsigned char  g_ucC8000_RcvFlag;
extern unsigned char  g_ucC6000_RcvFlag;

extern unsigned char g_ucPM9805_RecBuf[PM9805_RCV_NUM];
extern unsigned char g_ucIT8512_RecBuf[IT8512_RCV_NUM];
extern unsigned char g_ucGP9034_RecBuf[GP9034_RCV_NUM];
extern unsigned char g_ucGC9200_RecBuf[GC9200_RCV_NUM];
extern unsigned char g_ucC8000_RecBuf[C8000_RCV_NUM ];
extern unsigned char g_ucC6000_RecBuf[C6000_RCV_NUM ];
extern unsigned char g_ucC8000_RecLen;
extern unsigned char g_ucC6000_RecLen;

extern unsigned short int g_uiSecondCnt;
extern unsigned short int g_uiSentIntervalCnt;

unsigned char g_ucBeepOnCnt = BEEP_ON_TIME;



/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
//  if(g_uiSecondCnt < 1000){
//	  g_uiSecondCnt++;
//	}	
//	
//}
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)  //GC9200
{
//  unsigned char ucTempReceiveData = 0;
//	if(USART_GetITStatus(USART1,USART_IT_RXNE)!= RESET){ 
//	  ucTempReceiveData = USART_ReceiveData(USART1);
//	}
// USART_ClearFlag(USART1, USART_FLAG_TC);
	unsigned char ucTempReceiveData = 0;
	static unsigned char ucIndex = 0;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!= RESET){ 
		ucTempReceiveData = USART_ReceiveData(USART1);
			if(g_ucGC9200_RcvTime >= RCV_MAX_TIME)
				ucIndex = 0;	
			if(g_ucGC9200_RcvFlag == 0){        //buffer available
				g_ucGC9200_RecBuf[ucIndex++] = ucTempReceiveData;
				if(ucIndex > GC9200_RCV_NUM){
					ucIndex = GC9200_RCV_NUM; 
				}
				g_ucGC9200_RcvTime = 0;
			}	
	}
	USART_ClearFlag(USART1, USART_FLAG_TC);	
}
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void) //IT8512
{
//  unsigned char ucTempReceiveData = 0;
//	if(USART_GetITStatus(USART2,USART_IT_RXNE)!= RESET){ 
//	  ucTempReceiveData = USART_ReceiveData(USART2);
//	}
// USART_ClearFlag(USART2, USART_FLAG_TC);
	unsigned char ucTempReceiveData = 0;
	static unsigned char ucIndex = 0;
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!= RESET){ 
		ucTempReceiveData = USART_ReceiveData(USART2);
//			if(g_ucIT8512_RcvTime >= RCV_MAX_TIME)
//				ucIndex = 0;	
//			if(g_ucIT8512_RcvFlag == 0){        //buffer available
//				g_ucIT8512_RecBuf[ucIndex++] = ucTempReceiveData;
//				if(ucIndex > IT8512_RCV_NUM){
//					ucIndex = IT8512_RCV_NUM; 
//				}
//				g_ucIT8512_RcvTime = 0;
//			}	
	}
	USART_ClearFlag(USART2, USART_FLAG_TC);	
}
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None g_ucPM9805_RecBuf[PM9805_RCV_NUM]
  */
void USART3_IRQHandler(void)//PM9805
{
	unsigned char ucTempReceiveData = 0;
	static unsigned char ucIndex = 0;
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!= RESET){ 
		ucTempReceiveData = USART_ReceiveData(USART3);
			if(g_ucPM9805_RcvTime >= RCV_MAX_TIME)
				ucIndex = 0;	
			if(g_ucPM9805_RcvFlag == 0){        //buffer available
				g_ucPM9805_RecBuf[ucIndex++] = ucTempReceiveData;
				if(ucIndex > PM9805_RCV_NUM){
					ucIndex = PM9805_RCV_NUM; 
				}
				g_ucPM9805_RcvTime = 0;
			}	
	}
	USART_ClearFlag(USART3, USART_FLAG_TC);
}
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void)
{
  unsigned char ucTempReceiveData = 0;
	static unsigned char ucIndex = 0;
	
	if(USART_GetITStatus(UART4,USART_IT_RXNE)!= RESET){ 
		ucTempReceiveData = USART_ReceiveData(UART4);
			if(g_ucC6000_RcvTime >= RCV_MAX_TIME)
				ucIndex = 0;	
			if(g_ucC6000_RcvFlag == 0){     // buffer available
				g_ucC6000_RecBuf[ucIndex++] = ucTempReceiveData;
				g_ucC6000_RecLen = ucIndex;
				if(ucIndex > C6000_RCV_NUM){
					ucIndex = C6000_RCV_NUM; 
				}
				g_ucC6000_RcvTime = 0;
			}	
	}
	USART_ClearFlag(UART4, USART_FLAG_TC);		
}
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void UART5_IRQHandler(void) //GP9034
{
  unsigned char ucTempReceiveData = 0;
	static unsigned char ucIndex = 0;
	if(USART_GetITStatus(UART5,USART_IT_RXNE)!= RESET){ 
	    ucTempReceiveData = USART_ReceiveData(UART5);
			if(g_ucC8000_RcvTime >= RCV_MAX_TIME)
				  ucIndex = 0;	
			if(g_ucC8000_RcvFlag == 0){     // buffer available
				  g_ucC8000_RecBuf[ucIndex++] = ucTempReceiveData;
				  g_ucC8000_RecLen = ucIndex;
				  if(ucIndex > C8000_RCV_NUM){
					    ucIndex = C8000_RCV_NUM; 
				  }
				  g_ucC8000_RcvTime = 0;
			}	
	}
  USART_ClearFlag(UART5, USART_FLAG_TC);
}
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
unsigned char  g_ucPM9805_RcvTime = 0; //
unsigned char  g_ucIT8512_RecTime = 0;
unsigned char  g_ucGP9034_RecTime = 0;
unsigned char  g_ucPM9805_RcvFlag = 0; //
unsigned char  g_ucIT8512_RecFlag = 0;
  */


void TIM2_IRQHandler(void)
{
static unsigned short int cnt = 0;
 	
	//static unsigned char Table[4]="tim1";
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){ 
//      if(g_ucPM9805_RcvTime < RCV_MAX_TIME){
//			  g_ucPM9805_RcvTime++;
//				if(g_ucPM9805_RcvTime >= RCV_MAX_TIME)
//					g_ucPM9805_RcvFlag = 1;
//			}			
//      if(g_ucIT8512_RcvTime < RCV_MAX_TIME){				
//			  g_ucIT8512_RcvTime++;
//				if(g_ucIT8512_RcvTime >= RCV_MAX_TIME)
//					g_ucIT8512_RcvFlag = 1;
//			}
      if(g_ucC8000_RcvTime < RCV_MAX_TIME){				
			  g_ucC8000_RcvTime++;
				if(g_ucC8000_RcvTime >= RCV_MAX_TIME)
					g_ucC8000_RcvFlag = 1;
			}
      if(g_ucC6000_RcvTime < RCV_MAX_TIME){				
			  g_ucC6000_RcvTime++;
				if(g_ucC6000_RcvTime >= RCV_MAX_TIME)
					g_ucC6000_RcvFlag = 1;
			}
			
//			if((cnt++)> 1000){
//			    cnt = 0;			
//			}
//      if(g_ucGP9034_RcvTime < RCV_MAX_TIME){				
//			  g_ucGP9034_RcvTime++;
//				if(g_ucGP9034_RcvTime >= RCV_MAX_TIME)
//					g_ucGP9034_RcvFlag = 1;

//			}  
      if(g_ucGC9200_RcvTime < RCV_MAX_TIME){				
			  g_ucGC9200_RcvTime++;
				if(g_ucGC9200_RcvTime >= RCV_MAX_TIME)
					g_ucGC9200_RcvFlag = 1;
			}
			if(g_ucBeepOnCnt < BEEP_ON_TIME){
          g_ucBeepOnCnt++;
          if(g_ucBeepOnCnt >= BEEP_ON_TIME){					
              BEEP_OFF();
          }else{
              BEEP_ON();
		      }
	    }
											
//			if(g_uiSentIntervalCnt < INTERVAL_MAX ){
//			  g_uiSentIntervalCnt++;
//			}
			
	    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  
   }
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
