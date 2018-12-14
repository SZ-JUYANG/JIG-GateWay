#include "stm32f10x.h"
#include "main.h"
#include "MENU.h"
#include "LCD12864B.h"
#include "System_Initialize.h"
#include "KeyDriver.h"
#include "WordLib.h"
#include "string.h" 
#include "STM32_UART.h"
#include "IT8512B.h"
#include "GP9034T.h"
#include "AT24Cxx.h"
#include "delay.h"
#include "TaskApp.h"
#include "includes.h"
/*****************任务缓存*******************/
#define MUNE_TASK_SIZE 2  //任务最大个数（行）
#define MUNE_BUF_SIZE  2  //任务最大宽度（列）

//unsigned char g_ucMenuTaskRequireBuf[MUNE_TASK_SIZE][MUNE_BUF_SIZE];
//unsigned char g_ucMenuTaskAckBuf[MUNE_TASK_SIZE][MUNE_BUF_SIZE];

//static unsigned char  fg_ucMenuTaskRequireBufInIndex  = 0;  //请求buf输入指引
//static unsigned char  fg_ucMenuTaskRequireBufOutIndex = 0;  //请求buf输出指引
//static unsigned char  fg_ucMenuTaskRequireNum  = 0;         //请求buf中任务个数

//static unsigned char  fg_ucMenuTaskAckBufInIndex = 0;       //响应buf输出指引
//static unsigned char  fg_ucMenuTaskAckBufOutIndex = 0;      //响应buf输出指引
//static unsigned char  fg_ucMenuTaskAckNum  = 0;             //响应buf中任务个数
//static unsigned char  fg_ucTempBuf[MUNE_BUF_SIZE];          //任务缓存

//static unsigned char g_ucYear;
//static unsigned char g_ucMonth;
//static unsigned char g_ucDate;
//static unsigned char g_ucHour;
//static unsigned char g_ucMinute;

extern unsigned char g_ucBeepOnCnt;
extern unsigned char g_ucEnFlag;


static unsigned short int fg_uiKeyFuncIndex = 0; //
static unsigned char fg_ucKeyDir = 0x00;         //记录按键方向

extern unsigned char g_ucEnableFlag;
extern unsigned char g_ucBeepOnCnt;
extern ScmAT24CxxTypeDef    ScmAT24CxxIdData; //存储的ID 设备码……
extern ScmAT24CxxErrTypeDef ScmAT24CxxErr; //误差系数
extern ScmCorrectionTypeDef  DPS_WritePara;

extern ScmJIGTypeDef    ScmJIG;      // 存储 治具

//ScmAT24CxxTypeDef    ScmMenuIdData; //存储的ID 设备码……
ScmAT24CxxErrTypeDef ScmMenuErr;    //误差系数
char cPrintBu[20] = {0x00};
/*******************************************************************************
* Function Name : MenuTaskRequireBufOut
* Description   : 从缓存中取出一组任务讲求
*******************************************************************************/
//unsigned char* MenuTaskRequireBufOut(void)
//{
//	unsigned char ucCnt;	
//  if(fg_ucMenuTaskRequireNum > 0){             //查看是否有任务在buf中                                             
//    fg_ucMenuTaskRequireNum--;                 //-1表示取走一个值 
//		for(ucCnt=0;ucCnt<MUNE_BUF_SIZE;ucCnt++){
//		  fg_ucTempBuf[ucCnt] = g_ucMenuTaskRequireBuf[fg_ucMenuTaskRequireBufOutIndex][ucCnt];
//		}		
//    fg_ucMenuTaskRequireBufOutIndex++;
//    if(fg_ucMenuTaskRequireBufOutIndex >= MUNE_TASK_SIZE){    //buf满则指针循环回到起点               
//      fg_ucMenuTaskRequireBufOutIndex = 0;
//    }                                               
//		//return g_ucMenuTaskRequireBuf[fg_ucMenuTaskRequireBufOutIndex]; //返回值
//		return fg_ucTempBuf;
//  }else{                      
//    return 0;                           //没有,则返回0
//  }
//}
/*******************************************************************************
* Function Name : MenuTaskAckBufOut
* Description   : 清空响应
*******************************************************************************/
//void MenuTaskRequireBufClear(void)
//{
//  while(fg_ucMenuTaskRequireNum){
//	  MenuTaskRequireBufOut();
//	}
//}
/*******************************************************************************
* Function Name : MenuTaskRequireBufIn
* Description   : 把一组任务请求放入缓存中
*******************************************************************************/
//void MenuTaskRequireBufIn (unsigned char *pucTempBuf)
//{                        
//    unsigned char ucCnt;
//	  if(fg_ucMenuTaskRequireNum >= MUNE_TASK_SIZE){   //buf满则放弃最早的一个值                   
//      MenuTaskRequireBufOut();                                                 
//    }                  
//    fg_ucMenuTaskRequireNum++;
//		for(ucCnt=0;ucCnt<MUNE_BUF_SIZE;ucCnt++){
//		  g_ucMenuTaskRequireBuf[fg_ucMenuTaskRequireBufInIndex][ucCnt] = *pucTempBuf;
//		  pucTempBuf++;
//		}
//		fg_ucMenuTaskRequireBufInIndex++;
//    if(fg_ucMenuTaskRequireBufInIndex >= MUNE_TASK_SIZE){     //buf满则指针循环回到起点           
//      fg_ucMenuTaskRequireBufInIndex = 0;
//    }                      
//}
/*******************************************************************************
* Function Name : MenuTaskRequireBufIn
* Description   : 检查是否有请求任务存于缓存中
*******************************************************************************/
//unsigned char MenuTaskRequireFlag(void)
//{
//  if(fg_ucMenuTaskRequireNum>0){  //表示有任务
//	  return 0x01;
//	}else{
//	  return 0x00;
//	}
//}
/*******************************************************************************
* Function Name : MenuTaskAckBufOut
* Description   : 从缓存中取出一组响应
*******************************************************************************/
//unsigned char* MenuTaskAckBufOut(void)
//{
//	unsigned char ucCnt;
//  if(fg_ucMenuTaskAckNum > 0){      //查看是否有任务在buf中                                             
//    fg_ucMenuTaskAckNum--;          //-1表示取走一个值
//		for(ucCnt = 0;ucCnt < MUNE_BUF_SIZE;ucCnt++){
//		  fg_ucTempBuf[ucCnt] = g_ucMenuTaskAckBuf[fg_ucMenuTaskAckBufOutIndex][ucCnt];
//		}		
//    fg_ucMenuTaskAckBufOutIndex++;
//    if(fg_ucMenuTaskAckBufOutIndex >= MUNE_TASK_SIZE){          //buf满则指针循环回到起点               
//      fg_ucMenuTaskAckBufOutIndex = 0;
//    }                                               
//		//return g_ucMenuTaskAckBuf[fg_ucMenuTaskAckBufOutIndex]; //返回值
//		return fg_ucTempBuf;
//  }else{                      
//    return 0;                           //没有,则返回0
//  }
//}
/*******************************************************************************
* Function Name : MenuTaskAckBufOut
* Description   : 清空响应
*******************************************************************************/
//void MenuTaskAckBufClear(void)
//{
//  while(fg_ucMenuTaskAckNum){
//	  MenuTaskAckBufOut();
//	}
//}

/*******************************************************************************
* Function Name : HomePage
* Description   : 把一组响应放入缓存中
*******************************************************************************/
//void MenuTaskAckBufIn (unsigned char *pucTempBuf)
//{                        
//    unsigned char ucCnt;
//	  if(fg_ucMenuTaskAckNum >= MUNE_TASK_SIZE){            //buf满则放弃最早的一个值                   
//      MenuTaskAckBufOut();                                                 
//    }                  
//    fg_ucMenuTaskAckNum++;
//		for(ucCnt = 0;ucCnt < MUNE_BUF_SIZE;ucCnt++){
//		  g_ucMenuTaskAckBuf[fg_ucMenuTaskAckBufInIndex][ucCnt] = *pucTempBuf;
//		  pucTempBuf++;
//		}	
//		fg_ucMenuTaskAckBufInIndex++;				
//    if(fg_ucMenuTaskAckBufInIndex >= MUNE_TASK_SIZE){     //buf满则指针循环回到起点           
//      fg_ucMenuTaskAckBufInIndex = 0;
//    }                      
//}
/*******************************************************************************
* Function Name : MenuTaskAckBufIn
* Description   : 检查是否有响应存于缓存中
*******************************************************************************/
//unsigned char MenuTaskAckFlag(void)
//{
//  if(fg_ucMenuTaskAckNum>0){  //表示有任务
//	  return 0x01;
//	}else{
//	  return 0x00;
//	}
//}
//void WriteDataToFlash(uint32_t ulIputData)
//{
//	
//  FLASH_Unlock();
//  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
//  if (FLASH_ErasePage(PARAM_ADD)!= FLASH_COMPLETE){
//     /* Error occurred while sector erase. 
//         User can add here some code to deal with this error  */
//    while (1);
//  }	
//  if(FLASH_ProgramWord(PARAM_ADD, ulIputData) != FLASH_COMPLETE){
//	  while (1);  	
//	}
//  FLASH_Lock(); 		
//}

#define PIONT_ICO  0x10 //向右三角
#define SELECT_ICO 0x02 //笑脸
#define UP_ICO 0x1E //向上三角

unsigned char Table1[1]="1";
unsigned char Table2[1]="2";
unsigned char Table3[1]="3";
unsigned char Table4[1]="4";
/*******************************************************************************
* Function Name : HomePage
* Description   : 显示首页
*******************************************************************************/
void HomePage(void)
{
	//Lcd12864_clear();
  FillSpace();	
	DisplayString(1,2,"聚阳电子科技");
	DisplayString(2,3,"LED-JIG");
	DisplayString(3,2,"设置");
	DisplayString(4,2,"请按下一步");		
}
/*******************************************************************************
* Function Name : HomePageVoid
* Description   : 
*******************************************************************************/
void HomePageVoid(void)
{

}
/*******************************************************************************
* Function Name : Menu1
* Description   : 
*******************************************************************************/
void Menu1(void)//进入首页
{
  //Lcd12864_clear();
	FillSpace();
	LCD_delay_ms(50);
	DisplayString(1,2,"聚阳电子科技");
	DisplayString(2,2,"操作设置");
	DisplayString(3,2,"设备码日期设置");
	DisplayString(4,2,"误差系数设置");
	
	DisplayChar(3,1,SELECT_ICO);
	//DisplayChar(4,1,' ');	
}
/*******************************************************************************
* Function Name : Menu2
* Description   : 
*******************************************************************************/
void Menu2(void)
{
	DisplayChar(3,1,SELECT_ICO); //这设备码和日期
	DisplayChar(4,1,' ');
  DisplayChar(2,1,' ');	
}

void Menu3(void)
{
  DisplayChar(2,1,' ');
	DisplayChar(3,1,' ');
	DisplayChar(4,1,SELECT_ICO); //误差设置
	
	ScmMenuErr.fInputVol = ScmAT24CxxErr.fInputVol;
	ScmMenuErr.fInputCur = ScmAT24CxxErr.fInputCur;
	ScmMenuErr.fInputPow = ScmAT24CxxErr.fInputPow;
	
	ScmMenuErr.fOutputVol = ScmAT24CxxErr.fOutputVol;
	ScmMenuErr.fOutputCur = ScmAT24CxxErr.fOutputCur;
	ScmMenuErr.fOutputPow = ScmAT24CxxErr.fOutputPow;	
}

void Menu4(void)
{
	DisplayChar(3,1,' ');
	DisplayChar(2,1,SELECT_ICO); //操作模式设置
	DisplayChar(4,1,' ');
}
void Menu4_1(void)
{
  FillSpace();
  DisplayString(1,2,"恢复出厂设置");
	DisplayString(2,2,"测试模式设置");
	DisplayString(3,2,"治具信息查询");	
	DisplayString(4,2,"查询和打印ID");
	DisplayChar(1,1,SELECT_ICO); 
}
void Menu4_2(void) //出厂设置
{
  DisplayChar(1,1,SELECT_ICO);
	DisplayChar(2,1,' '); 
	DisplayChar(4,1,' ');
}
void Menu4_3(void) //测试模式设置
{
  DisplayChar(1,1,' ');
	DisplayChar(2,1,SELECT_ICO); 
	DisplayChar(3,1,' ');
}
void Menu4_4(void) //治具信息查询
{
  DisplayChar(4,1,' ');
	DisplayChar(2,1,' '); 
	DisplayChar(3,1,SELECT_ICO);
}

void Menu4_5(void) //查询和打印ID
{
  DisplayChar(1,1,' ');
	DisplayChar(3,1,' '); 
	DisplayChar(4,1,SELECT_ICO);
}


void Menu4_2_1(void)//恢复出厂设置
{
	FillSpace();
	DisplayString(1,1,"组网中请等待");
	if(GC9200GetNetworkNum()){ //
		DisplayString(1,1,"恢复出厂设置");
		if(GC9200GetNetworkID(&DPS_WritePara)){
		  if(SetFactory(DPS_WritePara.uiID)){
			  DisplayString(1,7,"成功");
			}else{
			  DisplayString(1,7,"失败");
			}						
		}else{
		  DisplayString(1,1,"组网失败    ");
		}
		
	}else{
	  DisplayString(1,1,"组网失败    ");
	}
	KeyFlush();
}



void Menu4_3_1(void) //测试模式设置
{
  unsigned char ucTemp[2] = {0};
	FillSpace();
	ucTemp[0] = MODE_1;
  DisplayString(1,1,"当前模式: ");
  if(ScmJIG.ucTestMode == MODE_1){
	  DisplayString(1,6,"校正");
	}else{
	  DisplayString(1,6,"出厂");
	}
	
	DisplayString(2,2,"校正测试模式");
  DisplayString(3,2,"出厂测试模式");
		
	DisplayChar(2,1,SELECT_ICO);	
	DisplayChar(3,1,' '); 
}

void Menu4_3_2(void) //校正模式设置
{	
	DisplayChar(2,1,SELECT_ICO);	
	DisplayChar(3,1,' '); 
}

void Menu4_3_3(void) //出厂模式设置
{

	DisplayChar(2,1,' '); 
	DisplayChar(3,1,SELECT_ICO);	
}

void Menu4_3_2_1(void) //校正模式设置
{	
  unsigned char ucTemp = 0;
	FillSpace();
	DisplayString(3,1,"设置校正模式");
	if(ScmJIG.ucTestMode != MODE_1){ //
	  AT24CXX_WriteOneByte(MODE_ADD,MODE_1);
		LCD_delay_ms(5);	
		ucTemp = AT24CXX_ReadOneByte(MODE_ADD);
		if(ucTemp == MODE_1){
			ScmJIG.ucTestMode = MODE_1;
		  DisplayString(3,7,"成功");
		}else{
		  DisplayString(3,7,"失败");
		}
	}else{
	  DisplayString(3,7,"成功");
	}
}
void Menu4_3_3_1(void) //校正模式设置
{	
  unsigned char ucTemp = 0;
	FillSpace();
	DisplayString(3,1,"设置出厂模式");
	if(ScmJIG.ucTestMode != MODE_2){ //
	  AT24CXX_WriteOneByte(MODE_ADD,MODE_2);
		LCD_delay_ms(5);	
		ucTemp = AT24CXX_ReadOneByte(MODE_ADD);
		if(ucTemp == MODE_2){
			ScmJIG.ucTestMode = MODE_2;
		  DisplayString(3,7,"成功");
		}else{
		  DisplayString(3,7,"失败");
		}
	}else{
	  DisplayString(3,7,"成功");
	}
}

void Menu4_4_1(void)
{
 unsigned char ucTemp[2] = {0};
	FillSpace();
	ucTemp[0] = MODE_2;
  DisplayString(1,1,"ID段");
  DisplayString(2,1,"起始ID: ");
  DisplayString(3,1,"当前ID: ");
  DisplayString(4,1,"剩余数量");
 
  sprintf(cPrintBu,":%05d",ScmJIG.uiSegmentID);
	DisplayString(1,3,cPrintBu);
 
  sprintf(cPrintBu,"%08X",ScmJIG.uiStartID);
	DisplayString(2,5,cPrintBu);

  sprintf(cPrintBu,"%08X",ScmJIG.uiCurentID);
	DisplayString(3,5,cPrintBu); 
 
  sprintf(cPrintBu,":%4d",ScmJIG.uiRestID);
	DisplayString(4,5,cPrintBu); 
}

extern ScmDPS_PNTypeDef ScmDPS_Para;
extern ScmJIGTypeDef    ScmJIG; 
void Menu4_5_1(void)//inquire and print ID
{
 char cPrintTable[16] ={0x00};
 ScmDPSInfoTypeDef ScmDPSInfo;
 unsigned char ucWait = 1;
 OS_ERR err;
	FillSpace();
  ScmDPSInfo.ucDeviceCode = ScmDPS_Para.ucDeviceCode;
	//GC9200GetNetworkNum(void)
	//SetFactory(unsigned int uiID)
	DisplayString(1,1,"组网中请等待");
	if(GC9200GetNetworkNum()){ //				
		//DisplayString(1,1,"打印电源ID成功");
		if(GC9200GetNetworkID(&DPS_WritePara)){	
			DisplayString(1,1,"组网成功    ");		
			sprintf(cPrintTable,"ID:%08X",DPS_WritePara.uiID);//ReadDPSInfo(unsigned int uiID,ScmDPSInfoTypeDef *ScmDPSInfo);
			DisplayString(4,1,cPrintTable);
			if(ReadDPSInfo(DPS_WritePara.uiID,&ScmDPSInfo)){
			  sprintf(cPrintTable,"S:0x%04X H:0x%02X",ScmDPSInfo.uiSotfVer,ScmDPSInfo.ucHardVer);
				DisplayString(2,1,cPrintTable);
			  if(ScmDPSInfo.ucActiveState==0x38){
				  DisplayString(3,1,"DPS 已校正");
				}else if(ScmDPSInfo.ucActiveState==0x00){
				  DisplayString(3,1,"DPS 未校正");
				}
			  sprintf(cPrintTable,"0x%02X",ScmDPSInfo.ucDeviceCode);//ReadDPSInfo(unsigned int uiID,ScmDPSInfoTypeDef *ScmDPSInfo);			
			  DisplayString(4,7,cPrintTable);				
			}		
			DisplayString(1,1,"打印按Enter ");
			KeyFlush();
				while(ucWait){
					KeyScan();
					  if(KeyHit() > 0){
	            switch(KeyGet()){   // get key value		  
				       case ENTER:	
								 DisplayString(1,1,"打印电源ID成功");
								 //GP9034T_PrintData(DPS_WritePara.uiID,ScmAT24CxxIdData.ucDeviceCode);
							  // GP9034T_PrintData(DPS_WritePara.uiID,ScmDPSInfo.ucDeviceCode);
							   GP9034T_PrintData(DPS_WritePara.uiID,ScmDPSInfo.ucDeviceCode,ScmJIG.ucYear,ScmJIG.ucmonth);
								// DisplayString(4,1,"           ");
							   ucWait  = 0;
								 break;
							 case ESC:
							  // DisplayString(4,1,"           ");
							   return;
                 break;								 
							 }
						}
					  OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); // 200ms	
				}					
			
		}else{
		  DisplayString(1,1,"组网失败    ");
		}
		
	}else{
	  DisplayString(1,1,"组网失败    ");
	}
	KeyFlush();
}
/*******************************************************************************
* Function Name : Menu1_1
* Description   : 查询菜单内容 page 1
*******************************************************************************/
//static unsigned char ucIDFlag = 0;
//static u32 uiTempID = 0;
//static u16 uiTempNum = 0;       //ID总数

static u8  ucTempYear = 0;  //年
static u8  ucTempMonth = 0; //月
static u8  ucTempDeviceCode = 0;//设备码

#define START_ID   0
#define CURRENT_ID 1

void Menu2_1(void) //进入设置菜单
{
  FillSpace();	
	DisplayString(1,1,"年份");	
	DisplayString(1,5,"月份");
	DisplayString(3,1,"设备码0x");
	DisplayString(4,2,"保存");

	sprintf(cPrintBu,"%02d",ScmJIG.ucYear);
	DisplayString(1,3,cPrintBu);
	
	sprintf(cPrintBu,"%02d",ScmJIG.ucmonth);
	DisplayString(1,7,cPrintBu);
	
	sprintf(cPrintBu,"%02X",ScmJIG.ucDeviceCode);
	DisplayString(3,5,cPrintBu);
	
	ucTempYear = ScmJIG.ucYear;
	ucTempMonth = ScmJIG.ucmonth;
	ucTempDeviceCode = ScmJIG.ucDeviceCode;
	
	DisplayString(4,5,"^ ");//设备码H	
	//DisplayChar(1,1,SELECT_ICO); //起始ID
}

void Menu2_2(void) 
{
  DisplayString(2,7,"  ");//清月份
  DisplayString(4,5,"^ ");//设备码H	
}

void Menu2_3(void) 
{
  //DisplayString(1,7,"  ");//清月份
  DisplayString(4,5," ^");//设备码L	
}
void Menu2_4(void) 
{
  //DisplayString(1,7,"  ");//清月份
  DisplayString(4,5,"  ");  //清设备码L	
	DisplayChar(4,1,SELECT_ICO); //保存
}

void Menu2_5(void) 
{
  DisplayString(2,3," ^");//年份
  DisplayString(4,1,"  ");//清保存
}

void Menu2_6(void) 
{
  DisplayString(2,3,"  ");//清年份
  DisplayString(2,7," ^");//月份
}


void Menu2_2_1(void) //设备码H 
{
 if(fg_ucKeyDir == UP){      
		if((ucTempDeviceCode&0xf0) == 0xf0 ){
		  ucTempDeviceCode &= 0x0f;
		}else{
		  ucTempDeviceCode += 0x10;
		} 
	}else if(fg_ucKeyDir == DOWN){
		if((ucTempDeviceCode&0xf0) == 0x00 ){
		  ucTempDeviceCode |= 0xf0;
		}else{
		  ucTempDeviceCode -= 0x10;
		}
	}
	sprintf(cPrintBu,"%02X",ucTempDeviceCode);
	DisplayString(3,5,cPrintBu);		
}
void Menu2_3_1(void) //设备码L
{
  if(fg_ucKeyDir == UP){      
		if((ucTempDeviceCode&0x0f) == 0x0f ){
		  ucTempDeviceCode &= 0xf0;
		}else{
		  ucTempDeviceCode += 0x01;
		} 
	}else if(fg_ucKeyDir == DOWN){
		if((ucTempDeviceCode&0x0f) == 0x00 ){
		  ucTempDeviceCode |= 0x0f;
		}else{
		  ucTempDeviceCode -= 0x01;
		}
	}
	sprintf(cPrintBu,"%02X",ucTempDeviceCode);
	DisplayString(3,5,cPrintBu);	
	//DisplayString(3,2," ^");
}
	
	
	
void Menu2_4_1(void) //进入保存
{
  unsigned char ucTemp = 0;
  FillSpace();
	if(ucTempYear != ScmJIG.ucYear){  // 年已修改
		AT24CXX_WriteOneByte(YEAR_ADD,ucTempYear);
		LCD_delay_ms(5);	
		ucTemp = AT24CXX_ReadOneByte(YEAR_ADD);
		if(ucTemp == ucTempYear){      //读出与写入一致 则成功
		  DisplayString(1,2,"保存年份成功"); 
			 ScmJIG.ucYear = ucTempYear;
		}else{
		  DisplayString(1,2,"保存年份失败");
		}
	}
	if(ucTempMonth != ScmJIG.ucmonth){  // 月已修改
		AT24CXX_WriteOneByte(MONTH_ADD,ucTempMonth);
		LCD_delay_ms(5);	
		ucTemp = AT24CXX_ReadOneByte(MONTH_ADD);
		if(ucTemp == ucTempMonth){      //读出与写入一致 则成功
		  DisplayString(2,2,"保存月份成功"); 
			ScmJIG.ucmonth = ucTempMonth;
		}else{
		  DisplayString(2,2,"保存月份失败");
		}
	}	
	if(ucTempDeviceCode != ScmJIG.ucDeviceCode){  // 设备码已修改
		AT24CXX_WriteOneByte(DEVICE_ADD,ucTempDeviceCode);
		LCD_delay_ms(5);	
		ucTemp = AT24CXX_ReadOneByte(DEVICE_ADD);
		if(ucTemp == ucTempDeviceCode){            // 读出与写入一致 则成功
		  DisplayString(3,2,"保存设备码成功"); 
			ScmJIG.ucDeviceCode = ucTempDeviceCode;
		}else{
		  DisplayString(3,2,"保存设备码失败");
		}
	}		
}

//	ucTempYear = ScmJIG.ucYear;
//	ucTempMonth = ScmJIG.ucmonth;
//	ucTempDeviceCode = ScmJIG.ucDeviceCode;
void Menu2_5_1(void) //年份
{
 if(fg_ucKeyDir == UP){      //0x0f000000 bit3
		if( ucTempYear < 26 ){
		  ucTempYear += 1;
		}
	}else if(fg_ucKeyDir == DOWN){
		if( ucTempYear > 16 ){
		  ucTempYear -= 1;
		}
	}
	sprintf(cPrintBu,"%02d",ucTempYear);
	DisplayString(1,3,cPrintBu);	
}
void Menu2_6_1(void) //月份
{
 if(fg_ucKeyDir == UP){      //0x0f000000 bit3
		if( ucTempMonth < 12 ){
		  ucTempMonth += 1;
		}
	}else if(fg_ucKeyDir == DOWN){
		if( ucTempMonth > 1 ){
		  ucTempMonth -= 1;
		}
	}
	sprintf(cPrintBu,"%02d",ucTempMonth);
	DisplayString(1,7,cPrintBu);	
}


/*
ScmAT24CxxErrTypeDef ScmMenuErr;    //误差系数
char cPrintBu[20] = {0x00};
*/
void Menu3_1(void) //误差设置
{
  FillSpace();
	//DisplayString(4,2,"确定"); 
	DisplayString(1,2,"误差系数设置");
//	ScmMenuErr.fInputVol = ScmAT24CxxErr.fInputVol;
//	ScmMenuErr.fInputCur = ScmAT24CxxErr.fInputCur;
//	ScmMenuErr.fInputPow = ScmAT24CxxErr.fInputPow;
//	ScmMenuErr.fOutputVol = ScmAT24CxxErr.fOutputVol;
//	ScmMenuErr.fOutputCur = ScmAT24CxxErr.fOutputCur;
//	ScmMenuErr.fOutputPow = ScmAT24CxxErr.fOutputPow;
	
	sprintf(cPrintBu,"I_Vol:%2.1f%%",ScmMenuErr.fInputVol);
	DisplayString(2,2,cPrintBu);
	sprintf(cPrintBu,"I_Cur:%2.1f%%",ScmMenuErr.fInputCur);
	DisplayString(3,2,cPrintBu);
	sprintf(cPrintBu,"I_Pow:%2.1f%%",ScmMenuErr.fInputPow);
	DisplayString(4,2,cPrintBu);
	
	DisplayChar(2,1,SELECT_ICO); 
}

void Menu3_2(void) //误差设置 InputVol
{	
  if(fg_ucKeyDir == UP){      //0x0f000000 bit3
		if(ScmMenuErr.fInputVol >= 9.8 ){
		  ScmMenuErr.fInputVol = 0.0;
		}else{
		  ScmMenuErr.fInputVol += 0.1;
		} 
	}else if(fg_ucKeyDir == DOWN){
		if(ScmMenuErr.fInputVol <= 0.1 ){
		  ScmMenuErr.fInputVol = 9.9;
		}else{
		  ScmMenuErr.fInputVol -= 0.1;
		} 
	}	
	sprintf(cPrintBu,"I_Vol:%2.1f%%",ScmMenuErr.fInputVol);
	DisplayString(2,2,cPrintBu);
	
  DisplayChar(2,1,SELECT_ICO);
 // DisplayChar(4,1,' '); 
}
void Menu3_3(void) //误差设置 fInputCur
{	
  if(fg_ucKeyDir == UP){      //0x0f000000 bit3
		if(ScmMenuErr.fInputCur >= 9.8 ){
		  ScmMenuErr.fInputCur = 0.0;
		}else{
		  ScmMenuErr.fInputCur += 0.1;
		} 
	}else if(fg_ucKeyDir == DOWN){
		if(ScmMenuErr.fInputCur <= 0.1 ){
		  ScmMenuErr.fInputCur = 9.9;
		}else{
		  ScmMenuErr.fInputCur -= 0.1;
		} 
	}	
	sprintf(cPrintBu,"I_Cur:%2.1f%%",ScmMenuErr.fInputCur);
	DisplayString(3,2,cPrintBu);
	
  DisplayChar(3,1,SELECT_ICO);
  DisplayChar(2,1,' '); 
}

void Menu3_4(void) //误差设置 fInputPow
{	
  if(fg_ucKeyDir == UP){      //0x0f000000 bit3
		if(ScmMenuErr.fInputPow >= 9.8 ){
		  ScmMenuErr.fInputPow = 0.0;
		}else{
		  ScmMenuErr.fInputPow += 0.1;
		} 
	}else if(fg_ucKeyDir == DOWN){
		if(ScmMenuErr.fInputPow <= 0.1 ){
		  ScmMenuErr.fInputPow = 9.9;
		}else{
		  ScmMenuErr.fInputPow -= 0.1;
		} 
	}	
	sprintf(cPrintBu,"I_Pow:%2.1f%%",ScmMenuErr.fInputPow);
	DisplayString(4,2,cPrintBu);
	
  DisplayChar(4,1,SELECT_ICO);
  DisplayChar(3,1,' '); 
}

void Menu3_5(void)
{
  FillSpace();
	sprintf(cPrintBu,"O_Vol:%2.1f%%",ScmMenuErr.fOutputVol);
	DisplayString(1,2,cPrintBu);
	sprintf(cPrintBu,"O_Cur:%2.1f%%",ScmMenuErr.fOutputCur);
	DisplayString(2,2,cPrintBu);
	sprintf(cPrintBu,"O_Pow:%2.1f%%",ScmMenuErr.fOutputPow);
	DisplayString(3,2,cPrintBu);

	DisplayString(4,2,"保存");
	
	DisplayChar(1,1,SELECT_ICO); 

}

void Menu3_6(void) //误差设置 fOutputVol //FillSpace();
{	
  if(fg_ucKeyDir == UP){      //0x0f000000 bit3
		if(ScmMenuErr.fOutputVol >= 9.8 ){
		  ScmMenuErr.fOutputVol = 0.0;
		}else{
		  ScmMenuErr.fOutputVol += 0.1;
		} 
	}else if(fg_ucKeyDir == DOWN){
		if(ScmMenuErr.fOutputVol <= 0.1 ){
		  ScmMenuErr.fOutputVol = 9.9;
		}else{
		  ScmMenuErr.fOutputVol -= 0.1;
		} 
	}	
	sprintf(cPrintBu,"O_Vol:%2.1f%%",ScmMenuErr.fOutputVol);
	DisplayString(1,2,cPrintBu);
  DisplayChar(1,1,SELECT_ICO);
  //DisplayChar(4,1,' '); 
}
void Menu3_7(void) //误差设置 fOutputCur
{	
  if(fg_ucKeyDir == UP){      //0x0f000000 bit3
		if(ScmMenuErr.fOutputCur >= 9.8 ){
		  ScmMenuErr.fOutputCur = 0.0;
		}else{
		  ScmMenuErr.fOutputCur += 0.1;;
		} 
	}else if(fg_ucKeyDir == DOWN){
		if(ScmMenuErr.fOutputCur <= 0.1 ){
		  ScmMenuErr.fOutputCur = 9.9;
		}else{
		  ScmMenuErr.fOutputCur -= 0.1;;
		} 
	}	
	sprintf(cPrintBu,"O_Cur:%2.1f%%",ScmMenuErr.fOutputCur);
	DisplayString(2,2,cPrintBu);
  DisplayChar(2,1,SELECT_ICO);
  DisplayChar(1,1,' '); 
}
void Menu3_8(void) //误差设置 fOutputPow
{	
  if(fg_ucKeyDir == UP){      //0x0f000000 bit3
		if(ScmMenuErr.fOutputPow >= 9.8 ){
		  ScmMenuErr.fOutputPow = 0.0;
		}else{
		  ScmMenuErr.fOutputPow += 0.1;;
		} 
	}else if(fg_ucKeyDir == DOWN){
		if(ScmMenuErr.fOutputPow <= 0.1 ){
		  ScmMenuErr.fOutputPow = 9.9;
		}else{
		  ScmMenuErr.fOutputPow -= 0.1;;
		} 
	}	
	sprintf(cPrintBu,"O_Pow:%2.1f%%",ScmMenuErr.fOutputPow);
	DisplayString(3,2,cPrintBu);
  DisplayChar(3,1,SELECT_ICO);
  DisplayChar(2,1,' '); 
}

void Menu3_9(void) //保存
{
  DisplayChar(4,1,SELECT_ICO);
  DisplayChar(3,1,' '); 
}

void Menu3_9_1(void) //保存
{
	unsigned char ucBuf[30] = {0x00};
	unsigned char ucBuf2[30] = {0x00};
	unsigned char i = 0;
  FillSpace();
	ScmAT24CxxErr.fInputVol = ScmMenuErr.fInputVol;
	ScmAT24CxxErr.fInputCur = ScmMenuErr.fInputCur;
	ScmAT24CxxErr.fInputPow = ScmMenuErr.fInputPow;
	
	ScmAT24CxxErr.fOutputVol = ScmMenuErr.fOutputVol;
	ScmAT24CxxErr.fOutputCur = ScmMenuErr.fOutputCur;
	ScmAT24CxxErr.fOutputPow = ScmMenuErr.fOutputPow;

  ErrScm2Array(ScmAT24CxxErr,&ucBuf[0]);	
  AT24CXX_Write(0x10,ucBuf,26);
	LCD_delay_ms(5);
	//memset(ucBuf,0,15);	
	AT24CXX_Read(0x10,ucBuf2,26);
	for(i=0;i<26;i++){
	  if(ucBuf[i] != ucBuf2[i] ){
		  DisplayString(1,1,"保存失败");
			return;
		}
	}
	//GC9200_SendString(ucBuf2,15);///
	DisplayString(1,1,"保存成功");		; 
}

/*******************************************************************************
* Name          : sKeyTab
* Description   : 存放按键值结构体，// SIZE_OF_KEYBD_MENU 50
*******************************************************************************/
ScmKbdTabStruct sKeyTab[SIZE_OF_KEYBD_MENU] = //60
{
// Ix  U  En Esc D 
  { 0, 1, 1, 1,  2,(*HomePage)},     //  
  { 1, 1, 1, 1,  2,(*HomePageVoid)}, //
  { 2, 5,19, 3,  4,(*Menu1) },  //clean + (Device Code)
  { 3, 5,19, 3,  4,(*Menu2) },  //Device code and Date
  { 4, 3,30, 3,  5,(*Menu3) },  //error coefficient
  { 5, 4, 6, 3,  3,(*Menu4) },  //Mode Setting
	
  { 6,10,11, 2,  8,(*Menu4_1) },  //Mode Setting + clean
  { 7,10,11, 2,  8,(*Menu4_2) },  //Recover setting
  { 8, 7,12, 2,  9,(*Menu4_3) },  //test mode
  { 9, 8,17, 2, 10,(*Menu4_4) },  //JIG information
  {10, 9,18, 2,  7,(*Menu4_5) },  //inquire and print ID
          
  {11, 6, 6, 6, 6,(*Menu4_2_1) }, //Recover setting enter + clean
  {12,14,15, 6,14,(*Menu4_3_1) },  //test mode enter + clean
  {13,14,15, 6,14,(*Menu4_3_2) },  //corretion mode
  {14,13,16, 6,13,(*Menu4_3_3) },  //check mode
  {15, 6, 6, 6, 6,(*Menu4_3_2_1) },//corretion mode enter
  {16, 6, 6, 6, 6,(*Menu4_3_3_1) },//check mode enter

  {17, 6, 6, 6, 6,(*Menu4_4_1) },  //JIG information enter
  {18, 6, 6, 6, 6,(*Menu4_5_1) },  //inquire and print ID enter

  {19,25,21, 2,25,(*Menu2_1) }, //Device code and Date enter
  {20,25,21, 2,25,(*Menu2_2)},  //Device code H	
  {21,26,22, 2,26,(*Menu2_3)},  //Device code L
  {22,23,23, 2,27,(*Menu2_4) }, //Save
  {23,28,24, 2,28,(*Menu2_5) }, //year
  {24,29,20, 2,29,(*Menu2_6) }, //month

  {25,25,21, 2,25,(*Menu2_2_1)},//Device code H Change	 
  {26,26,22, 2,26,(*Menu2_3_1)},//Device code L Change
  {27, 2, 2, 2, 2,(*Menu2_4_1)},//Save enter
  {28,28,24, 2,28,(*Menu2_5_1)},//year enter
  {29,29,20, 2,29,(*Menu2_6_1)},//month enter

  {30,31,32, 2,31,(*Menu3_1) },  //error coefficient 1 +clean
  {31,31,32, 2,31,(*Menu3_2) },  //input Voltage
  {32,32,33, 2,32,(*Menu3_3) },	 //input Current
  {33,33,34, 2,33,(*Menu3_4)},   //input power	
 
  {34,35,36, 2,35,(*Menu3_5) },  //error coefficient 2 +clean
  {35,35,36, 2,35,(*Menu3_6) }, //output Voltage
  {36,36,37, 2,36,(*Menu3_7) }, //output Current
  {37,37,38, 2,37,(*Menu3_8) }, //output power
  {38,38,30, 2,39,(*Menu3_9) }, //Save coefficient
  {39, 2, 2, 2, 2,(*Menu3_9_1) }, //Save coefficient enter			
};

//extern unsigned char  g_ucKeySetFlag ;
/*******************************************************************************
* Function Name : KeyOperation
* Description   : 按键操作函数，根据索引号执行对应函数
*******************************************************************************/
void KeyOperation(void)
{

  void (*funcp)();          // 指针函数 带指针的函数
	if(KeyHit() > 0){        // 有按键值才进行以下操作
		  g_ucBeepOnCnt = 0;  // 蜂鸣器响		
		  switch(KeyGet())   // get key value
		  {
				case UP: 
					fg_ucKeyDir = UP;				
					fg_uiKeyFuncIndex  = sKeyTab[fg_uiKeyFuncIndex].ucKeyUpState; 
					break;
				case DOWN: 
					fg_ucKeyDir = DOWN;
					fg_uiKeyFuncIndex  = sKeyTab[fg_uiKeyFuncIndex].ucKeyDownState;					
					break;					
				case ENTER: 
					fg_ucKeyDir = ENTER;
					fg_uiKeyFuncIndex  = sKeyTab[fg_uiKeyFuncIndex].ucKeyEnterState; //ucKeyEscState
					break;
				case ESC: 
					fg_ucKeyDir = ESC;
					fg_uiKeyFuncIndex  = sKeyTab[fg_uiKeyFuncIndex].ucKeyBackState; //ucKeyEscState
					break;				
//				case ENTER_LONG:
//					fg_ucKeyDir = ENTER_LONG;
//				  fg_uiKeyFuncIndex  = sKeyTab[fg_uiKeyFuncIndex].ucKeyEnterLongState;
//				  break;
		  }		
			funcp = sKeyTab[fg_uiKeyFuncIndex].CurrentOperate;	
			(*funcp)();
				
  }else{
	  fg_ucKeyDir = 0;
	}
	
//	if(fg_ucFuncRepeatFlag>0){ //重复操作标志		  
//		if( g_uiFuncRepeatCnt >= FUNC_MAX_TIME){      // 0.5s	
//			g_uiFuncRepeatCnt = 0;
//			fg_ucFuncRepeatFlag = 0;
//		  funcp = sKeyTab[fg_uiKeyFuncIndex].CurrentOperate;	
//		  (*funcp)();			
//		}			
//	}	
}


