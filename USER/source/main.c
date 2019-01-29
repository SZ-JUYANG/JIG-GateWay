/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    1-August-2016
  * @brief   Main program body
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
//#include "stm32f10x.h"
#include "LCD12864B.h"
#include "STM32_UART.h"
#include "System_Initialize.h"
#include "main.h"
#include <stdio.h>
#include "string.h"
#include "includes.h"
#include "KeyDriver.h"
#include "MENU.h"
#include "delay.h"
#include "TaskApp.h"
#include "IT8512B.h"
#include "AT24Cxx.h"
#include "RTC.h"
#include "LoraC8000.h"
#include "LoraC6000.h"
#include "RS485.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//volatile unsigned short int g_uiLedCnt = 0;
//volatile unsigned short int g_uiKeyScantCnt = 0;
//volatile unsigned char      g_ucBeeperCnt = 0;
//volatile unsigned short int g_uiLightCnt = 0;
//volatile unsigned short int g_uiFuncRepeatCnt = 0;
unsigned char JIG_ParameterInit(void);
unsigned char  g_ucTimeEnable = 1;    //ʹ�� ��ʾʱ�� ��־

unsigned char  g_ucPM9805_RcvTime = RCV_MAX_TIME; //
unsigned char  g_ucIT8512_RcvTime = RCV_MAX_TIME;
unsigned char  g_ucGP9034_RcvTime = RCV_MAX_TIME;
unsigned char  g_ucGC9200_RcvTime = RCV_MAX_TIME;
unsigned char  g_ucC8000_RcvTime = RCV_MAX_TIME;
unsigned char  g_ucC6000_RcvTime = RCV_MAX_TIME;

unsigned char  g_ucPM9805_RcvFlag = 0; 
unsigned char  g_ucIT8512_RcvFlag = 0;
unsigned char  g_ucGP9034_RcvFlag = 0;
unsigned char  g_ucGC9200_RcvFlag = 0;
unsigned char  g_ucC8000_RcvFlag = 0;
unsigned char  g_ucC6000_RcvFlag = 0;

unsigned char g_ucPM9805_RecBuf[PM9805_RCV_NUM];
unsigned char g_ucIT8512_RecBuf[IT8512_RCV_NUM];
unsigned char g_ucGP9034_RecBuf[GP9034_RCV_NUM];
unsigned char g_ucGC9200_RecBuf[GC9200_RCV_NUM];
unsigned char g_ucC8000_RecBuf[C8000_RCV_NUM ];
unsigned char g_ucC6000_RecBuf[C6000_RCV_NUM ];
unsigned char g_ucC8000_RecLen = 0;
unsigned char g_ucC6000_RecLen = 0;

unsigned char  g_ucEnableFlag = 0;  // ʹ��  ��־
unsigned char  g_ucKeySetFlag = 0;

extern unsigned int g_uiCurrentID;
//extern ScmPM9805TypeDef PM9805Value;
//extern ScmCorrectionTypeDef  DPS_WritePara;
//extern ScmDPSTypeDef         DPS_InquirePara; // ��ѯ�����Ĳ��� 
//extern ScmIT8512TypeDef ScmIT8512_1;

ScmAT24CxxTypeDef    ScmAT24CxxIdData;  //�洢��ID �豸�롭��
ScmAT24CxxErrTypeDef ScmAT24CxxErr;     //���ϵ��

ScmAT24CxxErrTypeDef ScmDPSErrVal; // 
//const ScmErrTypeDef ScmDPSErr;   //���������Сֵ

//unsigned char  g_ucAT24CxxBuf[30]={0x00};  //��ȡEEPROM����
char cTemp[16]={0x00};
extern unsigned char g_ucBeepOnCnt;

unsigned char g_ucNodeID[6][1] = {0x00};//�ξ߽ڵ�ID
unsigned char g_ucNodeNum = 0;

unsigned int g_uiCurrentID = 0x01004268;      //��Ҫʹ�õ�ID
unsigned int g_uiFailID[FAIL_ID_MAX]={0x00} ;  //����δ�ɹ���ID �´���
unsigned char ucFailIDNum = 0; 
unsigned int g_uiUsedID_Num = 0;//

unsigned short int uiSelfID = 0;
unsigned  int ulPLC_ID = 0;

unsigned char g_ucRxLedOnCnt = 2;
unsigned char g_ucRx_D_LedOnCnt = 2;

ScmDPS_PNTypeDef ScmDPS_Para; // �洢 DPS��� ����/��ѹ �豸��
ScmJIGTypeDef    ScmJIG;      // �洢 �ξ�

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private functions ---------------------------------------------------------*/

//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		512
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
__align(8) static CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

#define LED0_TASK_PRIO		6
#define LED0_STK_SIZE 		512
OS_TCB Led0TaskTCB;
__align(8) static CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_task(void *p_arg);


#define LED1_TASK_PRIO		5	
#define LED1_STK_SIZE 		512
OS_TCB Led1TaskTCB;	
__align(8) static CPU_STK LED1_TASK_STK[LED1_STK_SIZE];
void led1_task(void *p_arg);

#define KEY_TASK_PRIO		4
#define KEY_STK_SIZE		1024
OS_TCB	KeyTaskTCB;
__align(8) static  CPU_STK	KEYT_TASK_STK[KEY_STK_SIZE];
void Key_task(void *p_arg);

unsigned char ucHeartBeatCnt = 0;

unsigned char g_ucEepromError = 0;//
	
int main(void)
{	
	OS_ERR err;
	CPU_SR_ALLOC();
	STM32_UART_Init(); // 4�����ڳ�ʼ��
	Lcd12864_init();
	BEEP_GPIO_Init();
	LED_GPIO_Init();
	Key_GPIO_Config();
	RELAY_GPIO_Init();
	AT24CXX_Init();
  delay_init();      // ��ʱ��ʼ��
	RTC_Config();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ�������
	TIM2_Configuration();	

  delay_ms(1);
	g_ucBeepOnCnt = 0;

  delay_ms(200);
	IWDG_Init(7,0xff);
	DisplayChar(1,8,0x03);//03-�ģ�20-�ո�
	
	IWDG_Feed();
	OSInit(&err);		     //��ʼ��UCOSIII
	OS_CRITICAL_ENTER(); //�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		    //��������
                 (OS_TASK_PTR )start_task, 	//������
                 (void		* )0,					    //���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY)0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT    )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);			//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);      //����UCOSIII
	while(1);
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
//PUTCHAR_PROTOTYPE
//{
//  return ch;
//}
int fputc(int ch, FILE *f)
{  
//   USART_SendData(USART2, (unsigned char) ch);
// 
//   while(!(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == SET))
//   {
//   }
   return ch;
}
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  //ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	
	DataInit(); //���ݳ�ʼ��		
	
	//����LED0����
	OSTaskCreate((OS_TCB 	* )&Led0TaskTCB,		
				 (CPU_CHAR	* )"led0 task", 		
                 (OS_TASK_PTR )led0_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED0_TASK_PRIO,     
                 (CPU_STK   * )&LED0_TASK_STK[0],	
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED0_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
				 
	//����LED1����
	OSTaskCreate((OS_TCB 	* )&Led1TaskTCB,		
				 (CPU_CHAR	* )"led1 task", 		
                 (OS_TASK_PTR )led1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED1_TASK_PRIO,     	
                 (CPU_STK   * )&LED1_TASK_STK[0],	
                 (CPU_STK_SIZE)LED1_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	
			 
	//����Key����
	OSTaskCreate((OS_TCB 	* )&KeyTaskTCB,		
				 (CPU_CHAR	* )"Key_task", 		
                 (OS_TASK_PTR )Key_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )KEY_TASK_PRIO,     	
                 (CPU_STK   * )&KEYT_TASK_STK[0],	
                 (CPU_STK_SIZE)KEY_STK_SIZE/10,	
                 (CPU_STK_SIZE)KEY_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);


	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);	//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}
//	unsigned short int uiSeconds = 0;
//  unsigned char ucF = 0;
//  unsigned char ucSend[5]={0x01,0x02,0x03,0x04,0x05};
//	unsigned char uctest = 0;
//led0������
unsigned char ucTimeOutFunCnt = 0;
void led0_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;

	while(1)
	{
		IWDG_Feed();	         //��ʱι��					
		TimeOutCounter();		 //�ж��Ƿ��з����ȥ��ID��ʱ����գ���ʾ��æ�����ߡ��С�	
		if(g_ucRxLedOnCnt < 2){  //���յ��ڵ�ͨ��LORA������������g_ucRxLedOnCnt���㣬��Ӧ��LED����һ��
			g_ucRxLedOnCnt++;
			if(g_ucRxLedOnCnt >= 2){
				RX_LED_OFF();
			}else{
				RX_LED_ON();    
			}					
		}	
		if(g_ucRx_D_LedOnCnt < 2){
			g_ucRx_D_LedOnCnt++;
			if(g_ucRx_D_LedOnCnt >= 2){
				RX_D_LED_OFF();
			}else{
				RX_D_LED_ON();    
			}					
		}				
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ250ms
	}
}
unsigned char ucEEPROM_0[50] = {0x00};
unsigned char g_ucNodeCurrentIndex = 0;
//led1������
void led1_task(void *p_arg)
{ 
	unsigned char ucNodeDisplay[] = "JID:xx,xx,xx,xx";
	unsigned char ucNodeGroup;
	unsigned char ucIndex;
	unsigned char ucFlashFlag = 0; /* ��˸�ı�־ */
	OS_ERR err;
	p_arg = p_arg;
	while(1){     //��ʾ��Ļ���Ͻ����������ࡣ
		extern unsigned char ucNodeIDTable[];	
		ucHeartBeatCnt++;
		if(ucHeartBeatCnt == 3){
			ucFlashFlag = 1;
			DisplayChar(1,8,0x03); //0x03 heart
		}else if(ucHeartBeatCnt == 6){
			ucFlashFlag = 0;
			ucHeartBeatCnt = 0;    
			DisplayChar(1,8,0x20);//0x20 space
		} 	
	/* ��ʾ���ξ�ID */
		ucNodeGroup = g_ucNodeCurrentIndex / 4;
		ucIndex = ucNodeGroup * 4;
		/* ��ʾ��1�����ξ�ID */
		if((ucIndex < NODE_NUM) && (ucIndex != g_ucNodeCurrentIndex || ucFlashFlag)){
			/* ��ʾ��������1�������±��ں���Χ��2�����ǵ�ǰͨ�ŵĴӽڵ��ξ߻��߲�Ҫ����˸ */
			ucNodeDisplay[4] = ucNodeIDTable[ucIndex]%100/10 + 0x30;
			ucNodeDisplay[5] = ucNodeIDTable[ucIndex]%10 + 0x30;
		}else{/* ����ID��������1�������±곬�ޣ�2���ǵ�ǰͨ�ŵĴӽڵ��ξ���������˸�ر�״̬ */
			ucNodeDisplay[4] = ' ';
			ucNodeDisplay[5] = ' ';
		}
		/* ��ʾ��2�����ξ�ID */
		ucIndex++;
		if((ucIndex < NODE_NUM) && (ucIndex != g_ucNodeCurrentIndex || ucFlashFlag)){
			ucNodeDisplay[6] = ',';
			ucNodeDisplay[7] = ucNodeIDTable[ucIndex]%100/10 + 0x30;
			ucNodeDisplay[8] = ucNodeIDTable[ucIndex]%10 + 0x30;
		}else{
			if(ucIndex < NODE_NUM){ /* ���Ų���˸ */
			  ucNodeDisplay[6] = ',';
			}else{
				ucNodeDisplay[6] = ' ';
			}
			ucNodeDisplay[7] = ' ';
			ucNodeDisplay[8] = ' ';
		}
		/* ��ʾ��3�����ξ�ID */
		ucIndex++;
		if((ucIndex < NODE_NUM) && (ucIndex != g_ucNodeCurrentIndex || ucFlashFlag)){
			ucNodeDisplay[9] = ',';
			ucNodeDisplay[10] = ucNodeIDTable[ucIndex]%100/10 + 0x30;
			ucNodeDisplay[11] = ucNodeIDTable[ucIndex]%10 + 0x30;
		}else{
			if(ucIndex < NODE_NUM){ /* ���Ų���˸ */
			  ucNodeDisplay[9] = ',';
			}else{
				ucNodeDisplay[9] = ' ';
			}
			ucNodeDisplay[10] = ' ';
			ucNodeDisplay[11] = ' ';
		}
		/* ��ʾ��4�����ξ�ID */
		ucIndex++;
		if((ucIndex < NODE_NUM) && (ucIndex != g_ucNodeCurrentIndex || ucFlashFlag)){
			ucNodeDisplay[12] = ',';
			ucNodeDisplay[13] = ucNodeIDTable[ucIndex]%100/10 + 0x30;
			ucNodeDisplay[14] = ucNodeIDTable[ucIndex]%10 + 0x30;
		}else{
			if(ucIndex < NODE_NUM){ /* ���Ų���˸ */
			  ucNodeDisplay[12] = ',';
			}else{
				ucNodeDisplay[12] = ' ';
			}
			ucNodeDisplay[13] = ' ';
			ucNodeDisplay[14] = ' ';
		}
#if WRITE_EEPROM_EN == 0
		if(!g_ucEepromError){
		  DisplayString(2,1,ucNodeDisplay);
	  }
#endif
		OSTimeDlyHMSM(0,0,0,150,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ100ms
	}
}

//led1������ 
char cDis[2]={0x00};
unsigned char ucRes[2] = {0x68,0x16};
unsigned char ucWriteDataBuf[4]={0x00};
void Key_task(void *p_arg)
{
#if WRITE_EEPROM_EN == 1
	static unsigned char s_ucFlag = 1;
#endif
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{  
#if WRITE_EEPROM_EN == 0		
		if(g_ucEepromError == 0){
			TaskOperation();  //��Ҫ�ĺ���
		}else if(g_ucEepromError == 0xff){
			
		}else if(g_ucEepromError == EEPROM_ERR_09){
			DisplayString(2,1,"�ξ�IDʹ����");
			sprintf(cDis,"%2d",g_ucEepromError);
			DisplayString(2,7,cDis);	//��ʾ
			g_ucEepromError = 0xff;
		}else{		    
			DisplayString(3,1,"�ξߴ洢����");
			sprintf(cDis,"%2d",g_ucEepromError);
			DisplayString(3,7,cDis);	//��ʾ
			g_ucEepromError = 0xff;
		}			
    if(g_ucGC9200_RcvFlag > 0){        //buffer available
			if((g_ucGC9200_RecBuf[0] == 0xA1)&&(g_ucGC9200_RecBuf[1] == 0xA1)&&(g_ucGC9200_RecBuf[2] == 0xA1)){//���ó�ʼֵ main
//        g_uiCurrentID = 0x04020001;//0x04000119;  //ID
//			  g_uiUsedID_Num = 0;
//				ucFailIDNum = 0;
//				memset(g_uiFailID,0,FAIL_ID_MAX);
//					
//				SaveFailID_ToEEPROM();
//					
//				ucWriteDataBuf[0] = g_uiCurrentID>>24&0xff;   // in main.c 
//				ucWriteDataBuf[1] = (g_uiCurrentID>>16)&0xff;
//				ucWriteDataBuf[2] = (g_uiCurrentID>>8)&0xff;
//				ucWriteDataBuf[3] = (g_uiCurrentID>>0)&0xff;
//				SaveDadaToEEPROM(PLC_CURRENT_ID_ADD,ucWriteDataBuf,4);
//					
//				ucWriteDataBuf[0] = g_uiUsedID_Num>>24&0xff;
//				ucWriteDataBuf[1] = (g_uiUsedID_Num>>16)&0xff;
//				ucWriteDataBuf[2] = (g_uiUsedID_Num>>8)&0xff;
//				ucWriteDataBuf[3] = (g_uiUsedID_Num>>0)&0xff;
//				SaveDadaToEEPROM(ID_USED_NUM_ADD,ucWriteDataBuf,4);	//
//				GC9200_SendString(ucRes,2);
			}else{
				ReadEepromData(g_ucGC9200_RecBuf);				
			}								
			g_ucGC9200_RecBuf[0] = 0;
			g_ucGC9200_RecBuf[1] = 0;
			g_ucGC9200_RcvFlag = 0;				
		}
		OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ20ms
//////add by wzh//////////
#elif WRITE_EEPROM_EN == 1
		if(s_ucFlag){
			s_ucFlag = 0;/* ֻдһ�� */
			g_uiCurrentID = ID_START;//0x04000119;  //ID
			g_uiUsedID_Num = 0;
			ucFailIDNum = 0;
			memset(g_uiFailID,0,FAIL_ID_MAX);
				
			SaveFailID_ToEEPROM();
				
			ucWriteDataBuf[0] = g_uiCurrentID>>24&0xff;   // in main.c 
			ucWriteDataBuf[1] = (g_uiCurrentID>>16)&0xff;
			ucWriteDataBuf[2] = (g_uiCurrentID>>8)&0xff;
			ucWriteDataBuf[3] = (g_uiCurrentID>>0)&0xff;
			SaveDadaToEEPROM(PLC_CURRENT_ID_ADD,ucWriteDataBuf,4);
				
			ucWriteDataBuf[0] = g_uiUsedID_Num>>24&0xff;
			ucWriteDataBuf[1] = (g_uiUsedID_Num>>16)&0xff;
			ucWriteDataBuf[2] = (g_uiUsedID_Num>>8)&0xff;
			ucWriteDataBuf[3] = (g_uiUsedID_Num>>0)&0xff;
			SaveDadaToEEPROM(ID_USED_NUM_ADD,ucWriteDataBuf,4);	//
			GC9200_SendString(ucRes,2);
			OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ20ms
		}else{ /*����д���ID*/ 
			unsigned char CheckEEPROM_Frame(unsigned char *pucInputData,unsigned char ucLen);
			unsigned char ucTempEEPROM[50] = {0x00};
			char cPrintTable[16] = {0x00};
			AT24CXX_Read(PLC_CURRENT_ID_ADD,ucTempEEPROM,8);//��ȡ��ǰID(δʹ��)
			if(CheckEEPROM_Frame(ucTempEEPROM,8)){
				g_uiCurrentID = ucTempEEPROM[2];
				g_uiCurrentID = (g_uiCurrentID << 8) + ucTempEEPROM[3];
				g_uiCurrentID = (g_uiCurrentID << 8) + ucTempEEPROM[4];
				g_uiCurrentID = (g_uiCurrentID << 8) + ucTempEEPROM[5];
				//break;
			}
			sprintf(cPrintTable,"ID:%08X",g_uiCurrentID);
			while(1){
				DisplayString(2,1,"  ID�޸����  ");
				DisplayString(3,1,cPrintTable);
			  OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); 
			}
		}
#endif 	
	}
}


