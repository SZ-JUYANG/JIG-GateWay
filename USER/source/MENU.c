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
/*****************���񻺴�*******************/
#define MUNE_TASK_SIZE 2  //�������������У�
#define MUNE_BUF_SIZE  2  //��������ȣ��У�

//unsigned char g_ucMenuTaskRequireBuf[MUNE_TASK_SIZE][MUNE_BUF_SIZE];
//unsigned char g_ucMenuTaskAckBuf[MUNE_TASK_SIZE][MUNE_BUF_SIZE];

//static unsigned char  fg_ucMenuTaskRequireBufInIndex  = 0;  //����buf����ָ��
//static unsigned char  fg_ucMenuTaskRequireBufOutIndex = 0;  //����buf���ָ��
//static unsigned char  fg_ucMenuTaskRequireNum  = 0;         //����buf���������

//static unsigned char  fg_ucMenuTaskAckBufInIndex = 0;       //��Ӧbuf���ָ��
//static unsigned char  fg_ucMenuTaskAckBufOutIndex = 0;      //��Ӧbuf���ָ��
//static unsigned char  fg_ucMenuTaskAckNum  = 0;             //��Ӧbuf���������
//static unsigned char  fg_ucTempBuf[MUNE_BUF_SIZE];          //���񻺴�

//static unsigned char g_ucYear;
//static unsigned char g_ucMonth;
//static unsigned char g_ucDate;
//static unsigned char g_ucHour;
//static unsigned char g_ucMinute;

extern unsigned char g_ucBeepOnCnt;
extern unsigned char g_ucEnFlag;


static unsigned short int fg_uiKeyFuncIndex = 0; //
static unsigned char fg_ucKeyDir = 0x00;         //��¼��������

extern unsigned char g_ucEnableFlag;
extern unsigned char g_ucBeepOnCnt;
extern ScmAT24CxxTypeDef    ScmAT24CxxIdData; //�洢��ID �豸�롭��
extern ScmAT24CxxErrTypeDef ScmAT24CxxErr; //���ϵ��
extern ScmCorrectionTypeDef  DPS_WritePara;

extern ScmJIGTypeDef    ScmJIG;      // �洢 �ξ�

//ScmAT24CxxTypeDef    ScmMenuIdData; //�洢��ID �豸�롭��
ScmAT24CxxErrTypeDef ScmMenuErr;    //���ϵ��
char cPrintBu[20] = {0x00};
/*******************************************************************************
* Function Name : MenuTaskRequireBufOut
* Description   : �ӻ�����ȡ��һ��������
*******************************************************************************/
//unsigned char* MenuTaskRequireBufOut(void)
//{
//	unsigned char ucCnt;	
//  if(fg_ucMenuTaskRequireNum > 0){             //�鿴�Ƿ���������buf��                                             
//    fg_ucMenuTaskRequireNum--;                 //-1��ʾȡ��һ��ֵ 
//		for(ucCnt=0;ucCnt<MUNE_BUF_SIZE;ucCnt++){
//		  fg_ucTempBuf[ucCnt] = g_ucMenuTaskRequireBuf[fg_ucMenuTaskRequireBufOutIndex][ucCnt];
//		}		
//    fg_ucMenuTaskRequireBufOutIndex++;
//    if(fg_ucMenuTaskRequireBufOutIndex >= MUNE_TASK_SIZE){    //buf����ָ��ѭ���ص����               
//      fg_ucMenuTaskRequireBufOutIndex = 0;
//    }                                               
//		//return g_ucMenuTaskRequireBuf[fg_ucMenuTaskRequireBufOutIndex]; //����ֵ
//		return fg_ucTempBuf;
//  }else{                      
//    return 0;                           //û��,�򷵻�0
//  }
//}
/*******************************************************************************
* Function Name : MenuTaskAckBufOut
* Description   : �����Ӧ
*******************************************************************************/
//void MenuTaskRequireBufClear(void)
//{
//  while(fg_ucMenuTaskRequireNum){
//	  MenuTaskRequireBufOut();
//	}
//}
/*******************************************************************************
* Function Name : MenuTaskRequireBufIn
* Description   : ��һ������������뻺����
*******************************************************************************/
//void MenuTaskRequireBufIn (unsigned char *pucTempBuf)
//{                        
//    unsigned char ucCnt;
//	  if(fg_ucMenuTaskRequireNum >= MUNE_TASK_SIZE){   //buf������������һ��ֵ                   
//      MenuTaskRequireBufOut();                                                 
//    }                  
//    fg_ucMenuTaskRequireNum++;
//		for(ucCnt=0;ucCnt<MUNE_BUF_SIZE;ucCnt++){
//		  g_ucMenuTaskRequireBuf[fg_ucMenuTaskRequireBufInIndex][ucCnt] = *pucTempBuf;
//		  pucTempBuf++;
//		}
//		fg_ucMenuTaskRequireBufInIndex++;
//    if(fg_ucMenuTaskRequireBufInIndex >= MUNE_TASK_SIZE){     //buf����ָ��ѭ���ص����           
//      fg_ucMenuTaskRequireBufInIndex = 0;
//    }                      
//}
/*******************************************************************************
* Function Name : MenuTaskRequireBufIn
* Description   : ����Ƿ�������������ڻ�����
*******************************************************************************/
//unsigned char MenuTaskRequireFlag(void)
//{
//  if(fg_ucMenuTaskRequireNum>0){  //��ʾ������
//	  return 0x01;
//	}else{
//	  return 0x00;
//	}
//}
/*******************************************************************************
* Function Name : MenuTaskAckBufOut
* Description   : �ӻ�����ȡ��һ����Ӧ
*******************************************************************************/
//unsigned char* MenuTaskAckBufOut(void)
//{
//	unsigned char ucCnt;
//  if(fg_ucMenuTaskAckNum > 0){      //�鿴�Ƿ���������buf��                                             
//    fg_ucMenuTaskAckNum--;          //-1��ʾȡ��һ��ֵ
//		for(ucCnt = 0;ucCnt < MUNE_BUF_SIZE;ucCnt++){
//		  fg_ucTempBuf[ucCnt] = g_ucMenuTaskAckBuf[fg_ucMenuTaskAckBufOutIndex][ucCnt];
//		}		
//    fg_ucMenuTaskAckBufOutIndex++;
//    if(fg_ucMenuTaskAckBufOutIndex >= MUNE_TASK_SIZE){          //buf����ָ��ѭ���ص����               
//      fg_ucMenuTaskAckBufOutIndex = 0;
//    }                                               
//		//return g_ucMenuTaskAckBuf[fg_ucMenuTaskAckBufOutIndex]; //����ֵ
//		return fg_ucTempBuf;
//  }else{                      
//    return 0;                           //û��,�򷵻�0
//  }
//}
/*******************************************************************************
* Function Name : MenuTaskAckBufOut
* Description   : �����Ӧ
*******************************************************************************/
//void MenuTaskAckBufClear(void)
//{
//  while(fg_ucMenuTaskAckNum){
//	  MenuTaskAckBufOut();
//	}
//}

/*******************************************************************************
* Function Name : HomePage
* Description   : ��һ����Ӧ���뻺����
*******************************************************************************/
//void MenuTaskAckBufIn (unsigned char *pucTempBuf)
//{                        
//    unsigned char ucCnt;
//	  if(fg_ucMenuTaskAckNum >= MUNE_TASK_SIZE){            //buf������������һ��ֵ                   
//      MenuTaskAckBufOut();                                                 
//    }                  
//    fg_ucMenuTaskAckNum++;
//		for(ucCnt = 0;ucCnt < MUNE_BUF_SIZE;ucCnt++){
//		  g_ucMenuTaskAckBuf[fg_ucMenuTaskAckBufInIndex][ucCnt] = *pucTempBuf;
//		  pucTempBuf++;
//		}	
//		fg_ucMenuTaskAckBufInIndex++;				
//    if(fg_ucMenuTaskAckBufInIndex >= MUNE_TASK_SIZE){     //buf����ָ��ѭ���ص����           
//      fg_ucMenuTaskAckBufInIndex = 0;
//    }                      
//}
/*******************************************************************************
* Function Name : MenuTaskAckBufIn
* Description   : ����Ƿ�����Ӧ���ڻ�����
*******************************************************************************/
//unsigned char MenuTaskAckFlag(void)
//{
//  if(fg_ucMenuTaskAckNum>0){  //��ʾ������
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

#define PIONT_ICO  0x10 //��������
#define SELECT_ICO 0x02 //Ц��
#define UP_ICO 0x1E //��������

unsigned char Table1[1]="1";
unsigned char Table2[1]="2";
unsigned char Table3[1]="3";
unsigned char Table4[1]="4";
/*******************************************************************************
* Function Name : HomePage
* Description   : ��ʾ��ҳ
*******************************************************************************/
void HomePage(void)
{
	//Lcd12864_clear();
  FillSpace();	
	DisplayString(1,2,"�������ӿƼ�");
	DisplayString(2,3,"LED-JIG");
	DisplayString(3,2,"����");
	DisplayString(4,2,"�밴��һ��");		
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
void Menu1(void)//������ҳ
{
  //Lcd12864_clear();
	FillSpace();
	LCD_delay_ms(50);
	DisplayString(1,2,"�������ӿƼ�");
	DisplayString(2,2,"��������");
	DisplayString(3,2,"�豸����������");
	DisplayString(4,2,"���ϵ������");
	
	DisplayChar(3,1,SELECT_ICO);
	//DisplayChar(4,1,' ');	
}
/*******************************************************************************
* Function Name : Menu2
* Description   : 
*******************************************************************************/
void Menu2(void)
{
	DisplayChar(3,1,SELECT_ICO); //���豸�������
	DisplayChar(4,1,' ');
  DisplayChar(2,1,' ');	
}

void Menu3(void)
{
  DisplayChar(2,1,' ');
	DisplayChar(3,1,' ');
	DisplayChar(4,1,SELECT_ICO); //�������
	
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
	DisplayChar(2,1,SELECT_ICO); //����ģʽ����
	DisplayChar(4,1,' ');
}
void Menu4_1(void)
{
  FillSpace();
  DisplayString(1,2,"�ָ���������");
	DisplayString(2,2,"����ģʽ����");
	DisplayString(3,2,"�ξ���Ϣ��ѯ");	
	DisplayString(4,2,"��ѯ�ʹ�ӡID");
	DisplayChar(1,1,SELECT_ICO); 
}
void Menu4_2(void) //��������
{
  DisplayChar(1,1,SELECT_ICO);
	DisplayChar(2,1,' '); 
	DisplayChar(4,1,' ');
}
void Menu4_3(void) //����ģʽ����
{
  DisplayChar(1,1,' ');
	DisplayChar(2,1,SELECT_ICO); 
	DisplayChar(3,1,' ');
}
void Menu4_4(void) //�ξ���Ϣ��ѯ
{
  DisplayChar(4,1,' ');
	DisplayChar(2,1,' '); 
	DisplayChar(3,1,SELECT_ICO);
}

void Menu4_5(void) //��ѯ�ʹ�ӡID
{
  DisplayChar(1,1,' ');
	DisplayChar(3,1,' '); 
	DisplayChar(4,1,SELECT_ICO);
}


void Menu4_2_1(void)//�ָ���������
{
	FillSpace();
	DisplayString(1,1,"��������ȴ�");
	if(GC9200GetNetworkNum()){ //
		DisplayString(1,1,"�ָ���������");
		if(GC9200GetNetworkID(&DPS_WritePara)){
		  if(SetFactory(DPS_WritePara.uiID)){
			  DisplayString(1,7,"�ɹ�");
			}else{
			  DisplayString(1,7,"ʧ��");
			}						
		}else{
		  DisplayString(1,1,"����ʧ��    ");
		}
		
	}else{
	  DisplayString(1,1,"����ʧ��    ");
	}
	KeyFlush();
}



void Menu4_3_1(void) //����ģʽ����
{
  unsigned char ucTemp[2] = {0};
	FillSpace();
	ucTemp[0] = MODE_1;
  DisplayString(1,1,"��ǰģʽ: ");
  if(ScmJIG.ucTestMode == MODE_1){
	  DisplayString(1,6,"У��");
	}else{
	  DisplayString(1,6,"����");
	}
	
	DisplayString(2,2,"У������ģʽ");
  DisplayString(3,2,"��������ģʽ");
		
	DisplayChar(2,1,SELECT_ICO);	
	DisplayChar(3,1,' '); 
}

void Menu4_3_2(void) //У��ģʽ����
{	
	DisplayChar(2,1,SELECT_ICO);	
	DisplayChar(3,1,' '); 
}

void Menu4_3_3(void) //����ģʽ����
{

	DisplayChar(2,1,' '); 
	DisplayChar(3,1,SELECT_ICO);	
}

void Menu4_3_2_1(void) //У��ģʽ����
{	
  unsigned char ucTemp = 0;
	FillSpace();
	DisplayString(3,1,"����У��ģʽ");
	if(ScmJIG.ucTestMode != MODE_1){ //
	  AT24CXX_WriteOneByte(MODE_ADD,MODE_1);
		LCD_delay_ms(5);	
		ucTemp = AT24CXX_ReadOneByte(MODE_ADD);
		if(ucTemp == MODE_1){
			ScmJIG.ucTestMode = MODE_1;
		  DisplayString(3,7,"�ɹ�");
		}else{
		  DisplayString(3,7,"ʧ��");
		}
	}else{
	  DisplayString(3,7,"�ɹ�");
	}
}
void Menu4_3_3_1(void) //У��ģʽ����
{	
  unsigned char ucTemp = 0;
	FillSpace();
	DisplayString(3,1,"���ó���ģʽ");
	if(ScmJIG.ucTestMode != MODE_2){ //
	  AT24CXX_WriteOneByte(MODE_ADD,MODE_2);
		LCD_delay_ms(5);	
		ucTemp = AT24CXX_ReadOneByte(MODE_ADD);
		if(ucTemp == MODE_2){
			ScmJIG.ucTestMode = MODE_2;
		  DisplayString(3,7,"�ɹ�");
		}else{
		  DisplayString(3,7,"ʧ��");
		}
	}else{
	  DisplayString(3,7,"�ɹ�");
	}
}

void Menu4_4_1(void)
{
 unsigned char ucTemp[2] = {0};
	FillSpace();
	ucTemp[0] = MODE_2;
  DisplayString(1,1,"ID��");
  DisplayString(2,1,"��ʼID: ");
  DisplayString(3,1,"��ǰID: ");
  DisplayString(4,1,"ʣ������");
 
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
	DisplayString(1,1,"��������ȴ�");
	if(GC9200GetNetworkNum()){ //				
		//DisplayString(1,1,"��ӡ��ԴID�ɹ�");
		if(GC9200GetNetworkID(&DPS_WritePara)){	
			DisplayString(1,1,"�����ɹ�    ");		
			sprintf(cPrintTable,"ID:%08X",DPS_WritePara.uiID);//ReadDPSInfo(unsigned int uiID,ScmDPSInfoTypeDef *ScmDPSInfo);
			DisplayString(4,1,cPrintTable);
			if(ReadDPSInfo(DPS_WritePara.uiID,&ScmDPSInfo)){
			  sprintf(cPrintTable,"S:0x%04X H:0x%02X",ScmDPSInfo.uiSotfVer,ScmDPSInfo.ucHardVer);
				DisplayString(2,1,cPrintTable);
			  if(ScmDPSInfo.ucActiveState==0x38){
				  DisplayString(3,1,"DPS ��У��");
				}else if(ScmDPSInfo.ucActiveState==0x00){
				  DisplayString(3,1,"DPS δУ��");
				}
			  sprintf(cPrintTable,"0x%02X",ScmDPSInfo.ucDeviceCode);//ReadDPSInfo(unsigned int uiID,ScmDPSInfoTypeDef *ScmDPSInfo);			
			  DisplayString(4,7,cPrintTable);				
			}		
			DisplayString(1,1,"��ӡ��Enter ");
			KeyFlush();
				while(ucWait){
					KeyScan();
					  if(KeyHit() > 0){
	            switch(KeyGet()){   // get key value		  
				       case ENTER:	
								 DisplayString(1,1,"��ӡ��ԴID�ɹ�");
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
		  DisplayString(1,1,"����ʧ��    ");
		}
		
	}else{
	  DisplayString(1,1,"����ʧ��    ");
	}
	KeyFlush();
}
/*******************************************************************************
* Function Name : Menu1_1
* Description   : ��ѯ�˵����� page 1
*******************************************************************************/
//static unsigned char ucIDFlag = 0;
//static u32 uiTempID = 0;
//static u16 uiTempNum = 0;       //ID����

static u8  ucTempYear = 0;  //��
static u8  ucTempMonth = 0; //��
static u8  ucTempDeviceCode = 0;//�豸��

#define START_ID   0
#define CURRENT_ID 1

void Menu2_1(void) //�������ò˵�
{
  FillSpace();	
	DisplayString(1,1,"���");	
	DisplayString(1,5,"�·�");
	DisplayString(3,1,"�豸��0x");
	DisplayString(4,2,"����");

	sprintf(cPrintBu,"%02d",ScmJIG.ucYear);
	DisplayString(1,3,cPrintBu);
	
	sprintf(cPrintBu,"%02d",ScmJIG.ucmonth);
	DisplayString(1,7,cPrintBu);
	
	sprintf(cPrintBu,"%02X",ScmJIG.ucDeviceCode);
	DisplayString(3,5,cPrintBu);
	
	ucTempYear = ScmJIG.ucYear;
	ucTempMonth = ScmJIG.ucmonth;
	ucTempDeviceCode = ScmJIG.ucDeviceCode;
	
	DisplayString(4,5,"^ ");//�豸��H	
	//DisplayChar(1,1,SELECT_ICO); //��ʼID
}

void Menu2_2(void) 
{
  DisplayString(2,7,"  ");//���·�
  DisplayString(4,5,"^ ");//�豸��H	
}

void Menu2_3(void) 
{
  //DisplayString(1,7,"  ");//���·�
  DisplayString(4,5," ^");//�豸��L	
}
void Menu2_4(void) 
{
  //DisplayString(1,7,"  ");//���·�
  DisplayString(4,5,"  ");  //���豸��L	
	DisplayChar(4,1,SELECT_ICO); //����
}

void Menu2_5(void) 
{
  DisplayString(2,3," ^");//���
  DisplayString(4,1,"  ");//�屣��
}

void Menu2_6(void) 
{
  DisplayString(2,3,"  ");//�����
  DisplayString(2,7," ^");//�·�
}


void Menu2_2_1(void) //�豸��H 
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
void Menu2_3_1(void) //�豸��L
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
	
	
	
void Menu2_4_1(void) //���뱣��
{
  unsigned char ucTemp = 0;
  FillSpace();
	if(ucTempYear != ScmJIG.ucYear){  // �����޸�
		AT24CXX_WriteOneByte(YEAR_ADD,ucTempYear);
		LCD_delay_ms(5);	
		ucTemp = AT24CXX_ReadOneByte(YEAR_ADD);
		if(ucTemp == ucTempYear){      //������д��һ�� ��ɹ�
		  DisplayString(1,2,"������ݳɹ�"); 
			 ScmJIG.ucYear = ucTempYear;
		}else{
		  DisplayString(1,2,"�������ʧ��");
		}
	}
	if(ucTempMonth != ScmJIG.ucmonth){  // �����޸�
		AT24CXX_WriteOneByte(MONTH_ADD,ucTempMonth);
		LCD_delay_ms(5);	
		ucTemp = AT24CXX_ReadOneByte(MONTH_ADD);
		if(ucTemp == ucTempMonth){      //������д��һ�� ��ɹ�
		  DisplayString(2,2,"�����·ݳɹ�"); 
			ScmJIG.ucmonth = ucTempMonth;
		}else{
		  DisplayString(2,2,"�����·�ʧ��");
		}
	}	
	if(ucTempDeviceCode != ScmJIG.ucDeviceCode){  // �豸�����޸�
		AT24CXX_WriteOneByte(DEVICE_ADD,ucTempDeviceCode);
		LCD_delay_ms(5);	
		ucTemp = AT24CXX_ReadOneByte(DEVICE_ADD);
		if(ucTemp == ucTempDeviceCode){            // ������д��һ�� ��ɹ�
		  DisplayString(3,2,"�����豸��ɹ�"); 
			ScmJIG.ucDeviceCode = ucTempDeviceCode;
		}else{
		  DisplayString(3,2,"�����豸��ʧ��");
		}
	}		
}

//	ucTempYear = ScmJIG.ucYear;
//	ucTempMonth = ScmJIG.ucmonth;
//	ucTempDeviceCode = ScmJIG.ucDeviceCode;
void Menu2_5_1(void) //���
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
void Menu2_6_1(void) //�·�
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
ScmAT24CxxErrTypeDef ScmMenuErr;    //���ϵ��
char cPrintBu[20] = {0x00};
*/
void Menu3_1(void) //�������
{
  FillSpace();
	//DisplayString(4,2,"ȷ��"); 
	DisplayString(1,2,"���ϵ������");
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

void Menu3_2(void) //������� InputVol
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
void Menu3_3(void) //������� fInputCur
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

void Menu3_4(void) //������� fInputPow
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

	DisplayString(4,2,"����");
	
	DisplayChar(1,1,SELECT_ICO); 

}

void Menu3_6(void) //������� fOutputVol //FillSpace();
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
void Menu3_7(void) //������� fOutputCur
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
void Menu3_8(void) //������� fOutputPow
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

void Menu3_9(void) //����
{
  DisplayChar(4,1,SELECT_ICO);
  DisplayChar(3,1,' '); 
}

void Menu3_9_1(void) //����
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
		  DisplayString(1,1,"����ʧ��");
			return;
		}
	}
	//GC9200_SendString(ucBuf2,15);///
	DisplayString(1,1,"����ɹ�");		; 
}

/*******************************************************************************
* Name          : sKeyTab
* Description   : ��Ű���ֵ�ṹ�壬// SIZE_OF_KEYBD_MENU 50
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
* Description   : ������������������������ִ�ж�Ӧ����
*******************************************************************************/
void KeyOperation(void)
{

  void (*funcp)();          // ָ�뺯�� ��ָ��ĺ���
	if(KeyHit() > 0){        // �а���ֵ�Ž������²���
		  g_ucBeepOnCnt = 0;  // ��������		
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
	
//	if(fg_ucFuncRepeatFlag>0){ //�ظ�������־		  
//		if( g_uiFuncRepeatCnt >= FUNC_MAX_TIME){      // 0.5s	
//			g_uiFuncRepeatCnt = 0;
//			fg_ucFuncRepeatFlag = 0;
//		  funcp = sKeyTab[fg_uiKeyFuncIndex].CurrentOperate;	
//		  (*funcp)();			
//		}			
//	}	
}


