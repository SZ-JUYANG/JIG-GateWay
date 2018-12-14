#include "stm32f10x.h"
#include "delay.h"
#include "System_Initialize.h"
#include "main.h"
#include "STM32_UART.h"
#include <string.h>
#include "includes.h"
#include "LoraC8000.h"
#include "AT24Cxx.h"
#include "LCD12864B.h"

//#include "TaskApp.h"
//#include "IT8512B.h"

#define CONCEL  0
#define REQUIRE 1
#define CONFIRM 2

#define NULL_CMD        0
#define NO_ACK          1
#define REQUIRE_ID      2
#define CONFIRM_ID_OK   3
#define CONFIRM_ID_FAIL 4
#define COMPLETE        5

//#define OFF_LINE 0x00
//#define ON_LINE  0x01
 
unsigned char ucNodeIDTable[NODE_NUM]={0X01,0X02,0x04};//����ξ�LoraID  NODE_NUM define in LoraC8000.h
//unsigned char ucNodeIDTable[NODE_NUM]={0x04};

ScmTask fg_sTask[TASK_MAX];

//ScmResponseTask sResponseTask[TASK_MAX];//

unsigned char g_ucResponseNum = 0;

unsigned char ucTaskNum = 0;

extern unsigned int  g_uiCurrentID;             //��Ҫʹ�õ�ID
extern unsigned int  g_uiFailID[FAIL_ID_MAX] ;  //����δ�ɹ���ID �´���
extern unsigned char ucFailIDNum;              //
extern unsigned int  g_uiUsedID_Num;

//static unsigned char  fg_ucBufInIx  = 0;            //buf��ָ��                   
//static unsigned char  fg_ucBufOutIx = 0;            //buf��ָ��

//static unsigned char  fg_ucTaskBufInIx  = 0;        //buf��ָ��                   
//static unsigned char  fg_ucTaskBufOutIx = 0;        //buf��ָ��

//unsigned int g_uiWaitVerifyID[10];
//unsigned char ucWaitVerifyIDNum = 0;

// Byte0 0x68
// Byte1 L
// Byte2-Byte5    --> current ID
// Byte6-Byte9    --> used ID Num
// Byte10         --> Fail ID Num
// Bytexx-Bytexx  --> Fail ID 

unsigned char ucBackupDataFlash[2+9+4*FAIL_ID_MAX] = {0x00}; //
	
char cPrintTable[16] = {0x00};

unsigned char g_ucBusyFlag = 0;  //0 IDLE,1 busy

unsigned short int g_uiSentIntervalCnt ;

extern unsigned char g_ucC8000_RcvFlag;
extern unsigned char g_ucC8000_RecBuf[C8000_RCV_NUM ];
extern unsigned char g_ucC8000_RecLen;
extern unsigned char g_ucNodeID[6][1] ;
extern unsigned char g_ucNodeNum ;

extern unsigned char g_ucBeepOnCnt;
extern unsigned char g_ucRxLedOnCnt;
extern unsigned char g_ucRx_D_LedOnCnt;
extern unsigned char g_ucEepromError;


void TaskBufferDeal(ScmTask sNewTask);
void SendID_ToNode(unsigned char ucJigID,unsigned int uiPlcID);
unsigned char ReadFailID_FromEEPROM(void);
void ConfirmID(unsigned char ucJigID,unsigned char ucVal);
void TaskBufferRefresh(void);
ScmResponseTask ReadResponseTask(void);
/* ����ǰʹ�õ�ID���Ѿ�ʹ�õ�ID����������ʧ�ܵ�ID����������ʧ�ܵ�ID���浽RAM���� */
/* ����������±꣬������һ��ռ�õ��ֽ����� */
unsigned char LoadDataToBuffer(void)
{
	unsigned char ucIndex = 0;
	unsigned char i;
	ucBackupDataFlash[ucIndex++] = 0x68;
	ucBackupDataFlash[ucIndex++] = 2+9+ucFailIDNum*4; //frame L

	ucBackupDataFlash[ucIndex++] = (g_uiCurrentID>>24)&0xff;
	ucBackupDataFlash[ucIndex++] = (g_uiCurrentID>>16)&0xff;
	ucBackupDataFlash[ucIndex++] = (g_uiCurrentID>>8)&0xff;
	ucBackupDataFlash[ucIndex++] = (g_uiCurrentID>>0)&0xff;

	ucBackupDataFlash[ucIndex++] = (g_uiUsedID_Num>>24)&0xff;
	ucBackupDataFlash[ucIndex++] = (g_uiUsedID_Num>>16)&0xff;
	ucBackupDataFlash[ucIndex++] = (g_uiUsedID_Num>>8)&0xff;
	ucBackupDataFlash[ucIndex++] = (g_uiUsedID_Num>>0)&0xff;	

	ucBackupDataFlash[ucIndex++] = ucFailIDNum;
	for(i=0;i<ucFailIDNum;i++){
		ucBackupDataFlash[ucIndex++] = (g_uiFailID[i]>>24)&0xff;
		ucBackupDataFlash[ucIndex++] = (g_uiFailID[i]>>16)&0xff;
		ucBackupDataFlash[ucIndex++] = (g_uiFailID[i]>>8)&0xff;
		ucBackupDataFlash[ucIndex++] = (g_uiFailID[i]>>0)&0xff;
	}
	return ucIndex;
}
/*
extern unsigned int  g_uiCurrentID;             //��Ҫʹ�õ�ID
extern unsigned int  g_uiFailID[FAIL_ID_MAX] ;  //����δ�ɹ���ID �´���
extern unsigned char ucFailIDNum;              //
extern unsigned int  g_uiUsedID_Num;
*/

unsigned char CompareTwoString(unsigned char *p1,unsigned char *p2,unsigned char ucNum)
{
	unsigned char i = 0;
	for(i = 0;i < ucNum;i++){
		if(p1[i] != p2[i]){
			return 0;	
		}		
	}	
	return 1;	
}
/*******************************************************************************
  * @brief   CheckEEPROM_Frame
  * @param   ���EEPROM��֡��ʽ
  * @retval  0 fail,1 success
*******************************************************************************/
//A5 L Data cs 5A ,L = Length(Data),CS = Sum(L + Data)
unsigned char CheckEEPROM_Frame(unsigned char *pucInputData,unsigned char ucLen)
{
	unsigned char i,cs = 0;
	if(ucLen < 4)
		return 0;
	
	if((pucInputData[0] != 0xA5)||(pucInputData[ucLen-1] != 0x5A))
		return 0;
	
	for(i = 1;i < ucLen-2;i++)
		cs += pucInputData[i];		
	if(pucInputData[ucLen-2] != cs)
		return 0;	
	return 1;
}
/*******************************************************************************
  * @brief   SaveDadaToEEPROM
  * @param   д��EEPROM������Ŀ������ ���ȣ�
  * @retval  0 fail,1 success
*******************************************************************************/
unsigned char  SaveDadaToEEPROM(unsigned short int uiStartAdd,unsigned char *pucData,unsigned char ucLen)
{
	unsigned char i,cs = 0,ucIndex = 0;
	unsigned char ucTempEEPROM[FAIL_ID_MAX*4+4] = {0x00};//max ID Num 
	unsigned char ucReadEEPROM[FAIL_ID_MAX*4+4] = {0x00};
	OS_ERR err; 

	ucTempEEPROM[ucIndex++] = 0xA5;
	ucTempEEPROM[ucIndex++] = ucLen;
	for(i=0;i<ucLen;i++){
		ucTempEEPROM[ucIndex++] = pucData[i];		
	}
	for(i=1;i<(ucLen+2);i++){
		cs += ucTempEEPROM[i];
	}
	ucTempEEPROM[ucIndex++] = cs;	
	ucTempEEPROM[ucIndex] = 0x5A;
	for(i=0;i<3;i++){
		AT24CXX_Write(uiStartAdd,ucTempEEPROM,ucLen+4);
		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_HMSM_STRICT,&err);
		AT24CXX_Read(uiStartAdd,ucReadEEPROM,ucLen+4);
		if(CompareTwoString(ucTempEEPROM,ucReadEEPROM,ucLen+4))/* �Ƚ������ַ����Ƿ����*/
			return 1;		
		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_HMSM_STRICT,&err);
	}		 		
	return 0;
}


/*******************************************************************************
  * @brief   DataInit
  * @param   д��EEPROM������Ŀ������ ���ȣ�
  * @retval  0 fail,1 success
*******************************************************************************/
void DataInit(void)
{
	unsigned char i,j,ucNum = 0; //
	unsigned char ucTempEEPROM[50] = {0x00};
	OS_ERR err;
	//AT24CXX_Write(0,ucTempEEPROM,50);
	for(i=0;i<TASK_MAX;i++){           
		fg_sTask[i].ucJigID = ucNodeIDTable[i]; //װ�ؽڵ�ID
		fg_sTask[i].uiPLC_ID = 0;
		fg_sTask[i].ucState = 0;
		fg_sTask[i].ucTimeOutCnt = TIME_OUT_MAX;
		fg_sTask[i].ucTimeOutFlag = 0;
		fg_sTask[i].ucRsponseTimeCnt = TIME_RESPONSE_MAX;
		fg_sTask[i].ucRsponseType = 0;		
	}

	for(i = 0;i < 5;i++){  //5 times
		AT24CXX_Read(ID_USED_NUM_ADD,ucTempEEPROM,8);//�ѷ���ɹ�ID���� 
		if(CheckEEPROM_Frame(ucTempEEPROM,8)){
			g_uiUsedID_Num = ucTempEEPROM[2];
			g_uiUsedID_Num = (g_uiUsedID_Num << 8) + ucTempEEPROM[3];
			g_uiUsedID_Num = (g_uiUsedID_Num << 8) + ucTempEEPROM[4];
			g_uiUsedID_Num = (g_uiUsedID_Num << 8) + ucTempEEPROM[5];					            
			break;
		}
		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_HMSM_STRICT,&err);
	}

	sprintf(cPrintTable,"%6d",g_uiUsedID_Num);
	DisplayString(4,6,cPrintTable);	//��ʾ

	for(i = 0;i < 5;i++){ 
		AT24CXX_Read(PLC_CURRENT_ID_ADD,ucTempEEPROM,8);//��ȡ��ǰID(δʹ��)
		if(CheckEEPROM_Frame(ucTempEEPROM,8)){
			g_uiCurrentID = ucTempEEPROM[2];
			g_uiCurrentID = (g_uiCurrentID << 8) + ucTempEEPROM[3];
			g_uiCurrentID = (g_uiCurrentID << 8) + ucTempEEPROM[4];
			g_uiCurrentID = (g_uiCurrentID << 8) + ucTempEEPROM[5];
		#if ID_LIMIT_EN == 1
			if(g_uiCurrentID < ID_START || g_uiCurrentID >= ID_END){ /* �ж�ID�Ƿ񳬳����� */
				g_ucEepromError = EEPROM_ERR_09;
			}
		#endif
			break;
		}
		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_HMSM_STRICT,&err); //
	}
	if( i>=5 ){
		g_ucEepromError = EEPROM_ERR_03;
	}

	ReadFailID_FromEEPROM();

	DisplayString(1,2,"��������");
	DisplayString(1,7,"��");
	sprintf(cPrintTable,"ID:%08X",g_uiCurrentID);
	DisplayString(3,1,cPrintTable);	
}

/*******************************************************************************
  * @brief  SaveFailID_ToEEPROM
  * @param  
  * @retval 
*******************************************************************************/ 
void SaveFailID_ToEEPROM(void)
{
	unsigned char i,j;
	union {
		unsigned int uiID;
		unsigned char ucBuf[4];
	}unID;
	OS_ERR err;
	unsigned char ucTempFailID[FAIL_ID_MAX*4]= {0};
	unsigned char ucReadFailID[FAIL_ID_MAX*4]= {0}; //

    //�洢ʧ��ID�������������洢ʧ�������Ӧ����
	if(SaveDadaToEEPROM(PLC_FAIL_ID_NUM_ADD,&ucFailIDNum,1) == 0){//��ʧ��ID������
		g_ucEepromError = EEPROM_ERR_04; //д�����ʧ�ܵ�ID��������
		i = LoadDataToBuffer();  //����ǰʹ�õ�ID���Ѿ�ʹ�õ�ID����������ʧ�ܵ�ID����������ʧ�ܵ�ID���浽RAM����
		WriteDataToFlash(ucBackupDataFlash,FLASH_BACKUP_PAGE,i);//����FLASH
		return;
	}         

    /* ��ʧ�ܵ�ID�ֽ���ֽڴ��뻺��ucTempFailID */
	for(i = 0;i < ucFailIDNum;i++){    
		unID.uiID = g_uiFailID[i]; 
		for(j = 0;j < 4;j++){
			ucTempFailID[i*4+j] = unID.ucBuf[3-j];
		}
	}	
    //����ʧ�ܵ�ID��������ʧ��������Ӧ����
	if(SaveDadaToEEPROM(PLC_FAIL_ID_ADD,ucTempFailID,ucFailIDNum*4)==0){  //��ID
		g_ucEepromError = EEPROM_ERR_05; //д�����ʧ�ܵ�ID������
		i = LoadDataToBuffer(); //����ǰʹ�õ�ID���Ѿ�ʹ�õ�ID����������ʧ�ܵ�ID����������ʧ�ܵ�ID���浽RAM����
		WriteDataToFlash(ucBackupDataFlash,FLASH_BACKUP_PAGE,i);//����FLASH
	}
}

/*******************************************************************************
  * @brief  ReadFailID_FromEEPROM
  * @param  
  * @retval 
*******************************************************************************/ 
unsigned char  ReadFailID_FromEEPROM(void)
{
	unsigned char i,j;
	union {
		unsigned int uiID;
		unsigned char ucBuf[4];
	}unID;
	unsigned char ucTempID[FAIL_ID_MAX*4+4]= {0};
	unsigned char ucNum = 0;
	OS_ERR err;

	for (i = 0;i < 5;i++){
		AT24CXX_Read(PLC_FAIL_ID_NUM_ADD,ucTempID,5);//��ȡ����ʧ�ܵ�ID ����
		if (CheckEEPROM_Frame(ucTempID,5)){
			ucFailIDNum = ucTempID[2];
			break;
		}
		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_HMSM_STRICT,&err);
	}
	if (i >= 5){
		g_ucEepromError = EEPROM_ERR_01;
		return 0;
	}	

	if (ucFailIDNum > 0){                              
		ucNum = ucFailIDNum*4+4;
		for (i = 0;i < 5;i++){
			AT24CXX_Read(PLC_FAIL_ID_ADD,ucTempID,ucNum);////��ȡ����ʧ�ܵ�ID
			if (CheckEEPROM_Frame(ucTempID,ucNum)){
				for (j = 0;j < ucFailIDNum;j++){  // g_uiFailID
					g_uiFailID[j] = ucTempID[2+(j*4)];
					g_uiFailID[j] = (g_uiFailID[j]<<8) + ucTempID[3+(j*4)];
					g_uiFailID[j] = (g_uiFailID[j]<<8) + ucTempID[4+(j*4)];								
					g_uiFailID[j] = (g_uiFailID[j]<<8) + ucTempID[5+(j*4)];
				}
				break;
			}
			OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_HMSM_STRICT,&err);
		}
		if (i >= 5){
			g_ucEepromError = EEPROM_ERR_02;
			return 0;
		}
	}
	return 1;		
}

/*******************************************************************************
  * @brief  ID_BufRefresh
  * @param  ����ˢ��ID���� 
  * @retval 
*******************************************************************************/
void ID_BufRefresh(void)
{
	unsigned char i,j;
	for (i = 0;i < ucFailIDNum;i++){
		if (g_uiFailID[i]!= 0){
			continue ;
		}
		for (j = i;j < ucFailIDNum;j++){
			if (g_uiFailID[j] != 0){
				g_uiFailID[i] = g_uiFailID[j];
				g_uiFailID[j] = 0;
				break;
			}
		}		
	}

	j = 0;
	for (i = 0; i < ucFailIDNum; i++){
		if (g_uiFailID[i] != 0){
			j++;
		}
	}
	ucFailIDNum = j;	
}

/*******************************************************************************
  * @brief  ID_BufOut
  * @param  �ӻ�����ȡ��һ��ID,������EEPROM
  * @retval 
*******************************************************************************/
static unsigned int ID_BufOut (void)
{
	unsigned char i;
	unsigned int uiReturnVal = 0;
	if(ucFailIDNum > 0){
		for(i = 0;i<ucFailIDNum;i++){
			if(g_uiFailID[i] != 0){
				uiReturnVal = g_uiFailID[i];
				g_uiFailID[i] = 0;
				//ucFailIDNum --;
				ID_BufRefresh();  //ˢ�»���
				SaveFailID_ToEEPROM();
				return uiReturnVal;
			}  
		}		
	}		
	return 0;
}
/*******************************************************************************
  * @brief  ID_BufIn
  * @param  ����һ��ID,������EEPROM
  * @retval 
*******************************************************************************/
static void  ID_BufIn (unsigned int code)
{                        
	unsigned char i;	
	if(code == 0){
		return ;
	}else{
		for(i = 0;i < ucFailIDNum;i++){
			if(g_uiFailID[i] == code){
				return ;
			}
		}
		g_uiFailID[ucFailIDNum++] = code; //��ʧ�ܵ�IDװ��ʧ��ID�������飬����ʧ��ID��������1
		SaveFailID_ToEEPROM();
	}	
}

/*******************************************************************************
  * @brief  TimeOutCounter
  * @param  200ms once
  * @retval 
*******************************************************************************/
void TimeOutCounter(void)
{
	unsigned char i;
	unsigned char ucBusyTempFlag = 0;
	//static unsigned char s_ucSentIntervalCnt = 3; //200*3 = 600  ���ͼ��

	ScmTask          sTempTask;
	ScmResponseTask  sTempReTask;

	for(i = 0;i < TASK_MAX;i++){  //Ѳ�����е�Task���ж���û�г�ʱ��Ҫ���յ�ID
		if(fg_sTask[i].ucJigID 	!= 0){
			if(fg_sTask[i].ucTimeOutCnt < TIME_OUT_MAX){ //150*200 = 30s
				fg_sTask[i].ucTimeOutCnt++;
				ucBusyTempFlag = 1;    //�ñ�־������ʾ������æ����ʾ��Ҫ�ػ�
				if(fg_sTask[i].ucTimeOutCnt >= TIME_OUT_MAX){
					ID_BufIn(fg_sTask[i].uiPLC_ID);          //����ID,���ұ��浽EEPROM
					fg_sTask[i].uiPLC_ID = 0;
					break;
				}
			}	
		}
	}
    //��ʾæ������
	if(ucBusyTempFlag > 0){  // æ
		if(g_ucBusyFlag == 0){
			g_ucBusyFlag = 1;
			DisplayString(1,2,"����ػ�");
			DisplayString(1,7,"æ");
		}
	}else{                  // ��
		if(g_ucBusyFlag == 1){
			g_ucBusyFlag = 0;
			DisplayString(1,2,"��������");
			DisplayString(1,7,"��");
		}	
	}	
}


/*****************************************************
*����ˢ�� ��������
******************************************************/
void TaskBufferRefresh(void)
{
	unsigned char i,j,ucTaskCnt = 0;
	for(i = 0;i < ucTaskNum;i++){
		if(fg_sTask[i].ucJigID 	!= 0){
			continue ;
		}
		for(j = i;j < ucTaskNum ;j++){
			if(fg_sTask[j].ucJigID 	!= 0){
				fg_sTask[i].ucJigID = fg_sTask[j].ucJigID;
				fg_sTask[i].uiPLC_ID = fg_sTask[j].uiPLC_ID;
				fg_sTask[i].ucState = fg_sTask[j].ucState;
				fg_sTask[i].ucTimeOutCnt = fg_sTask[j].ucTimeOutCnt;
				fg_sTask[i].ucTimeOutFlag = fg_sTask[j].ucTimeOutFlag;
				fg_sTask[i].ucRsponseTimeCnt = fg_sTask[j].ucRsponseTimeCnt;
				fg_sTask[i].ucRsponseType = fg_sTask[j].ucRsponseType;  

				fg_sTask[j].ucJigID = 0;
				fg_sTask[j].uiPLC_ID = 0;
				fg_sTask[j].ucState = 0;
				fg_sTask[j].ucTimeOutCnt = TIME_OUT_MAX;
				fg_sTask[j].ucTimeOutFlag = 0;
				fg_sTask[j].ucRsponseTimeCnt = TIME_RESPONSE_MAX;
				fg_sTask[j].ucRsponseType = 0;
				break;
			}
		}

		j = 0;
		for(i = 0;i < TASK_MAX;i++){
			if(fg_sTask[i].ucJigID 	!= 0){
				j++;
			}
		}
		ucTaskNum = j;			
	}
}

/*****************
*ȥ����ĳ������,�����ξ�ID
*******************/
void TaskBufferDelete(unsigned char ucJigID)
{
	unsigned char i;
	for(i = 0;i < ucTaskNum;i++){
		if(fg_sTask[i].ucJigID == ucJigID){ // �������
			fg_sTask[i].ucJigID = 0;
			fg_sTask[i].uiPLC_ID = 0;
			fg_sTask[i].ucState = 0;
			fg_sTask[i].ucTimeOutCnt = TIME_OUT_MAX;
			fg_sTask[i].ucTimeOutFlag = 0;
			fg_sTask[i].ucRsponseTimeCnt = TIME_RESPONSE_MAX;
			fg_sTask[i].ucRsponseType = 0;
		}
	}
	TaskBufferRefresh(); //ˢ������
}

/**********************
*
*g_ucResponseFlag
*********************/
/*
    unsigned char ucResponseTask[TASK_MAX]={0x00};
    unsigned char g_ucResponseNum = 0;
*/


/**********************
*DeleReponseTask
*
*********************/

/**********************
*
*////////////////


/*******************
*������
*******************/
/*
void TaskBufferDeal(ScmTask sNewTask)
{
    unsigned char i;
	  unsigned int uiTempID = 0;
	  unsigned char ucTempIDBuf[4] = {0x00};
		OS_ERR err;
    switch(sNewTask.ucState){
			case CONCEL:                                       // ȡ������ID���ջ�������
					for(i = 0;i < ucTaskNum;i++){
						if(fg_sTask[i].ucJigID == sNewTask.ucJigID){ // ����������������
							  fg_sTask[i].ucJigID = 0;
							  fg_sTask[i].ucState = 0;
                uiTempID = fg_sTask[i].uiPLC_ID;			
							  ID_BufIn(uiTempID);     // ID����							
							  fg_sTask[i].uiPLC_ID = 0;
							  TaskBufferRefresh();   // ˢ������
							  g_ucBeepOnCnt = 0;
						} 		
					}	
          AddToReponseTask(sNewTask.ucJigID,0x01);					
				break;
			case REQUIRE:          // �ڵ��������ID
					for(i = 0;i < ucTaskNum;i++){
						  if(fg_sTask[i].ucJigID == sNewTask.ucJigID){ // ������������� ID����
							    fg_sTask[i].ucState = sNewTask.ucState;
							    fg_sTask[i].ucTimeOutCnt = 0;
							//uiTempID = fg_sTask[i].uiPLC_ID ;
							//fg_sTask[i].ucTimeOutCnt = 0;
						      fg_sTask[i].ucRsponseType = 0x01;//����ID
						      fg_sTask[i].ucRsponseTimeCnt = 0;
						  //fg_sTask[i].uiPLC_ID = uiTempID;
							    break; // ����for 
						  } 		
					}
					if(i >= ucTaskNum){// ����û
						if(ucTaskNum < TASK_MAX){
						    fg_sTask[ucTaskNum].ucJigID = sNewTask.ucJigID;
							  fg_sTask[ucTaskNum].ucState = sNewTask.ucState;
//							  fg_sTask[i].ucTimeOutCnt = 0;
//							  fg_sTask[i].ucRsponseType = 0x01;//����ID
//							  fg_sTask[i].ucRsponseTimeCnt = 0;
						}else{
						    break;
						}
						
					  if(ucFailIDNum > 0){ 
						    uiTempID = ID_BufOut(); // �ӷ���ʧ�ܵ�ID������ȡ
						}else{
						    uiTempID = g_uiCurrentID;
							  g_uiCurrentID++;
							  ucTempIDBuf[0] = g_uiCurrentID>>24&0xff;
							  ucTempIDBuf[1] = (g_uiCurrentID>>16)&0xff;
							  ucTempIDBuf[2] = (g_uiCurrentID>>8)&0xff;
							  ucTempIDBuf[3] = (g_uiCurrentID>>0)&0xff;							
							  SaveDadaToEEPROM(PLC_CURRENT_ID_ADD,ucTempIDBuf,4); //��ǰID�б� ������EEPROM
							 // GC9200_SendString(unsigned char *pString,unsigned char ucLen);
							  sprintf(cPrintTable,"ID:%08X",g_uiCurrentID);
	              DisplayString(3,1,cPrintTable);
						}
						fg_sTask[ucTaskNum].ucTimeOutCnt = 0;
						fg_sTask[ucTaskNum].ucRsponseType = 0x01;//����ID
						fg_sTask[ucTaskNum].ucRsponseTimeCnt = 0;
						fg_sTask[ucTaskNum++].uiPLC_ID = uiTempID;						
					}
					g_ucBeepOnCnt = 0;
					//OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //bug 	******
					//SendID_ToNode(sNewTask.ucJigID,uiTempID);					
				  break;
			case CONFIRM:         // ID����ɹ�			
				for(i = 0;i < ucTaskNum;i++){
	          if(fg_sTask[i].ucJigID == sNewTask.ucJigID){ // �������
		            g_uiUsedID_Num++;
							  ucTempIDBuf[0] = g_uiUsedID_Num>>24&0xff;
							  ucTempIDBuf[1] = (g_uiUsedID_Num>>16)&0xff;
							  ucTempIDBuf[2] = (g_uiUsedID_Num>>8)&0xff;
							  ucTempIDBuf[3] = (g_uiUsedID_Num>>0)&0xff;							
							  SaveDadaToEEPROM(ID_USED_NUM_ADD,ucTempIDBuf,4); //��ǰID�б� ������EEPROM							
							  //AT24CXX_Read(ID_USED_NUM_ADD,ucTempEEPROM,8);
							  
							  sprintf(cPrintTable,"%6d",g_uiUsedID_Num);
							  DisplayString(4,6,cPrintTable);
							
							  //OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //bug 	****
							  //ConfirmID(sNewTask.ucJigID,0x01);
								fg_sTask[i].ucRsponseType = 0x02;//ȷ�Ϸ���ID
						    fg_sTask[i].ucRsponseTimeCnt = 0;
							  break;
		        }
	      }
				if(i >= ucTaskNum){   //���񲻴���
//				    fg_sTask[ucTaskNum].ucJigID = sNewTask.ucJigID;
//					  fg_sTask[ucTaskNum].uiPLC_ID = 0;
//					  fg_sTask[ucTaskNum].ucRsponseType = 0x02;              //ȷ�Ϸ���ID					
//					  fg_sTask[ucTaskNum++].ucRsponseTimeCnt = 0;
				    AddToReponseTask(sNewTask.ucJigID,0x01);
				}								
//        TaskBufferDelete(sNewTask.ucJigID);											
			  g_ucBeepOnCnt = 0; 
				break;											
		}			
}
*/
/*************************
****���������룬MCU��ѯC8000ģ����Ϣ
************************/
unsigned char CheckData(unsigned char *pucInputData,unsigned char ucDataLen)
{
	unsigned char ucLen = 0;
	unsigned char ucCs = 0;
	unsigned char i;
	ucLen = pucInputData[1];
	if(pucInputData[0] != 0x68)  // Header SOF
		return 0;
	if((ucLen + 4) != ucDataLen){ // Len
		return 0;
	}
	if(pucInputData[ucDataLen-1] != 0x16)  // EOF
		return 0;
	for(i=0;i<ucLen;i++){
		ucCs += pucInputData[2+i];
	}
	if(ucCs != pucInputData[ucDataLen-2]) //cs
		return 0;
	return pucInputData[2];
}
/*************************
**0ʧ��1�ɹ���MCU��ѯC8000ע��Ľڵ���Ϣ
************************/
unsigned char ReadRegisterNodeID(void)
{
 //void C8000_SendString(unsigned char *pString,unsigned char ucLen);
	unsigned char ucReadRegisterNodeID_CMD[5] = {0x68,0x01,0x21,0x21,0x16};
	unsigned char i,j,k;
	OS_ERR err;	
	for(j = 0;j < 3;j++){
		memset(g_ucC8000_RecBuf,0,C8000_RCV_NUM); // g_ucC8000_RecBuf[C8000_RCV_NUM ];
		g_ucC8000_RcvFlag = 0;
		C8000_SendString(ucReadRegisterNodeID_CMD,5);
		for(i = 0;i < 70;i++){
			if(g_ucC8000_RcvFlag > 0){ //
				if(CheckData(g_ucC8000_RecBuf,g_ucC8000_RecLen)==0xA1){// CMD 
					g_ucNodeNum = g_ucC8000_RecBuf[5];
					for(k=0;k<g_ucNodeNum;k++){
						g_ucNodeID[k][0] = g_ucC8000_RecBuf[3]+k; // Read ID number 
					}
					g_ucC8000_RcvFlag = 0;
					memset(g_ucC8000_RecBuf,0,g_ucC8000_RecLen);
					return 1;	
				}
				g_ucC8000_RcvFlag = 0;
				memset(g_ucC8000_RecBuf,0,g_ucC8000_RecLen);								
			}											
			OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);	//OS_ERR err;
		}			
	}
	return 0;	
}	
/********************************************
** C8000��������, pucInputDataΪ͸��������
**
************************/
void C8000SendData(unsigned char ucJigID,unsigned char *pucInputData,unsigned char ucDataLen)
{
	unsigned char i,j,ucLen;
	unsigned char cs = 0;
	unsigned char ucIndex = 0;
	unsigned char ucDataBuf[30] = {0x00};
	ucLen = ucDataLen + 3;          //1Byte CMD 2 Bytes ID 
	ucDataBuf[ucIndex++] = 0x68;
	ucDataBuf[ucIndex++] = ucLen;
	ucDataBuf[ucIndex++] = 0x40;    //CMD
	ucDataBuf[ucIndex++] = ucJigID; //ID L
	ucDataBuf[ucIndex++] = 0;       //ID H
	for (i=0;i<ucDataLen;i++){
		ucDataBuf[ucIndex++] = pucInputData[i];
	}
	for (i=0;i<ucLen;i++){
		cs += ucDataBuf[2+i];
	}
	ucDataBuf[ucIndex++] = cs;
	ucDataBuf[ucIndex] = 0x16;
	C8000_SendString(ucDataBuf,ucLen+4);
}
/*************************
** ��������
************************/
void SendHeartBeatToNode(unsigned char ucJigID)
{
//  unsigned char i;
//	unsigned char cs = 0;
//  unsigned char ucPDT_Buf[10] = {0x00};
//  ucPDT_Buf[0] = 0x68;
//  ucPDT_Buf[1] = 0x81;
//  ucPDT_Buf[2] = 0x05;
//	ucPDT_Buf[3] = 0x10;
//	ucPDT_Buf[4] = uiPlcID>>24;
//	ucPDT_Buf[5] = (uiPlcID>>16)&0xff;
//	ucPDT_Buf[6] = (uiPlcID>>8)&0xff;
//	ucPDT_Buf[7] = (uiPlcID>>0)&0xff;
//	for(i=0;i<8;i++){
//	  cs += ucPDT_Buf[i];
//	}
//  ucPDT_Buf[8] = cs;
//	ucPDT_Buf[9] = 0x16;
    unsigned char ucHeardBeatCmd[6]={0x68,0x02,0x01,0xF1,0x5c,0x16};
    C8000SendData(ucJigID,ucHeardBeatCmd,6);
	//GC9200_SendString(ucPDT_Buf,10);
}

/*************************
** ����ID���ξ�
************************/
void SendID_ToNode(unsigned char ucJigID,unsigned int uiPlcID)
{
	unsigned char i;
	unsigned char cs = 0;
	unsigned char ucPDT_Buf[10] = {0x00};
	ucPDT_Buf[0] = 0x68;
	//ucPDT_Buf[1] = 0x81;
	ucPDT_Buf[1] = 0xA2;  //C
	ucPDT_Buf[2] = 0x05;  //L
	ucPDT_Buf[3] = 0xF2;  //CMD
	ucPDT_Buf[4] = uiPlcID>>24;
	ucPDT_Buf[5] = (uiPlcID>>16)&0xff;
	ucPDT_Buf[6] = (uiPlcID>>8)&0xff;
	ucPDT_Buf[7] = (uiPlcID>>0)&0xff;
	for(i=0;i<8;i++){
		cs += ucPDT_Buf[i];
	}
	ucPDT_Buf[8] = cs;
	ucPDT_Buf[9] = 0x16;
	C8000SendData(ucJigID,ucPDT_Buf,10);
	//GC9200_SendString(ucPDT_Buf,10);
}
/*************************
** �ظ�ȷ�Ϸ���ID���ξ� ucVal 0 fail, 1 success
************************/
void ConfirmID(unsigned char ucJigID,unsigned char ucVal)
{
	unsigned char i;
	unsigned char cs = 0;
	unsigned char ucPDT_Buf[10] = {0x00};
	if(ucJigID == 0){
		return;
	}
	ucPDT_Buf[0] = 0x68;
	//ucPDT_Buf[1] = 0xA1;
	ucPDT_Buf[1] = 0xA2; //C
	ucPDT_Buf[2] = 0x02; //L
	//ucPDT_Buf[3] = 0x11;//CMD
	ucPDT_Buf[3] = 0xF4;//CMD
	ucPDT_Buf[4] = ucVal;
	for(i=0;i<5;i++){
		cs += ucPDT_Buf[i];
	}
	ucPDT_Buf[5] = cs;
	ucPDT_Buf[6] = 0x16;
	C8000SendData(ucJigID,ucPDT_Buf,7);
}

/*************************
** �������
************************/
void AddToTask(ScmTask sNewTask)
{
	unsigned char i;            //sResponseTask[TASK_MAX]
	for (i=0;i<TASK_MAX;i++){
		if (fg_sTask[i].ucJigID == sNewTask.ucJigID ){
			//fg_sTask[i].ucState = ON_LINE;                //����
			fg_sTask[i].ucTaskType = sNewTask.ucTaskType;						
			fg_sTask[i].ucTimeOutCnt = sNewTask.ucTimeOutCnt;
		}
	}
}

/***************************************************************************
*����յ��ڵ������
*
***************************************************************************/
void CheckReceiveData(unsigned char *pucInputData,unsigned char ucDataLen)
{
	unsigned char ucLen = 0;
	unsigned char ucCs = 0;
	unsigned char i;
	unsigned char ucCtrl = 0;
	unsigned char ucCMD = 0;
	unsigned char ucJIG_ID = 0;
	ScmTask sTempTask;	
	  //RX_D_LED_ON();
	if(pucInputData[0] != 0x68){
		return ;
	}
	ucLen = pucInputData[2];
	if ((ucLen+5) != ucDataLen){ // ����
		return ;		 
	}
	for (i=0;i<(ucLen+3);i++){
		ucCs += pucInputData[i];
	}
	if (ucCs != pucInputData[ucLen+3]){
		return ;
	}
	if (0x16 != pucInputData[ucLen+4]){
		return ;
	}
	if (0x00 == pucInputData[5]){ //ID����Ϊ0
		return ;
	}
	ucCtrl = pucInputData[1];
	ucCMD = pucInputData[3];		   			
	sTempTask.ucJigID = pucInputData[5];	
	if (ucCtrl == 0xA2){
			//RX_LED_ON();
		g_ucRxLedOnCnt = 0;			
		switch(ucCMD){
			case 0xF1://F1 ����IDǰ������
				sTempTask.ucTaskType = NO_ACK;
				sTempTask.ucTimeOutCnt = TIME_OUT_MAX;
				break;
			case 0xF2://F2 ����IDʱ������
				sTempTask.ucTaskType = REQUIRE_ID ;
				sTempTask.ucTimeOutCnt = 0;
				break;
			case 0xF3://F3 ����ID�������
				sTempTask.ucTaskType = NO_ACK;
				sTempTask.ucTimeOutCnt = 0;
				break;
			case 0xF4://F4 ȷ��ID�ɹ�������
				sTempTask.ucTaskType = CONFIRM_ID_OK;
				sTempTask.ucTimeOutCnt = TIME_OUT_MAX;
				sTempTask.uiPLC_ID = 0;
				break;
			case 0xF5://F5 ȷ��IDʧ�ܵ�����
				sTempTask.ucTaskType = CONFIRM_ID_FAIL;	
				sTempTask.ucTimeOutCnt = TIME_OUT_MAX;
				break;
			case 0xF6://F6 ȷ��ID�������
				sTempTask.ucTaskType = COMPLETE;
				sTempTask.ucTimeOutCnt = TIME_OUT_MAX;
				break;
			default:
				sTempTask.ucJigID = 0; //ID��0 �����������
				break;					
		}				
		AddToTask(sTempTask); //��ӵ��ڵ�����������ȥ���Ա�����ĳ������
				//g_ucBeepOnCnt = 0;  //����֡��ȷ  ��������һ��
	}
}	
/***************************************************************************
*TaskOperation(void)
*
***************************************************************************/
void TaskOperation(void)
{
	unsigned char i,k;        
	static unsigned char j = 0;
	static unsigned char s_ucRecvFlag = 0;
	unsigned char ucLen = 0;
	OS_ERR err;
	unsigned int uiTempID;
	unsigned char ucTempIDBuf[4];
	
	for(i=0;i<TASK_MAX;i++){  //ͨ��LORA������ڵ㣨�����ξߣ���������  
		if(s_ucRecvFlag == 1){  //��һ�ν��ճɹ� ����ʱ2s�ٽ�����һ��
			s_ucRecvFlag = 0;
			for(k=j;k<(100+j);k++){
				OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err);
			}			
		}
		TX_LED_ON();
		SendHeartBeatToNode(fg_sTask[i].ucJigID); //������
		TX_LED_OFF();
		fg_sTask[i].ucTaskType = NULL_CMD;
		g_ucC8000_RcvFlag = 0;
		memset(g_ucC8000_RecBuf,0,g_ucC8000_RecLen);
		for(j=0;j<100;j++){ //�ȴ��ڵ���Ӧ���淢��ȥ������				  
			if(g_ucC8000_RcvFlag == 1){
				g_ucRx_D_LedOnCnt = 0; //����һ��
				CheckReceiveData(g_ucC8000_RecBuf,g_ucC8000_RecLen); //���ݽڵ�Ļ�Ӧ��ȷ������ʲôͨ�Ų���						   
				g_ucC8000_RcvFlag = 0;
				memset(g_ucC8000_RecBuf,0,g_ucC8000_RecLen);
				if(fg_sTask[i].ucTaskType == NO_ACK ){
					s_ucRecvFlag = 1;  //���ճɹ���־								
					break;
				}else if(fg_sTask[i].ucTaskType == REQUIRE_ID){ //����ID	
					s_ucRecvFlag = 1;	 //���ճɹ���־							
					if(fg_sTask[i].uiPLC_ID == 0){  
						if(ucFailIDNum > 0){ //�ж��Ƿ��з���ʧ�ܶ����յ�ID
							fg_sTask[i].uiPLC_ID = ID_BufOut();  // �ӷ���ʧ�ܵ�ID������ȡ
						}else{//û�л��յ�ID
							fg_sTask[i].uiPLC_ID = g_uiCurrentID;
				    #if ID_LIMIT_EN == 1
							if(g_uiCurrentID >= ID_END){ /* �ж�ID�Ƿ񳬳����� */
								g_ucEepromError = EEPROM_ERR_09;
							}
				    #endif
							g_uiCurrentID++;
							ucTempIDBuf[0] = (g_uiCurrentID>>24)&0xff;
							ucTempIDBuf[1] = (g_uiCurrentID>>16)&0xff;
							ucTempIDBuf[2] = (g_uiCurrentID>>8)&0xff;
							ucTempIDBuf[3] = (g_uiCurrentID>>0)&0xff;							
							if(SaveDadaToEEPROM(PLC_CURRENT_ID_ADD,ucTempIDBuf,4) == 0){ // ��ǰID�б� ������EEPROM
								g_ucEepromError = EEPROM_ERR_06;                       // д�뵱ǰID(δʹ��)����
								ucLen = LoadDataToBuffer();
								WriteDataToFlash(ucBackupDataFlash,FLASH_BACKUP_PAGE,ucLen);														
							}
							sprintf(cPrintTable,"ID:%08X",g_uiCurrentID);
							DisplayString(3,1,cPrintTable);
						}									    
					}
					OSTimeDlyHMSM(0,0,0,800,OS_OPT_TIME_HMSM_STRICT,&err);
					TX_LED_ON();
					SendID_ToNode(fg_sTask[i].ucJigID,fg_sTask[i].uiPLC_ID); //ͨ��LORA��ID������ڵ�
					TX_LED_OFF();
					break;
				}else if(fg_sTask[i].ucTaskType == CONFIRM_ID_OK ){
					//fg_sTask[i].ucTaskType = NO_ACK;
					s_ucRecvFlag = 1;
					OSTimeDlyHMSM(0,0,0,800,OS_OPT_TIME_HMSM_STRICT,&err);
					if(fg_sTask[i].uiPLC_ID != 0){ //����ID�ɹ��󣬽��ɹ������ID��������1��������EEPROM
						fg_sTask[i].uiPLC_ID = 0;
						g_uiUsedID_Num++;
						ucTempIDBuf[0] = g_uiUsedID_Num>>24&0xff;
						ucTempIDBuf[1] = (g_uiUsedID_Num>>16)&0xff;
						ucTempIDBuf[2] = (g_uiUsedID_Num>>8)&0xff;
						ucTempIDBuf[3] = (g_uiUsedID_Num>>0)&0xff;							
						SaveDadaToEEPROM(ID_USED_NUM_ADD,ucTempIDBuf,4); //������EEPROM														  
						sprintf(cPrintTable,"%6d",g_uiUsedID_Num);
						DisplayString(4,6,cPrintTable);																				
					}
					TX_LED_ON();								  
					ConfirmID(fg_sTask[i].ucJigID,0x01);
					TX_LED_OFF();
					break;	
				}else if(fg_sTask[i].ucTaskType == CONFIRM_ID_FAIL ){
					s_ucRecvFlag = 1; //����IDʧ�ܣ�����ID
					ID_BufIn(fg_sTask[i].uiPLC_ID);//����ID,���ұ��浽EEPROM																
					fg_sTask[i].uiPLC_ID = 0;								
					OSTimeDlyHMSM(0,0,0,800,OS_OPT_TIME_HMSM_STRICT,&err);
					TX_LED_ON();
					ConfirmID(fg_sTask[i].ucJigID,0x01);
					TX_LED_OFF();
					break;	
				}else if(fg_sTask[i].ucTaskType == COMPLETE ){
					s_ucRecvFlag = 1;								
					break;	
				}	
				//RX_LED_OFF();
				//RX_D_LED_OFF();							
			}
			OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err);
		}   		
	}		
}







