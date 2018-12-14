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
 
unsigned char ucNodeIDTable[NODE_NUM]={0X01,0X02,0x04};//存放治具LoraID  NODE_NUM define in LoraC8000.h
//unsigned char ucNodeIDTable[NODE_NUM]={0x04};

ScmTask fg_sTask[TASK_MAX];

//ScmResponseTask sResponseTask[TASK_MAX];//

unsigned char g_ucResponseNum = 0;

unsigned char ucTaskNum = 0;

extern unsigned int  g_uiCurrentID;             //将要使用的ID
extern unsigned int  g_uiFailID[FAIL_ID_MAX] ;  //分配未成功的ID 下次用
extern unsigned char ucFailIDNum;              //
extern unsigned int  g_uiUsedID_Num;

//static unsigned char  fg_ucBufInIx  = 0;            //buf入指针                   
//static unsigned char  fg_ucBufOutIx = 0;            //buf出指针

//static unsigned char  fg_ucTaskBufInIx  = 0;        //buf入指针                   
//static unsigned char  fg_ucTaskBufOutIx = 0;        //buf出指针

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
/* 将当前使用的ID、已经使用的ID数量、分配失败的ID数量、分配失败的ID保存到RAM缓存 */
/* 返回数组的下标，即保存一共占用的字节数量 */
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
extern unsigned int  g_uiCurrentID;             //将要使用的ID
extern unsigned int  g_uiFailID[FAIL_ID_MAX] ;  //分配未成功的ID 下次用
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
  * @param   检查EEPROM的帧格式
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
  * @param   写入EEPROM（输入目标数据 长度）
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
		if(CompareTwoString(ucTempEEPROM,ucReadEEPROM,ucLen+4))/* 比较两个字符串是否相等*/
			return 1;		
		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_HMSM_STRICT,&err);
	}		 		
	return 0;
}


/*******************************************************************************
  * @brief   DataInit
  * @param   写入EEPROM（输入目标数据 长度）
  * @retval  0 fail,1 success
*******************************************************************************/
void DataInit(void)
{
	unsigned char i,j,ucNum = 0; //
	unsigned char ucTempEEPROM[50] = {0x00};
	OS_ERR err;
	//AT24CXX_Write(0,ucTempEEPROM,50);
	for(i=0;i<TASK_MAX;i++){           
		fg_sTask[i].ucJigID = ucNodeIDTable[i]; //装载节点ID
		fg_sTask[i].uiPLC_ID = 0;
		fg_sTask[i].ucState = 0;
		fg_sTask[i].ucTimeOutCnt = TIME_OUT_MAX;
		fg_sTask[i].ucTimeOutFlag = 0;
		fg_sTask[i].ucRsponseTimeCnt = TIME_RESPONSE_MAX;
		fg_sTask[i].ucRsponseType = 0;		
	}

	for(i = 0;i < 5;i++){  //5 times
		AT24CXX_Read(ID_USED_NUM_ADD,ucTempEEPROM,8);//已分配成功ID个数 
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
	DisplayString(4,6,cPrintTable);	//显示

	for(i = 0;i < 5;i++){ 
		AT24CXX_Read(PLC_CURRENT_ID_ADD,ucTempEEPROM,8);//读取当前ID(未使用)
		if(CheckEEPROM_Frame(ucTempEEPROM,8)){
			g_uiCurrentID = ucTempEEPROM[2];
			g_uiCurrentID = (g_uiCurrentID << 8) + ucTempEEPROM[3];
			g_uiCurrentID = (g_uiCurrentID << 8) + ucTempEEPROM[4];
			g_uiCurrentID = (g_uiCurrentID << 8) + ucTempEEPROM[5];
		#if ID_LIMIT_EN == 1
			if(g_uiCurrentID < ID_START || g_uiCurrentID >= ID_END){ /* 判断ID是否超出限制 */
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

	DisplayString(1,2,"聚阳电子");
	DisplayString(1,7,"闲");
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

    //存储失败ID的总数量，若存储失败则错相应处理
	if(SaveDadaToEEPROM(PLC_FAIL_ID_NUM_ADD,&ucFailIDNum,1) == 0){//存失败ID的数量
		g_ucEepromError = EEPROM_ERR_04; //写入分配失败的ID数量有误
		i = LoadDataToBuffer();  //将当前使用的ID、已经使用的ID数量、分配失败的ID数量、分配失败的ID保存到RAM缓存
		WriteDataToFlash(ucBackupDataFlash,FLASH_BACKUP_PAGE,i);//存入FLASH
		return;
	}         

    /* 将失败的ID分解成字节存入缓存ucTempFailID */
	for(i = 0;i < ucFailIDNum;i++){    
		unID.uiID = g_uiFailID[i]; 
		for(j = 0;j < 4;j++){
			ucTempFailID[i*4+j] = unID.ucBuf[3-j];
		}
	}	
    //保存失败的ID，若操作失败则做相应处理
	if(SaveDadaToEEPROM(PLC_FAIL_ID_ADD,ucTempFailID,ucFailIDNum*4)==0){  //存ID
		g_ucEepromError = EEPROM_ERR_05; //写入分配失败的ID号有误
		i = LoadDataToBuffer(); //将当前使用的ID、已经使用的ID数量、分配失败的ID数量、分配失败的ID保存到RAM缓存
		WriteDataToFlash(ucBackupDataFlash,FLASH_BACKUP_PAGE,i);//存入FLASH
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
		AT24CXX_Read(PLC_FAIL_ID_NUM_ADD,ucTempID,5);//读取分配失败的ID 个数
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
			AT24CXX_Read(PLC_FAIL_ID_ADD,ucTempID,ucNum);////读取分配失败的ID
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
  * @param  重新刷新ID缓存 
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
  * @param  从缓存中取出一个ID,并更新EEPROM
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
				ID_BufRefresh();  //刷新缓存
				SaveFailID_ToEEPROM();
				return uiReturnVal;
			}  
		}		
	}		
	return 0;
}
/*******************************************************************************
  * @brief  ID_BufIn
  * @param  存入一个ID,并更新EEPROM
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
		g_uiFailID[ucFailIDNum++] = code; //将失败的ID装入失败ID保存数组，并且失败ID总数量加1
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
	//static unsigned char s_ucSentIntervalCnt = 3; //200*3 = 600  发送间隔

	ScmTask          sTempTask;
	ScmResponseTask  sTempReTask;

	for(i = 0;i < TASK_MAX;i++){  //巡查所有的Task，判断有没有超时需要回收的ID
		if(fg_sTask[i].ucJigID 	!= 0){
			if(fg_sTask[i].ucTimeOutCnt < TIME_OUT_MAX){ //150*200 = 30s
				fg_sTask[i].ucTimeOutCnt++;
				ucBusyTempFlag = 1;    //该标志用于显示网关正忙！显示不要关机
				if(fg_sTask[i].ucTimeOutCnt >= TIME_OUT_MAX){
					ID_BufIn(fg_sTask[i].uiPLC_ID);          //回收ID,并且保存到EEPROM
					fg_sTask[i].uiPLC_ID = 0;
					break;
				}
			}	
		}
	}
    //显示忙或者闲
	if(ucBusyTempFlag > 0){  // 忙
		if(g_ucBusyFlag == 0){
			g_ucBusyFlag = 1;
			DisplayString(1,2,"请勿关机");
			DisplayString(1,7,"忙");
		}
	}else{                  // 闲
		if(g_ucBusyFlag == 1){
			g_ucBusyFlag = 0;
			DisplayString(1,2,"聚阳电子");
			DisplayString(1,7,"闲");
		}	
	}	
}


/*****************************************************
*任务刷新 重新排序
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
*去除掉某个任务,输入治具ID
*******************/
void TaskBufferDelete(unsigned char ucJigID)
{
	unsigned char i;
	for(i = 0;i < ucTaskNum;i++){
		if(fg_sTask[i].ucJigID == ucJigID){ // 任务存在
			fg_sTask[i].ucJigID = 0;
			fg_sTask[i].uiPLC_ID = 0;
			fg_sTask[i].ucState = 0;
			fg_sTask[i].ucTimeOutCnt = TIME_OUT_MAX;
			fg_sTask[i].ucTimeOutFlag = 0;
			fg_sTask[i].ucRsponseTimeCnt = TIME_RESPONSE_MAX;
			fg_sTask[i].ucRsponseType = 0;
		}
	}
	TaskBufferRefresh(); //刷新任务
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
*任务处理
*******************/
/*
void TaskBufferDeal(ScmTask sNewTask)
{
    unsigned char i;
	  unsigned int uiTempID = 0;
	  unsigned char ucTempIDBuf[4] = {0x00};
		OS_ERR err;
    switch(sNewTask.ucState){
			case CONCEL:                                       // 取消任务ID被收回再利用
					for(i = 0;i < ucTaskNum;i++){
						if(fg_sTask[i].ucJigID == sNewTask.ucJigID){ // 如果缓存中有则清除
							  fg_sTask[i].ucJigID = 0;
							  fg_sTask[i].ucState = 0;
                uiTempID = fg_sTask[i].uiPLC_ID;			
							  ID_BufIn(uiTempID);     // ID回收							
							  fg_sTask[i].uiPLC_ID = 0;
							  TaskBufferRefresh();   // 刷新任务
							  g_ucBeepOnCnt = 0;
						} 		
					}	
          AddToReponseTask(sNewTask.ucJigID,0x01);					
				break;
			case REQUIRE:          // 节点请求分配ID
					for(i = 0;i < ucTaskNum;i++){
						  if(fg_sTask[i].ucJigID == sNewTask.ucJigID){ // 如果缓存中有则 ID再用
							    fg_sTask[i].ucState = sNewTask.ucState;
							    fg_sTask[i].ucTimeOutCnt = 0;
							//uiTempID = fg_sTask[i].uiPLC_ID ;
							//fg_sTask[i].ucTimeOutCnt = 0;
						      fg_sTask[i].ucRsponseType = 0x01;//分配ID
						      fg_sTask[i].ucRsponseTimeCnt = 0;
						  //fg_sTask[i].uiPLC_ID = uiTempID;
							    break; // 跳出for 
						  } 		
					}
					if(i >= ucTaskNum){// 缓存没
						if(ucTaskNum < TASK_MAX){
						    fg_sTask[ucTaskNum].ucJigID = sNewTask.ucJigID;
							  fg_sTask[ucTaskNum].ucState = sNewTask.ucState;
//							  fg_sTask[i].ucTimeOutCnt = 0;
//							  fg_sTask[i].ucRsponseType = 0x01;//分配ID
//							  fg_sTask[i].ucRsponseTimeCnt = 0;
						}else{
						    break;
						}
						
					  if(ucFailIDNum > 0){ 
						    uiTempID = ID_BufOut(); // 从分配失败的ID缓存中取
						}else{
						    uiTempID = g_uiCurrentID;
							  g_uiCurrentID++;
							  ucTempIDBuf[0] = g_uiCurrentID>>24&0xff;
							  ucTempIDBuf[1] = (g_uiCurrentID>>16)&0xff;
							  ucTempIDBuf[2] = (g_uiCurrentID>>8)&0xff;
							  ucTempIDBuf[3] = (g_uiCurrentID>>0)&0xff;							
							  SaveDadaToEEPROM(PLC_CURRENT_ID_ADD,ucTempIDBuf,4); //当前ID有变 保存入EEPROM
							 // GC9200_SendString(unsigned char *pString,unsigned char ucLen);
							  sprintf(cPrintTable,"ID:%08X",g_uiCurrentID);
	              DisplayString(3,1,cPrintTable);
						}
						fg_sTask[ucTaskNum].ucTimeOutCnt = 0;
						fg_sTask[ucTaskNum].ucRsponseType = 0x01;//分配ID
						fg_sTask[ucTaskNum].ucRsponseTimeCnt = 0;
						fg_sTask[ucTaskNum++].uiPLC_ID = uiTempID;						
					}
					g_ucBeepOnCnt = 0;
					//OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //bug 	******
					//SendID_ToNode(sNewTask.ucJigID,uiTempID);					
				  break;
			case CONFIRM:         // ID分配成功			
				for(i = 0;i < ucTaskNum;i++){
	          if(fg_sTask[i].ucJigID == sNewTask.ucJigID){ // 任务存在
		            g_uiUsedID_Num++;
							  ucTempIDBuf[0] = g_uiUsedID_Num>>24&0xff;
							  ucTempIDBuf[1] = (g_uiUsedID_Num>>16)&0xff;
							  ucTempIDBuf[2] = (g_uiUsedID_Num>>8)&0xff;
							  ucTempIDBuf[3] = (g_uiUsedID_Num>>0)&0xff;							
							  SaveDadaToEEPROM(ID_USED_NUM_ADD,ucTempIDBuf,4); //当前ID有变 保存入EEPROM							
							  //AT24CXX_Read(ID_USED_NUM_ADD,ucTempEEPROM,8);
							  
							  sprintf(cPrintTable,"%6d",g_uiUsedID_Num);
							  DisplayString(4,6,cPrintTable);
							
							  //OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //bug 	****
							  //ConfirmID(sNewTask.ucJigID,0x01);
								fg_sTask[i].ucRsponseType = 0x02;//确认分配ID
						    fg_sTask[i].ucRsponseTimeCnt = 0;
							  break;
		        }
	      }
				if(i >= ucTaskNum){   //任务不存在
//				    fg_sTask[ucTaskNum].ucJigID = sNewTask.ucJigID;
//					  fg_sTask[ucTaskNum].uiPLC_ID = 0;
//					  fg_sTask[ucTaskNum].ucRsponseType = 0x02;              //确认分配ID					
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
****返回命令码，MCU查询C8000模块信息
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
**0失败1成功，MCU查询C8000注册的节点信息
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
** C8000发送数据, pucInputData为透传的数据
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
** 发送心跳
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
** 分配ID给治具
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
** 回复确认分配ID给治具 ucVal 0 fail, 1 success
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
** 添加任务
************************/
void AddToTask(ScmTask sNewTask)
{
	unsigned char i;            //sResponseTask[TASK_MAX]
	for (i=0;i<TASK_MAX;i++){
		if (fg_sTask[i].ucJigID == sNewTask.ucJigID ){
			//fg_sTask[i].ucState = ON_LINE;                //在线
			fg_sTask[i].ucTaskType = sNewTask.ucTaskType;						
			fg_sTask[i].ucTimeOutCnt = sNewTask.ucTimeOutCnt;
		}
	}
}

/***************************************************************************
*检查收到节点的数据
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
	if ((ucLen+5) != ucDataLen){ // 长度
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
	if (0x00 == pucInputData[5]){ //ID不能为0
		return ;
	}
	ucCtrl = pucInputData[1];
	ucCMD = pucInputData[3];		   			
	sTempTask.ucJigID = pucInputData[5];	
	if (ucCtrl == 0xA2){
			//RX_LED_ON();
		g_ucRxLedOnCnt = 0;			
		switch(ucCMD){
			case 0xF1://F1 请求ID前的心跳
				sTempTask.ucTaskType = NO_ACK;
				sTempTask.ucTimeOutCnt = TIME_OUT_MAX;
				break;
			case 0xF2://F2 请求ID时的心跳
				sTempTask.ucTaskType = REQUIRE_ID ;
				sTempTask.ucTimeOutCnt = 0;
				break;
			case 0xF3://F3 请求ID后的心跳
				sTempTask.ucTaskType = NO_ACK;
				sTempTask.ucTimeOutCnt = 0;
				break;
			case 0xF4://F4 确认ID成功的心跳
				sTempTask.ucTaskType = CONFIRM_ID_OK;
				sTempTask.ucTimeOutCnt = TIME_OUT_MAX;
				sTempTask.uiPLC_ID = 0;
				break;
			case 0xF5://F5 确认ID失败的心跳
				sTempTask.ucTaskType = CONFIRM_ID_FAIL;	
				sTempTask.ucTimeOutCnt = TIME_OUT_MAX;
				break;
			case 0xF6://F6 确认ID后的心跳
				sTempTask.ucTaskType = COMPLETE;
				sTempTask.ucTimeOutCnt = TIME_OUT_MAX;
				break;
			default:
				sTempTask.ucJigID = 0; //ID置0 则不添加至任务
				break;					
		}				
		AddToTask(sTempTask); //添加到节点任务数组里去，以便下面的程序访问
				//g_ucBeepOnCnt = 0;  //数据帧正确  蜂鸣器响一下
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
	
	for(i=0;i<TASK_MAX;i++){  //通过LORA轮流向节点（测试治具）发送心跳  
		if(s_ucRecvFlag == 1){  //上一次接收成功 则延时2s再进行下一次
			s_ucRecvFlag = 0;
			for(k=j;k<(100+j);k++){
				OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err);
			}			
		}
		TX_LED_ON();
		SendHeartBeatToNode(fg_sTask[i].ucJigID); //发心跳
		TX_LED_OFF();
		fg_sTask[i].ucTaskType = NULL_CMD;
		g_ucC8000_RcvFlag = 0;
		memset(g_ucC8000_RecBuf,0,g_ucC8000_RecLen);
		for(j=0;j<100;j++){ //等待节点响应上面发出去的心跳				  
			if(g_ucC8000_RcvFlag == 1){
				g_ucRx_D_LedOnCnt = 0; //灯闪一下
				CheckReceiveData(g_ucC8000_RecBuf,g_ucC8000_RecLen); //根据节点的回应，确定处于什么通信步骤						   
				g_ucC8000_RcvFlag = 0;
				memset(g_ucC8000_RecBuf,0,g_ucC8000_RecLen);
				if(fg_sTask[i].ucTaskType == NO_ACK ){
					s_ucRecvFlag = 1;  //接收成功标志								
					break;
				}else if(fg_sTask[i].ucTaskType == REQUIRE_ID){ //请求ID	
					s_ucRecvFlag = 1;	 //接收成功标志							
					if(fg_sTask[i].uiPLC_ID == 0){  
						if(ucFailIDNum > 0){ //判断是否有分配失败而回收的ID
							fg_sTask[i].uiPLC_ID = ID_BufOut();  // 从分配失败的ID缓存中取
						}else{//没有回收的ID
							fg_sTask[i].uiPLC_ID = g_uiCurrentID;
				    #if ID_LIMIT_EN == 1
							if(g_uiCurrentID >= ID_END){ /* 判断ID是否超出限制 */
								g_ucEepromError = EEPROM_ERR_09;
							}
				    #endif
							g_uiCurrentID++;
							ucTempIDBuf[0] = (g_uiCurrentID>>24)&0xff;
							ucTempIDBuf[1] = (g_uiCurrentID>>16)&0xff;
							ucTempIDBuf[2] = (g_uiCurrentID>>8)&0xff;
							ucTempIDBuf[3] = (g_uiCurrentID>>0)&0xff;							
							if(SaveDadaToEEPROM(PLC_CURRENT_ID_ADD,ucTempIDBuf,4) == 0){ // 当前ID有变 保存入EEPROM
								g_ucEepromError = EEPROM_ERR_06;                       // 写入当前ID(未使用)有误
								ucLen = LoadDataToBuffer();
								WriteDataToFlash(ucBackupDataFlash,FLASH_BACKUP_PAGE,ucLen);														
							}
							sprintf(cPrintTable,"ID:%08X",g_uiCurrentID);
							DisplayString(3,1,cPrintTable);
						}									    
					}
					OSTimeDlyHMSM(0,0,0,800,OS_OPT_TIME_HMSM_STRICT,&err);
					TX_LED_ON();
					SendID_ToNode(fg_sTask[i].ucJigID,fg_sTask[i].uiPLC_ID); //通过LORA将ID分配给节点
					TX_LED_OFF();
					break;
				}else if(fg_sTask[i].ucTaskType == CONFIRM_ID_OK ){
					//fg_sTask[i].ucTaskType = NO_ACK;
					s_ucRecvFlag = 1;
					OSTimeDlyHMSM(0,0,0,800,OS_OPT_TIME_HMSM_STRICT,&err);
					if(fg_sTask[i].uiPLC_ID != 0){ //分配ID成功后，将成功分配的ID总数量加1，并存入EEPROM
						fg_sTask[i].uiPLC_ID = 0;
						g_uiUsedID_Num++;
						ucTempIDBuf[0] = g_uiUsedID_Num>>24&0xff;
						ucTempIDBuf[1] = (g_uiUsedID_Num>>16)&0xff;
						ucTempIDBuf[2] = (g_uiUsedID_Num>>8)&0xff;
						ucTempIDBuf[3] = (g_uiUsedID_Num>>0)&0xff;							
						SaveDadaToEEPROM(ID_USED_NUM_ADD,ucTempIDBuf,4); //保存入EEPROM														  
						sprintf(cPrintTable,"%6d",g_uiUsedID_Num);
						DisplayString(4,6,cPrintTable);																				
					}
					TX_LED_ON();								  
					ConfirmID(fg_sTask[i].ucJigID,0x01);
					TX_LED_OFF();
					break;	
				}else if(fg_sTask[i].ucTaskType == CONFIRM_ID_FAIL ){
					s_ucRecvFlag = 1; //分配ID失败，回收ID
					ID_BufIn(fg_sTask[i].uiPLC_ID);//回收ID,并且保存到EEPROM																
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







