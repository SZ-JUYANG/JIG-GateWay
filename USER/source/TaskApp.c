#include "stm32f10x.h"
#include "delay.h"
#include "System_Initialize.h"
#include "main.h"
#include "STM32_UART.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "includes.h"
#include "TaskApp.h"
#include "IT8512B.h"
#include "LCD12864B.h"
#include "KeyDriver.h"

extern unsigned char  g_ucPM9805_RcvTime ; //
extern unsigned char  g_ucIT8512_RcvTime ;
extern unsigned char  g_ucGP9034_RcvTime ;
extern unsigned char  g_ucGC9200_RcvTime ;
extern unsigned char  g_ucPM9805_RcvFlag ; //
extern unsigned char  g_ucIT8512_RcvFlag ;
extern unsigned char  g_ucGP9034_RcvFlag ;
extern unsigned char  g_ucGC9200_RcvFlag ;
extern unsigned char g_ucPM9805_RecBuf[PM9805_RCV_NUM];
extern unsigned char g_ucIT8512_RecBuf[IT8512_RCV_NUM];
extern unsigned char g_ucGP9034_RecBuf[GP9034_RCV_NUM];
extern unsigned char g_ucGC9200_RecBuf[GC9200_RCV_NUM];

extern ScmAT24CxxTypeDef ScmAT24CxxIdData; //存储的ID 设备码……
extern ScmAT24CxxErrTypeDef ScmAT24CxxErr; //误差系数
extern ScmDPS_PNTypeDef ScmDPS_Para; // 存储 DPS规格 功率/电压 设备码
extern ScmAT24CxxErrTypeDef ScmDPSErrVal;
extern ScmJIGTypeDef    ScmJIG;      // 存储 治具
//const unsigned char ucPM9805_CMD[4]={0x55,0x00,0x10,0x65};

ScmPM9805TypeDef      PM9805Value;    //
ScmPM9805TypeDef      PM9805Value_1;  //
ScmPM9805TypeDef      PM9805Value_2;   //

ScmCorrectionTypeDef  DPS_WritePara;   // 写入DPS的斜率和截距

ScmReadParaTypeDef    DPS_ReadPara;    // 读Fre, AD
ScmReadParaTypeDef    DPS_ReadPara_1;  // 读Fre, AD
ScmReadParaTypeDef    DPS_ReadPara_2;  // 读Fre, AD

ScmDPSTypeDef         DPS_InquirePara; // 查询回来的参数 
ScmDPS_VersionTypeDef ScmDPS_Version; //DPS 软硬件版本 设备码 激活状态

extern ScmIT8512TypeDef ScmIT8512_1; //读两组数据 求slope intercept
extern ScmIT8512TypeDef ScmIT8512_2;
/*******************************************************************************
  * @brief  读取功率计参数 功率 电压 电流
  * @param  
  * @retval 
*******************************************************************************/
unsigned char ReadPM9805Parameter(ScmPM9805TypeDef *pPM9805)
{
  unsigned char i,j,k;
	unsigned char ucTemp = 0;
	unsigned char ucPM9805_CMD[4]={0x55,0x00,0x10,0x65}; // 55H - Address - Command - CS
	OS_ERR err;
	UnTypeDef UnionVol;
	UnTypeDef UnionCur;
	UnTypeDef UnionPow;
	UnTypeDef UnionPf;
	
	for(j=0;j<3;j++){
		memset(g_ucPM9805_RecBuf,0,PM9805_RCV_NUM);
		g_ucPM9805_RcvFlag = 0;
		PM9805_SendString(ucPM9805_CMD,4);
	  for(i = 0;i < 80;i++){        // 80 * 15 = 1.2s
		  if(g_ucPM9805_RcvFlag > 0){ //
			  for(k = 0;k < 23;k++){
				  ucTemp += g_ucPM9805_RecBuf[k];	
				}	
				if((ucTemp == g_ucPM9805_RecBuf[23])&&(0xaa == g_ucPM9805_RecBuf[0])){ // cs 
				    for(k = 0;k < 4;k++){
							UnionVol.ucDataTable[k] = g_ucPM9805_RecBuf[3+k];//AAH - Address - Command - Data - CS
							UnionCur.ucDataTable[k] = g_ucPM9805_RecBuf[7+k];//Data V,I,P,FREQ,PF
							UnionPow.ucDataTable[k] = g_ucPM9805_RecBuf[11+k];
							UnionPf.ucDataTable[k]  = g_ucPM9805_RecBuf[19+k];
						}							
				    pPM9805->fVoltage = UnionVol.fValue;
						pPM9805->fCurrent = UnionCur.fValue;
						pPM9805->fPower = UnionPow.fValue;
						pPM9805->fPowerFactory = UnionPf.fValue;
						g_ucPM9805_RcvFlag = 0;
						memset(g_ucPM9805_RecBuf,0,PM9805_RCV_NUM);
						return 1;
				}
				g_ucPM9805_RcvFlag = 0;
				memset(g_ucPM9805_RecBuf,0,PM9805_RCV_NUM);
			}
			OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err); 
		}	
	}
	return 0;
}	

/*******************************************************************************
  * @brief  DPS初厂设置
  * @param  None
  * @retval None
*******************************************************************************/
unsigned char SetFactory(unsigned int uiID)
{
    union 
    {
	    unsigned int uiValue;  
      unsigned char ucDataTable[4];
    }UnData;
	  OS_ERR err;
    unsigned char ucCMD[9]={0x00,0x00,0x00,0x00,0xa1,0x01,0x00,0xa0,0xa2};
    const unsigned char ucResCMD[5] = {0x8a,0x01,0x00,0x01,0x8c}; 
		unsigned char i,j;
		UnData.uiValue = uiID;
		for(i = 0;i < 4;i++){
		  ucCMD[3-i] = UnData.ucDataTable[i];	
		}
		
	for(i = 0;i < 3;i++){
		memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
		g_ucGC9200_RcvFlag = 0;
	  GC9200_SendString(ucCMD,9);
		for(j = 0;j < 200;j++){
			  OSTimeDlyHMSM(0,0,1,15,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){ 
					if(CompareData(g_ucGC9200_RecBuf,ucResCMD,5)){ //正确响应
						g_ucGC9200_RcvFlag = 0;
						memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
						return 1;
					}
					g_ucGC9200_RcvFlag = 0;
					memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);					
				}					    	
		}	
	}
	return 0;			
}

//将浮点数转换成char
static void Float2Table(u8 *pch,float fValue)
{
//	union
//	{
//		float f;
//		unsigned char ucTable[4];
//	}_data;
//	unsigned char i;
//	_data.f = fValue;
//	for(i = 0;i < 4;i++){
//	  pch[i] = _data.ucTable[i];	
//	}
	union
	{
		float f;
		unsigned int lData;
	}_data;
	unsigned char i;
	_data.f = fValue;
	pch[0] = (u8)(_data.lData>>24);	
	pch[1] = (u8)(_data.lData>>16);
	pch[2] = (u8)(_data.lData>>8);
	pch[3] = (u8)_data.lData;

}
/*******************************************************************************
  * @brief  
  * @param  
  * @retval 
*******************************************************************************/
static void ID2Table(u8 *pch,unsigned int ID)
{
	union
	{
		unsigned int uiID;
		unsigned char ucTable[4];
	}_data;
	unsigned char i;
	_data.uiID = ID;
	for(i = 0;i < 4;i++){
	  pch[i] = _data.ucTable[3-i];	
	}
}

/*******************************************************************************
  * @brief  求异或校验值
  * @param  None
  * @retval None
*******************************************************************************/
unsigned char CalXor(unsigned char*p,unsigned char ucNum)
{
    unsigned char i,j,ucXor = 0;
	  for(i = 0;i < ucNum;i++){
		    ucXor ^= p[i];
		}
    return ucXor;
}
/*******************************************************************************
  * @brief  求和校验值
  * @param  None
  * @retval None
*******************************************************************************/
unsigned char CalSum(unsigned char*p,unsigned char ucNum)
{
    unsigned char i,j,ucCs = 0;
	  for(i = 0;i < ucNum;i++){
		    ucCs += p[i];
		}
    return ucCs;
}
/*******************************************************************************
  * @brief  串口收到的数转存
  * @param  None
  * @retval None
*******************************************************************************/
void MoveData(ScmReadParaTypeDef *scm, unsigned char *p )
{
	unsigned short int uitemp = 0;
	/*两点校正*/
//  uitemp = p[3];
//	scm->uiI_PowFre = (uitemp<<8) + p[4];
//	uitemp = p[5];
//	scm->uiI_VolFre = (uitemp << 8) + p[6];
//	uitemp = p[7];
//	scm->uiI_CurFre= (uitemp << 8) + p[8];
//	uitemp = p[9];
//	scm->uiO_VolVal = (uitemp << 8) + p[10];
//	uitemp = p[11];
//	scm->uiO_CurVal = (uitemp << 8) + p[12];
/*输入单点输出两点*/
	uitemp = p[3];
	scm->uiO_VolVal = (uitemp << 8) + p[4];
	uitemp = p[5];
	scm->uiO_CurVal = (uitemp << 8) + p[6];		
}


/*******************************************************************************
  * @brief  读取DPS 输入Fre 输出的
  * @param  None
  * @retval None
*******************************************************************************/
unsigned char ReadDPSPara(unsigned int uiID,ScmReadParaTypeDef *scm)
{
	OS_ERR err;
  union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnData;	
  unsigned char ucCMD[8]={0x00,0x00,0x00,0x00,0xa2,0x05,0xa7,0xa7};
  unsigned char i,j,k,cs=0;
	UnData.uiValue = uiID;
	for(i = 0;i < 4;i++){
	  ucCMD[i] = UnData.ucDataTable[3-i];
	}
	for(i = 0;i < 3;i++){
		memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
		g_ucGC9200_RcvFlag = 0;
	  GC9200_SendString(ucCMD,8);
		for(j = 0;j < 200;j++){
			  OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){
					for(k = 0;k < 7;k++)
              cs += g_ucGC9200_RecBuf[k];
					if((g_ucGC9200_RecBuf[0]==0x8b)&&(g_ucGC9200_RecBuf[1]==0x05)&&(g_ucGC9200_RecBuf[2]==0x01)&&(g_ucGC9200_RecBuf[7]==cs)){
					    MoveData(scm,g_ucGC9200_RecBuf); //存储读回数据
						  memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
					    g_ucGC9200_RcvFlag = 0;
						  return 1;	
					}
					g_ucGC9200_RcvFlag = 0;
					memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);					
				}					    	
		}	
	}
	return 0;			
}

/*
ScmPM9805TypeDef      PM9805Value_2;   //

ScmCorrectionTypeDef  DPS_WritePara;   // 写入DPS的斜率和截距

ScmReadParaTypeDef    DPS_ReadPara;    // 读Fre, AD

ScmIT8512TypeDef   
*/
/*******************************************************************************
  * @brief  求 功率 电压 电流  斜率 截距
  * @param  
  * @retval 
*******************************************************************************/
void CalculatePara(ScmPM9805TypeDef ScmPM9805_1,ScmPM9805TypeDef ScmPM9805_2,
	                 ScmIT8512TypeDef ScmIT8512_1,ScmIT8512TypeDef ScmIT8512_2,	                 
	                 ScmReadParaTypeDef DPSPara1,ScmReadParaTypeDef DPSPara2,ScmCorrectionTypeDef *pScmCorrection)
{
    float fK,fB;
/*	
  //calculate input voltage slope and intercept
	fK = (ScmPM9805_2.fVoltage - ScmPM9805_1.fVoltage)/((float)(DPSPara2.uiI_VolFre - DPSPara1.uiI_VolFre));
	fB = ScmPM9805_2.fVoltage - (fK * ((float)DPSPara2.uiI_VolFre));
	pScmCorrection->fI_VolSlope = fK;  //频率单位由1Hz->0.1Hz
	pScmCorrection->fI_VolIntercept = fB;
	//calculate input current slope and intercept
	fK = (ScmPM9805_2.fCurrent - ScmPM9805_1.fCurrent)/((float)(DPSPara2.uiI_CurFre - DPSPara1.uiI_CurFre));
	fB = ScmPM9805_2.fCurrent - (fK * ((float)DPSPara2.uiI_CurFre));
	pScmCorrection->fI_CurSlope = fK;
	pScmCorrection->fI_CurIntercept = fB;	
	//calculate input power slope and intercept
	fK = (ScmPM9805_2.fPower - ScmPM9805_1.fPower)/((float)(DPSPara2.uiI_PowFre - DPSPara1.uiI_PowFre));
	fB = ScmPM9805_2.fPower - (fK * ((float)DPSPara2.uiI_PowFre));
	pScmCorrection->fI_PowSlope = fK;
	pScmCorrection->fI_PowIntercept = fB;	
*/	
/***********************************/	
	//calculate output voltage slope and intercept
	fK = ScmIT8512_2.fVoltage /DPSPara2.uiO_VolVal; //输出电压单点
//	pScmCorrection->fO_VolSlope = fK;
//	fK = (ScmIT8512_2.fVoltage - ScmIT8512_1.fVoltage)/(DPSPara2.uiO_VolVal-DPSPara1.uiO_VolVal);
  //fB = ScmIT8512_2.fVoltage - (fK * DPSPara2.uiO_VolVal);
	pScmCorrection->fO_VolSlope = fK;
	pScmCorrection->fO_VolIntercept = 0;
	
	//calculate output current slope and intercept
	ScmIT8512_2.fCurrent = ScmIT8512_2.fCurrent*1000;//转为mA
	ScmIT8512_1.fCurrent = ScmIT8512_1.fCurrent*1000;
	fK = (ScmIT8512_2.fCurrent - ScmIT8512_1.fCurrent)/(DPSPara2.uiO_CurVal - DPSPara1.uiO_CurVal); //输出电流两点
	fB = ScmIT8512_2.fCurrent - (fK * DPSPara2.uiO_CurVal);	
	pScmCorrection->fO_CurSlope = fK;
  pScmCorrection->fO_CurIntercept = fB;
	
	pScmCorrection->fI_PowerVal = ScmPM9805_2.fPower;
	pScmCorrection->fI_VoltageVal = ScmPM9805_2.fVoltage;
	pScmCorrection->fI_CurrentVal = ScmPM9805_2.fCurrent;
}

/*******************************************************************************
  * @brief  写入校正参数
  * @param  
  * @retval 
*******************************************************************************/
unsigned char WriteCorrectionPara(ScmCorrectionTypeDef scm)
{
  const unsigned char ucResCMD[4] = {0x8a,0x05,0x01,0x90}; 
  unsigned char i,j,k;
	OS_ERR err;
//	scm->uiID = 0x00;//先设置0 方便求Xor cs
//	scm->ucWR = 0xa1;
//	scm->ucCMD = 0x05; //Float2Table(u8 *pch,float fValue)
  ID2Table(&scm.ucSendDataTable[0],0); //先设置0 方便求Xor cs
	
	scm.ucSendDataTable[4] = 0xa1;
  scm.ucSendDataTable[5] = 0x05;
	/*
  Float2Table(&scm.ucSendDataTable[6],scm.fI_PowSlope);
  Float2Table(&scm.ucSendDataTable[10],scm.fI_PowIntercept);
  Float2Table(&scm.ucSendDataTable[14],scm.fI_VolSlope);
  Float2Table(&scm.ucSendDataTable[18],scm.fI_VolIntercept);
  Float2Table(&scm.ucSendDataTable[22],scm.fI_CurSlope);
  Float2Table(&scm.ucSendDataTable[26],scm.fI_CurIntercept);

  Float2Table(&scm.ucSendDataTable[30],scm.fO_VolSlope);
	Float2Table(&scm.ucSendDataTable[34],scm.fO_VolIntercept);
	
  Float2Table(&scm.ucSendDataTable[38],scm.fO_CurSlope);	
	Float2Table(&scm.ucSendDataTable[42],scm.fO_CurIntercept);	
	
  scm.ucSendDataTable[46]= CalXor(scm.ucSendDataTable,46);
  scm.ucSendDataTable[47]= CalSum(scm.ucSendDataTable,46);
	
  ID2Table(&scm.ucSendDataTable[0],scm.uiID);
	*/
	Float2Table(&scm.ucSendDataTable[6],scm.fI_PowerVal);   //输入功率值
	Float2Table(&scm.ucSendDataTable[10],scm.fI_VoltageVal);//输入电压值
	Float2Table(&scm.ucSendDataTable[14],scm.fI_CurrentVal);//输入电流值
	
	Float2Table(&scm.ucSendDataTable[18],scm.fO_VolSlope);  //输出电压斜率
	Float2Table(&scm.ucSendDataTable[22],scm.fO_CurSlope);  //输出电流斜率
	Float2Table(&scm.ucSendDataTable[26],scm.fO_CurIntercept);//输出电流截距
	scm.ucSendDataTable[30]= CalXor(scm.ucSendDataTable,30);
	scm.ucSendDataTable[31]= CalSum(scm.ucSendDataTable,30);
	ID2Table(&scm.ucSendDataTable[0],scm.uiID);
	for(i = 0;i < 3;i++){
		memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
		g_ucGC9200_RcvFlag = 0;
	  GC9200_SendString(scm.ucSendDataTable,32);
		for(j = 0;j < 200;j++){
			  OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);
				    if(g_ucGC9200_RcvFlag > 0){ 
					    if(CompareData(g_ucGC9200_RecBuf,ucResCMD,4)){ //正确响应
								  g_ucGC9200_RcvFlag = 0;
								  memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
						      return 1;
					    }
					    g_ucGC9200_RcvFlag = 0;
					    memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
						}															    	
		}	
	}
	return 0;	 
}



const unsigned char CMD_Head[8]={0x1b,0x67,0x72,0x1d,0xc0,0x6d,0x6d,0x3a};
/*******************************************************************************
* Function Name : 0x1b 0x67 0x72 0x1d 0xc0 0x6d 0x6d 0x3a FC1 FC2 Len Seq Cmd Obj Sub Values 
* Description   : 计算FC1 FC2  
* &p[10]
*******************************************************************************/
void CalculateFC(unsigned char *p,unsigned char *ucFC1,unsigned char *ucFC2, unsigned char len)
{
	unsigned char sum1=0,sum2=0,i;
	for(i=0;i<len;i++){	
		sum1 = (sum1+p[i])%255;
		sum2 = (sum1+sum2)%255;
	}
  *ucFC1 = sum1; 
	*ucFC2 = sum2;	
}

/*
send
1b 67 72 1d c0 6d 6d 3a 0b 22 05 01 00 00 05 
1b 67 72 1d c0 6d 6d 3a 4b 46 09 02 01 15 02 01 00 2a fc 
1b 67 72 1d c0 6d 6d 3a 11 58 07 03 01 00 05 00 01
respond
1b 67 72 1d c0 6d 6d 3a 07 1e 05 01 00 00 01 
1b 67 72 1d c0 6d 6d 3a 05 0d 03 02 00 
1b 67 72 1d c0 6d 6d 3a 06 0f 03 03 00
*/
/*******************************************************************************
* Function Name : SetDPS_ID_First
* Description   : 设置ID的第一帧数据 
*******************************************************************************/
unsigned char SetDPS_ID_First(unsigned int uiID)
{
  unsigned char i,j,k,cs,ucFC1=0,ucFC2=0;
	OS_ERR err;
	union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnData;
	//const unsigned char ucCmdFirst[7] = {0x0b,0x22,0x05,0x01,0x00,0x00,0x05};
  unsigned char ucCmdArray[26] = {0x1b,0x67,0x72,0x1d,0xc0,0x6d,0x6d,0x3a,0x19,0x7c,
	                                0x10,0x00,0x01,0x0f,0x00,0x01,0x00,0x00,0x01,0x26,
	                                0x8e,0x05,0x09,0x00,0x00,0x05};
  //const unsigned char ucCmdThird[9] = {0x11,0x58,0x07,0x03,0x01,0x00,0x05,0x00,0x01};
	//返回两帧数据
	UnData.uiValue = uiID;
	for(i = 0;i < 4;i++){
	  ucCmdArray[15+i] = UnData.ucDataTable[3-i];
	}

	CalculateFC(&ucCmdArray[21],&ucFC1,&ucFC2,0x05);
	ucCmdArray[19] = ucFC1;
	ucCmdArray[20] = ucFC2;

	CalculateFC(&ucCmdArray[10],&ucFC1,&ucFC2,0x10);
	ucCmdArray[8] = ucFC1;
	ucCmdArray[9] = ucFC2;
  for(i = 0;i < 3;i++){ 
		memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
		g_ucGC9200_RcvFlag = 0;
	  GC9200_SendString(ucCmdArray,26);
		for(j = 0;j < 200;j++){			
			  OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){					
					if(CompareData(g_ucGC9200_RecBuf,CMD_Head,8)){
				    g_ucGC9200_RcvFlag = 0;
				    memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);	
						for(k = 0;k < 200;k++){
						  OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);
							if(g_ucGC9200_RcvFlag > 0){								
								if(CompareData(g_ucGC9200_RecBuf,CMD_Head,8)){
									g_ucGC9200_RcvFlag = 0;
									memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);							
									return 1;					
								}
                g_ucGC9200_RcvFlag = 0;								
								memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
							}							
						}
					}
					g_ucGC9200_RcvFlag = 0;
          memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);					
				}														
		}					    			
	}	
	return 0;
}	
	/*
	for(i = 0;i < 4;i++){ 
		memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
		g_ucGC9200_RcvFlag = 0;
	  GC9200_SendString(ucCmdArray,26);
	//	for(j = 0;j < 3;j++){			
			  OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){
					if(CompareData(g_ucGC9200_RecBuf,CMD_Head,8)){
				    g_ucGC9200_RcvFlag = 0;
				    memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);							
					  //return 1;
						break;
					}								
				}
	//			g_ucGC9200_RcvFlag = 0;
	//			memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);											
	//	}					    			
	}
  if(i==4){
	  return 0;
	}
		for(j = 0;j < 4;j++){			
			  OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){
					if(CompareData(g_ucGC9200_RecBuf,CMD_Head,8)){
				    g_ucGC9200_RcvFlag = 0;
				    memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);							
					  return 1;					
					}								
				}
				g_ucGC9200_RcvFlag = 0;
				memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);											
		}	
  return 0; */


unsigned char SetDPS_ID_Third(unsigned int uiID)
{
  unsigned char i,j,k,cs,ucFC1=0,ucFC2=0;
	OS_ERR err;
	union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnData;
	//const unsigned char ucCmdFirst[7] = {0x0b,0x22,0x05,0x01,0x00,0x00,0x05};
  unsigned char ucCmdArray[28] = {0x1b,0x67,0x72,0x1d,0xc0,0x6d,0x6d,0x3a,0x95,0xae,
	                                0x12,0x00,0x01,0x0f,0x00,0x01,0x00,0x00,0x01,0x2c,
	                                0xfa,0x07,0x09,0x01,0x00,0x05,0x00,0x01 };
  //const unsigned char ucCmdThird[9] = {0x11,0x58,0x07,0x03,0x01,0x00,0x05,0x00,0x01};
	
	UnData.uiValue = uiID;
	for(i = 0;i < 4;i++){
	  ucCmdArray[15+i] = UnData.ucDataTable[3-i];
	}
	CalculateFC(&ucCmdArray[21],&ucFC1,&ucFC2,0x07);
	ucCmdArray[19] = ucFC1;
	ucCmdArray[20] = ucFC2;
	
	CalculateFC(&ucCmdArray[10],&ucFC1,&ucFC2,0x12);
	ucCmdArray[8] = ucFC1;
	ucCmdArray[9] = ucFC2;	
	for(i = 0;i < 3;i++){ 
		memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
		g_ucGC9200_RcvFlag = 0;
	  GC9200_SendString(ucCmdArray,28);
		for(j = 0;j < 200;j++){			
			  OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){				
					if(CompareData(g_ucGC9200_RecBuf,CMD_Head,8)){
						g_ucGC9200_RcvFlag = 0;
				    memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
						for(k = 0;k < 200;k++){	
							OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);
							if(g_ucGC9200_RcvFlag > 0){
								if(CompareData(g_ucGC9200_RecBuf,CMD_Head,8)){
									g_ucGC9200_RcvFlag = 0;
									memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);	
									return 1;
								}
								g_ucGC9200_RcvFlag = 0;
								memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
							}
						}				  						
					}
					g_ucGC9200_RcvFlag = 0;
					memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);	
				}																
		}					    			
	}
	return 0;
}	
/*	
  if(i==4){
	  return 0;
	}
		for(j = 0;j < 4;j++){			
			  OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){
					if(CompareData(g_ucGC9200_RecBuf,CMD_Head,8)){
				    g_ucGC9200_RcvFlag = 0;
				    memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);							
					  return 1;					
					}								
				}
				g_ucGC9200_RcvFlag = 0;
				memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);											
		}	
  return 0;  */


unsigned char SetDPS_ID(unsigned int uiID,unsigned int uiSetID)
{
  unsigned char i,j,k,cs,ucFC1=0,ucFC2=0;
	OS_ERR err;
	union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnData;
	//const unsigned char ucCmdFirst[7] = {0x0b,0x22,0x05,0x01,0x00,0x00,0x05};
  unsigned char ucCmdArray[30] = {0x1b,0x67,0x72,0x1d,0xc0,0x6d,0x6d,0x3a,0x98,0xb9,
	                                0x14,0x00,0x01,0x0f,0x00,0x01,0x00,0x00,0x01,0x42,
	                                0xd0,0x09,0x0,0x01,0x15,0x02,0x01,0x00,0x00,0x03};
  //const unsigned char ucCmdThird[9] = {0x11,0x58,0x07,0x03,0x01,0x00,0x05,0x00,0x01};
	
	UnData.uiValue = uiID;
	for(i = 0;i < 4;i++){
	  ucCmdArray[15+i] = UnData.ucDataTable[3-i];
	}
	UnData.uiValue = uiSetID;
	for(i = 0;i < 4;i++){
	  ucCmdArray[26+i] = UnData.ucDataTable[3-i];
	}

	CalculateFC(&ucCmdArray[21],&ucFC1,&ucFC2, 0x09);
	ucCmdArray[19] = ucFC1;
	ucCmdArray[20] = ucFC2;	
	
	CalculateFC(&ucCmdArray[10],&ucFC1,&ucFC2, 0x14);
	ucCmdArray[8] = ucFC1;
	ucCmdArray[9] = ucFC2;
	
	for(i = 0;i < 3;i++){ 
		memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
		g_ucGC9200_RcvFlag = 0;
	  GC9200_SendString(ucCmdArray,30);
		for(j = 0;j < 200;j++){			
			  OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){			
					if(CompareData(g_ucGC9200_RecBuf,CMD_Head,8)){
				    g_ucGC9200_RcvFlag = 0;
				    memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
						for(k = 0;k < 200;k++){
							  OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);
							  if(g_ucGC9200_RcvFlag > 0){								  
									if(CompareData(g_ucGC9200_RecBuf,CMD_Head,8)){
										g_ucGC9200_RcvFlag = 0;
										memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
										return 1;
									}
									g_ucGC9200_RcvFlag = 0;
									memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
								}
						}				  					
					}
					g_ucGC9200_RcvFlag = 0;
					memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
				}																		
		}					    			
	}	
  return 0;
}	
	/*
  if(i == 4 ){
    return 0;
	}
		for(j = 0;j < 4;j++){			
			  OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){
					if(CompareData(g_ucGC9200_RecBuf,CMD_Head,8)){
				    g_ucGC9200_RcvFlag = 0;
				    memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);							
					  return 1;					
					}								
				}
				g_ucGC9200_RcvFlag = 0;
				memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);											
		}	*/	

/*******************************************************************************
  * @brief  
  * @param  
  * @retval 
*******************************************************************************/
unsigned char GC9200GetNetworkNum(void)
{
  unsigned char i,j,k;
	OS_ERR err;
	const unsigned char ucGetIDNumArray[15]={0x1b,0x67,0x72,0x1d,0xc0,0x6d,0x6d,0x3a, //CMD_Head
			                                     0x1a,0x55,0x05,0x0a,0x00,0x09,0x02}; //
	for(i = 0;i < 50;i++){ // 45 seconds
		memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
		g_ucGC9200_RcvFlag = 0;
	  //GC9200_SendString(scm.ucSendDataTable,40);
		GC9200_SendArray(ucGetIDNumArray,15);
		for(j = 0;j < 3;j++){
			  OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){
				    if(g_ucGC9200_RcvFlag > 0){ 
					    if(CompareData(g_ucGC9200_RecBuf,CMD_Head,8)){ //正确响应
								  if((g_ucGC9200_RecBuf[10]==4)&&(g_ucGC9200_RecBuf[13]==1)){// len&&num
									  g_ucGC9200_RcvFlag = 0;
										memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
									  return 1;
									}					        
					    }
					    g_ucGC9200_RcvFlag = 0;
					    memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
						}							
				}					    	
		}	
	}		                                              
  return 0;
}

/*******************************************************************************
  * @brief  
  * @param  
  * @retval 
*******************************************************************************/
unsigned char GC9200GetNetworkID(ScmCorrectionTypeDef *scm)
{
  unsigned char i,j,k;
	OS_ERR err;
	  union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnData;	
	const unsigned char ucGetIDArray[16]={0x1b,0x67,0x72,0x1d,0xc0,0x6d,0x6d,0x3a,//CMD_Head
			                                  0x15,0x4c,0x06,0x01,0x00,0x09,0x03,0x02}; //
	for(i = 0;i < 4;i++){ 
		memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
		g_ucGC9200_RcvFlag = 0;
	  //GC9200_SendString(scm.ucSendDataTable,40);
		GC9200_SendArray(ucGetIDArray,16);
		for(j = 0;j < 10;j++){
			  OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){
					  if(CompareData(g_ucGC9200_RecBuf,CMD_Head,8)){ //正确响应
                for(k = 0;k < 4;k++){
								  UnData.ucDataTable[3-k]	= g_ucGC9200_RecBuf[13+k];
								}
								scm->uiID = UnData.uiValue;								
								g_ucGC9200_RcvFlag = 0;
								memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
								return 1;
					  }
					  g_ucGC9200_RcvFlag = 0;
					  memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);											
				}					    	
		}	
	}		                                              
  return 0;
}

/*******************************************************************************
  * @brief  
  * @param  
  * @retval 
*******************************************************************************/
// ucDim 0x00~0xc8
unsigned char DPS_SetDim(unsigned int uiID,unsigned char ucDim)
{
  unsigned char i,j,cs;
	OS_ERR err;
	union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnData;	
  unsigned char ucDimArray[7] = {0x00,0x00,0x00,0x00,0x14,0x00,0x00};
	UnData.uiValue = uiID;
	for(i = 0;i < 4;i++){
	  ucDimArray[3-i] = UnData.ucDataTable[i];
	}
	ucDimArray[5] = ucDim;	
  ucDimArray[6] = ucDimArray[4] + ucDimArray[5];
	
	for(i = 0;i < 4;i++){ 
		memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
		g_ucGC9200_RcvFlag = 0;
	  GC9200_SendString(ucDimArray,7);
		//GC9200_SendArray(ucGetIDArray,16);
		for(j = 0;j < 30;j++){
			  OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){
					  cs = g_ucGC9200_RecBuf[0] +g_ucGC9200_RecBuf[1];
            if((g_ucGC9200_RecBuf[0]==0x94)&&(g_ucGC9200_RecBuf[2]==cs)){
						  g_ucGC9200_RcvFlag = 0;
							memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);	
						  return 1;
						}
				    g_ucGC9200_RcvFlag = 0;
				    memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);						
				}											
		}					    			
	}		                                              
  return 0;
}

/*******************************************************************************
  * @brief  
  * @param  
  * @retval 
*******************************************************************************/
void DPSMoveData(ScmDPSTypeDef *pscm,unsigned char *p)
{
   unsigned short int uiTemp;
   pscm->cTemperature = p[1]; 
   uiTemp = p[3];
	 pscm->fO_Voltage = (float)((uiTemp<<8)+p[2])/10;
   uiTemp = p[5];
	 pscm->fO_Current = (float)((uiTemp<<8)+p[4])/1000;
   uiTemp = p[7];
	 pscm->fI_Voltage = (float)((uiTemp<<8)+p[6])/10;
   uiTemp = p[9];
	 pscm->fI_Current = (float)((uiTemp<<8)+p[8])/1000;	
   uiTemp = p[11];
	 pscm->fPower = (float)((uiTemp<<8)+p[10])/10;		
}
/*******************************************************************************
  * @brief  
  * @param  
  * @retval 
*******************************************************************************/
unsigned char InquireDPSPara(unsigned int uiID,ScmDPSTypeDef *pscm)
{
  unsigned char i,j,k,cs = 0;
	OS_ERR err;
	union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnData;		
  unsigned char ucInquireArray[7] = {0x00,0x00,0x00,0x00,0x13,0x00,0x13};
	UnData.uiValue = uiID;
	for(i = 0;i < 4;i++){
	  ucInquireArray[i] = UnData.ucDataTable[3-i];
	}
	for(i = 0;i < 4;i++){ 
		memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
		g_ucGC9200_RcvFlag = 0;
	  GC9200_SendString(ucInquireArray,7);
		//GC9200_SendArray(ucGetIDArray,16);
		for(j = 0;j < 30;j++){
			  OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){
					  for(k = 1;k < 16;k++){
						  cs += g_ucGC9200_RecBuf[k];
						}					  
            if((g_ucGC9200_RecBuf[0] == 0x93)&&(g_ucGC9200_RecBuf[16] == cs)){
             // pscm->cTemperature = g_ucGC9200_RecBuf[1];
							DPSMoveData(pscm,g_ucGC9200_RecBuf);
							g_ucGC9200_RcvFlag = 0;
							memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);	
							return 1;
						}
				  g_ucGC9200_RcvFlag = 0;
				  memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);						
				}										
		}					    			
	}		                                              
  return 0;	
}

/*
ScmDPS_VersionTypeDef
*/

unsigned char ActivateDPS(unsigned char ucMode,unsigned int uiID,unsigned char ucDeviceCode,ScmDPS_VersionTypeDef *pscm)
{
  unsigned char i,j,k,cs = 0;
	unsigned char ucReturnVal = 0;
	unsigned char ucAckCMD = 0;
	OS_ERR err;
	union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnData;		
  unsigned char ucCmdArray[9] = {0x00,0x00,0x00,0x00,0xA1,0x06,0x00,0x00,0x13};
  if(ucMode == MODE_1){
	  ucAckCMD = 0x06;
	}else if(ucMode == MODE_2){
	  ucAckCMD = 0x07;
	}
  ucCmdArray[5] = ucAckCMD;
	
	UnData.uiValue = uiID;
	for(i = 0;i < 4;i++){
	  ucCmdArray[i] = UnData.ucDataTable[3-i]; // 装载ID
	}
	ucCmdArray[6] = ucDeviceCode;
	ucCmdArray[7] = ucCmdArray[4]^ucCmdArray[5]^ucCmdArray[6]; // Xor
	ucCmdArray[8] = ucCmdArray[4]+ucCmdArray[5]+ucCmdArray[6]; // CS
	
	for(i = 0;i < 4;i++){ 
		memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
		g_ucGC9200_RcvFlag = 0;
	  GC9200_SendString(ucCmdArray,9);
		//GC9200_SendArray(ucGetIDArray,16);
		for(j = 0;j < 30;j++){
			  OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){
					  for(k = 0;k < 7;k++){
						  cs += g_ucGC9200_RecBuf[k];
						}					  
            if((g_ucGC9200_RecBuf[0] == 0x8A)&&(g_ucGC9200_RecBuf[1] == ucAckCMD)&&(g_ucGC9200_RecBuf[7] == cs)){ //
             	pscm->uifirmware = 	g_ucGC9200_RecBuf[3];//firmeare H	
							pscm->uifirmware = 	(pscm->uifirmware<<8)+ g_ucGC9200_RecBuf[2];//firmeare L
							pscm->ucHardware =  g_ucGC9200_RecBuf[4];
							pscm->ucDeviceCode =  g_ucGC9200_RecBuf[5];	
							ucReturnVal = g_ucGC9200_RecBuf[6];
							g_ucGC9200_RcvFlag = 0;
							memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);	
							return ucReturnVal;     //01设备码不对;02激活成功;03激活失败
						}	
				    g_ucGC9200_RcvFlag = 0;
				    memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);							
				}										
		}					    			
	}		                                              
  return 0;	
}

unsigned char ConfigDPSToTestMode(unsigned int uiID,unsigned char ucDeviceCode,ScmDPS_VersionTypeDef *pscm)
{
  unsigned char i,j,k,cs = 0;
	unsigned char ucReturnVal = 0;
	OS_ERR err;
	union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnData;		
  unsigned char ucCmdArray[9] = {0x00,0x00,0x00,0x00,0xA1,0x06,0x00,0x00,0x13};
	UnData.uiValue = uiID;
	for(i = 0;i < 4;i++){
	  ucCmdArray[i] = UnData.ucDataTable[3-i];
	}
	ucCmdArray[6] = ucDeviceCode;
	ucCmdArray[7] = ucCmdArray[4]^ucCmdArray[5]^ucCmdArray[6]; // Xor
	ucCmdArray[8] = ucCmdArray[4]+ucCmdArray[5]+ucCmdArray[6]; // CS
	
	for(i = 0;i < 4;i++){ 
		memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
		g_ucGC9200_RcvFlag = 0;
	  GC9200_SendString(ucCmdArray,9);
		//GC9200_SendArray(ucGetIDArray,16);
		for(j = 0;j < 30;j++){
			  OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){
					  for(k = 0;k < 7;k++){
						  cs += g_ucGC9200_RecBuf[k];
						}					  
            if((g_ucGC9200_RecBuf[0] == 0x8A)&&(g_ucGC9200_RecBuf[1] == 0x06)&&(g_ucGC9200_RecBuf[7] == cs)){ //
             	pscm->uifirmware = 	g_ucGC9200_RecBuf[3];//firmeare H	
							pscm->uifirmware = 	(pscm->uifirmware<<8)+ g_ucGC9200_RecBuf[2];//firmeare L
							pscm->ucHardware =  g_ucGC9200_RecBuf[4];
							pscm->ucDeviceCode =  g_ucGC9200_RecBuf[5];	
							ucReturnVal = g_ucGC9200_RecBuf[6];
							g_ucGC9200_RcvFlag = 0;
							memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);	
							return ucReturnVal;     //01设备码不对;02激活成功;03激活失败
						}	
				    g_ucGC9200_RcvFlag = 0;
				    memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);							
				}										
		}					    			
	}		                                              
  return 0;	
}
unsigned char ReadDPSInfo(unsigned int uiID,ScmDPSInfoTypeDef *ScmDPSInfo)
{
  unsigned char ucCmdTable[7]={0x00,0x00,0x00,0x00,0x16,0x01,0x17};
  unsigned char i,j,k,cs = 0;
	unsigned char ucReturnVal = 0;
	union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnData;	
	OS_ERR err;
	
	UnData.uiValue = uiID;
	for(i = 0;i < 4;i++){
	  ucCmdTable[i] = UnData.ucDataTable[3-i];
	}
	for(i = 0;i < 3;i++){ 
		memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
		g_ucGC9200_RcvFlag = 0;
	  GC9200_SendString(ucCmdTable,7);
		//GC9200_SendArray(ucGetIDArray,16);
		for(j = 0;j < 30;j++){
			  OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){
				    for(k = 0;k < 6;k++){
						  cs += g_ucGC9200_RecBuf[k];
						}															  
            if((g_ucGC9200_RecBuf[0] == 0x96)&&(g_ucGC9200_RecBuf[6] == cs)){
              ScmDPSInfo->uiSotfVer = g_ucGC9200_RecBuf[2];
							ScmDPSInfo->uiSotfVer = (ScmDPSInfo->uiSotfVer<<8) + g_ucGC9200_RecBuf[1] ;
							ScmDPSInfo->ucHardVer = g_ucGC9200_RecBuf[3];
							ScmDPSInfo->ucDeviceCode = g_ucGC9200_RecBuf[4];
							ScmDPSInfo->ucActiveState = g_ucGC9200_RecBuf[5];
							g_ucGC9200_RcvFlag = 0;
							memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);	
							return 1;
						}
				  g_ucGC9200_RcvFlag = 0;
				  memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);						
				}										
		}					    			
	}		                                              
  return 0;	
}

/****************
*对V2.9协议增加函数
*****************/
unsigned char SendInstrumentDataToNode(unsigned int uiID)
{
//Float2Table(&scm.ucSendDataTable[6],scm.fI_PowSlope);
  unsigned char ucTable[28]={0x00};
	unsigned char i,j,k,cs = 0,ucReturn;
	OS_ERR err;
		union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnData;
	UnData.uiValue = uiID;
	
  ucTable[4] = 0xA1;
  ucTable[5] = 0x05;
	Float2Table(&ucTable[6],PM9805Value_1.fPower);
	Float2Table(&ucTable[10],PM9805Value_1.fVoltage);
	Float2Table(&ucTable[14],PM9805Value_1.fCurrent);
	Float2Table(&ucTable[18],ScmIT8512_1.fVoltage);
	Float2Table(&ucTable[22],ScmIT8512_1.fCurrent);
	ucTable[26] = CalXor(ucTable,26);
	ucTable[27] = CalSum(ucTable,26);
	for(i = 0;i < 4;i++){
	  ucTable[i] = UnData.ucDataTable[3-i];
	}		
	
	for(i = 0;i < 3;i++){ 
		memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);
		g_ucGC9200_RcvFlag = 0;
	  GC9200_SendString(ucTable,28);
		for(j = 0;j < 30;j++){
			  OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
				if(g_ucGC9200_RcvFlag > 0){									    
						cs = g_ucGC9200_RecBuf[0]+g_ucGC9200_RecBuf[1]+g_ucGC9200_RecBuf[2];														  
            if((g_ucGC9200_RecBuf[0] == 0x8A)&&(g_ucGC9200_RecBuf[1] == 0x05)&&(g_ucGC9200_RecBuf[3] == cs)){
							g_ucGC9200_RcvFlag = 0;
							ucReturn = g_ucGC9200_RecBuf[2]; 
							memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);	
							return ucReturn;							
						}
				  g_ucGC9200_RcvFlag = 0;
				  memset(g_ucGC9200_RecBuf,0,GC9200_RCV_NUM);						
				}				
			
		}		
	}	
	return 0;
}
/*
  scm.ucSendDataTable[38]= CalXor(scm.ucSendDataTable,38);
  scm.ucSendDataTable[39]= CalSum(scm.ucSendDataTable,38);
*/

/*
1、接好电源给治具上电；

2、读取功率计参数（初始电压AC250V），电源空载功耗是否合格

3、等待组网，设定电子负载CV xxV,ON

4、读取功率计，电子负载参数，电源参数（AC250V/100%）------------ 第一组数据

5、切换输入电压至AC110V ，电源调光50%，读取功率计参数，电源参数；--第二组数据

6、计算输入电压，电流，功率和输出电压，电流 的斜率，截距，完成校正；

7、电源调光100%，设定电子负载短路，读取功率计参数；

8、设定电子负载CV xxV,ON，电源满载，读取功率计参数和电子负载参数，判断额定功率是否合格；

9、切换输入电压至AC220V，电源调光70%，读取功率计参数和电子负载参数 与电源参数对比；
*/

/*****************
1、功率计出错
2、电子负载出错
3、组网超时
4、DPS无数据返回
5、
ScmPM9805TypeDef      PM9805Value;    //
ScmPM9805TypeDef      PM9805Value_1;  //
ScmPM9805TypeDef      PM9805Value_2;   //

ScmCorrectionTypeDef  DPS_WritePara;   // 写入DPS的斜率和截距

ScmReadParaTypeDef    DPS_ReadPara;    // 读Fre, AD
ScmReadParaTypeDef    DPS_ReadPara_1;  // 读Fre, AD
ScmReadParaTypeDef    DPS_ReadPara_2;  // 读Fre, AD

ScmDPSTypeDef         DPS_InquirePara; // 查询回来的参数 
******************/
#define INIT_S    0
#define UNLOAD_S  1
#define NETWORK_S 2
#define FIRST_S   3
#define SECOND_S  4
#define SHORT_S   5
#define RATED_S   6
#define CHECK_S   7
#define SETID_S   8
#define PRINT_S   9
#define END_S     10

//#define SET_ID_S
//char cTempTable[10] = {0x00};
extern unsigned char g_ucBeepOnCnt;
extern OS_TCB Led0TaskTCB;
void CorrectionTask(void)
{
    OS_ERR err;
    static unsigned char ucState = 0;
	  static unsigned char s_ucFinishFlag = 1; //完成后置0
	  float fTemp =0;
	  unsigned char ucTempTable[16] = {0x00};
		char cPrintTable[16] = {0x00};
		unsigned char i = 0;
		unsigned char ucDataL;
		unsigned short int uiDataH;
		unsigned char ucWait = 1;
		unsigned char ucTemp = 0;
		static unsigned int uiTempID = 0;  //电源ID
		union 
    {
	    unsigned int  uiValue;  
      unsigned char ucDataTable[4];
    }UnData;
		
    switch(ucState){
			case INIT_S:
				DisplayString(2,1,"设置电子负载");
			  if(ScmDPS_Para.ucTestMode == MODE_1){//校正测试
				  DisplayString(4,3,"进度:1/9");	
				}else{
				  DisplayString(4,3,"进度:1/7");
				}
				if(IT8512B_SetMode(REMOTE)){    //电子负载设置为远程控制模式
				  if(IT8512B_SetMode(CV_MODE)){ //设置为恒压模式
						//if(IT8512B_SetVol(48000)){
						  if(IT8512B_SetVol(ScmDPS_Para.uiRateVol*1000)){// 设置对应型号LED电源电压
							IT8512B_SetMode(SET_OFF);                      // 电子负载设置OFF 测试电源空载功耗                    
							OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err);// delay 2s
							ucState = UNLOAD_S;
							return;													
            }							
					}
				}
				DisplayString(2,1,"设置电子负载失败");
				ucState = END_S;
			  break;	
			case UNLOAD_S: // 
				DisplayString(2,1,"测试电源空载功耗");
			  if(ScmDPS_Para.ucTestMode == MODE_1){//裸板测试
				  DisplayString(4,3,"进度:2/9");	
				}else{
				  DisplayString(4,3,"进度:2/7");
				}			
				if(ReadPM9805Parameter(&PM9805Value)){ // 读取功率计参数  
				  //sprintf(cTempTable,"%4.1f",PM9805Value.fPower);	
//					uiDataH = (unsigned short int)PM9805Value.fPower;
//					ucDataL = (unsigned char)((PM9805Value.fPower - uiDataH)*10);
//					sprintf(cTempTable,"%3d.%1dw",uiDataH,ucDataL);
//					DisplayString(2,1,cTempTable);
					if(PM9805Value.fPower < 5){ //电源空载功耗小于5W才算合格
            ucState = NETWORK_S;
						break;
					}else{
						DisplayString(2,1,"电源空载功耗过高");
						DisplayString(3,1,"错误代码 18");
					  ucState = END_S;
						break;
					}						
				}else{
				  DisplayString(2,1,"读取功率计失败  ");
					ucState = END_S;
				}
				break;
			case NETWORK_S:
				DisplayString(2,1,"正在组网中请等待");
			  if(ScmDPS_Para.ucTestMode == MODE_1){ //校正测试
				  DisplayString(4,3,"进度:3/9");	
				}else{
				  DisplayString(4,3,"进度:3/7");
				}			
			  if(IT8512B_SetMode(SET_ON) == 0){ //turn on IT8512 fail
					  DisplayString(2,1,"打开电子负载失败");
				    ucState = END_S;
				    return;
				}
			  if (GC9200GetNetworkNum()) {     //读组网到的个数
					DisplayString(2,1,"组网成功        ");
				  if(GC9200GetNetworkID(&DPS_WritePara)){	//读取组网节点						
						sprintf(cPrintTable,"ID:%08X",DPS_WritePara.uiID);//
				    DisplayString(3,1,cPrintTable);
						ucState = FIRST_S;
						if(ScmDPS_Para.ucTestMode == MODE_1){ 
						  ucTemp = ActivateDPS(MODE_1,DPS_WritePara.uiID,ScmDPS_Para.ucDeviceCode,&ScmDPS_Version); //激活查询电源 
						}else if(ScmDPS_Para.ucTestMode == MODE_2){
						  ucTemp = ActivateDPS(MODE_2,DPS_WritePara.uiID,ScmDPS_Para.ucDeviceCode,&ScmDPS_Version); //激活查询电源 
						}	
						  if (ucTemp == 1) {
						    DisplayString(2,1,"设备码不对");
							  sprintf(cPrintTable,"0x%02X",ScmDPS_Version.ucDeviceCode);					
					      DisplayString(2,6,cPrintTable);
							  DisplayString(3,1,"错误代码 26");
							  ucState = END_S;
						  }else if (ucTemp == 2) { //成功
							  ucState = FIRST_S;     //跳至下一步
							  break;
						  }else{													
						    DisplayString(2,1,"激活电源失败    ");
							  DisplayString(3,1,"错误代码 25");
							  ucState = END_S;
						  }					  
					}						
				}else{					
				  DisplayString(2,1,"组网超时        ");
					DisplayString(3,1,"错误代码 13");
          ucState = END_S;
				}			
				break;
			case FIRST_S:			
				if(ScmDPS_Para.ucTestMode == MODE_2){ //出厂测试 不校正--直接
					  //ucState = CHECK_S;
					  ucState = SHORT_S;
				    break;
				}
				DisplayString(4,3,"进度:4/9");
				
				if(DPS_SetDim(DPS_WritePara.uiID,0x64) == 0){ //调光50%
					DisplayString(2,1,"电源调光50% 失败");
					DisplayString(3,1,"错误代码 21");
				  ucState = END_S;
				  return;					
				}				
				
				DisplayString(2,1,"读电源第一组数据");
				OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);// 适当延时 减少载波机对功率计的影响				
//				if(ReadPM9805Parameter(&PM9805Value_1) == 0){ //read input parameter
//					DisplayString(2,1,"读取功率计失败  ");
//				  ucState = END_S;
//				  return;
//				}
				
				if(IT8512B_RD_Parameter(&ScmIT8512_1) == 0){ //read output parameter
					DisplayString(2,1,"读取电子负载失败");
				  ucState = END_S;
				  return;				
				}
				
/****************两点校正*******************/				
				if(ReadDPSPara(DPS_WritePara.uiID,&DPS_ReadPara_1) == 0){//read fre vol
					DisplayString(2,1,"读取电源参数失败");
					DisplayString(3,1,"错误代码 21");
				  ucState = END_S;
				  return;		
				}
				ucState = SECOND_S;
/****************单点校正****************/				
//        ucTemp = SendInstrumentDataToNode(DPS_WritePara.uiID);
//				if(ucTemp == 0x01){//写入成功
//				  ucState = SHORT_S;					
//				}else{				
//					DisplayString(2,1,"写入校正参数失败");
//					DisplayString(3,1,"错误代码 7 ");
//					ucState = END_S;
//					return;
//				}
/*****************************/				
				break;
			case SECOND_S:
				DisplayString(2,1,"读电源第二组数据");
			  DisplayString(4,3,"进度:5/9");
//				SELECT_110V();
//			  OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
			  if(DPS_SetDim(DPS_WritePara.uiID,0xc8) == 0){ //调光100%
					DisplayString(2,1,"电源调光100% 失败");
					DisplayString(3,1,"错误代码 21");
				  ucState = END_S;
				  return;					
				}
				OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err); // 延时3s	
				if(ReadPM9805Parameter(&PM9805Value_2) == 0){ // read input parameter
					DisplayString(2,1,"读取功率计失败  ");
				  ucState = END_S;
				  return;
				}
				if(IT8512B_RD_Parameter(&ScmIT8512_2) == 0){  // read output parameter
					DisplayString(2,1,"读取电子负载失败");
				  ucState = END_S;
				  return;				
				}
				if(ReadDPSPara(DPS_WritePara.uiID,&DPS_ReadPara_2) == 0){ // read fre vol 
					DisplayString(2,1,"读取电源参数失败");
					DisplayString(3,1,"错误代码 21");
				  ucState = END_S;
				  return;		
				}	
        CalculatePara(PM9805Value_1,PM9805Value_2,
				              ScmIT8512_1,ScmIT8512_2,	                 
	                    DPS_ReadPara_1,DPS_ReadPara_2,&DPS_WritePara);  // Calculate slope and intercept, Save to DPS_WritePara
				if(WriteCorrectionPara(DPS_WritePara)==0){
					DisplayString(2,1,"写入校正参数失败");
					DisplayString(3,1,"错误代码 7 ");
				  ucState = END_S;
				  return;					
				}
				ucState = SHORT_S;
				break;
			case SHORT_S: 
				DisplayString(2,1,"短路测试中      ");
			  if(ScmDPS_Para.ucTestMode == MODE_1){//校正模式
				  DisplayString(4,3,"进度:6/9");	
				}else{
				  DisplayString(4,3,"进度:4/7");
				}			
				SELECT_220V();
				OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); // 延时100ms	
//			  if(DPS_SetDim(DPS_WritePara.uiID,0xc8) == 0){ //由于上一步已是100%
//					DisplayString(2,1,"调光100%失败    ");
//					DisplayString(3,1,"错误代码 21 ");
//				  ucState = END_S;
//				  return;					
//				}				
 //       OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); // 延时100ms	
			  if(IT8512B_SetMode(SHORT_MODE)  == 0){    // 电子负载设置短路模式
				  DisplayString(2,1,"设置电子负载失败");
					ucState = END_S;
					return;
				} 
				OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); // 短路延时1s	
			  if(IT8512B_SetMode(FIXED_MODE) == 0){     // 去短路
				  DisplayString(2,1,"设置电子负载失败");
					ucState = END_S;
					return;
				}	
        OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err); // 延时3s					
				if(IT8512B_RD_Parameter(&ScmIT8512_1) == 0){ //read output parameter
					DisplayString(2,1,"读取电子负载失败");
				  ucState = END_S;
				  return;				
				}
				if((ScmIT8512_1.fVoltage <= (ScmDPS_Para.uiRateVol-1))||(ScmIT8512_1.fVoltage >= (ScmDPS_Para.uiRateVol+1))){
				  DisplayString(2,1,"电源短路测试失败");
					DisplayString(3,1,"错误代码 24 ");
					ucState = END_S;
					return;
				}	
				ucState = RATED_S;
				break;			
			case RATED_S: 
				DisplayString(2,1,"测试电源额定功率");
				if(ScmDPS_Para.ucTestMode == MODE_1){//校正模式
				  DisplayString(4,3,"进度:7/9");	
				}else{
				  DisplayString(4,3,"进度:5/7");
				}
				OSTimeDlyHMSM(0,0,0,600,OS_OPT_TIME_HMSM_STRICT,&err); // 延时600ms	
			  fTemp =  fabs(ScmIT8512_1.fPower - ScmDPS_Para.uiRatePow);
				if(fTemp > ScmDPSErrVal.fOutputPow){
				  DisplayString(2,1,"电源额定功率超标");
					DisplayString(3,1,"错误代码 22 ");
					
					sprintf(cPrintTable,"%4.1fW",fTemp);
					DisplayString(4,3,"误差");
					DisplayString(4,5,cPrintTable);
					
					ucState = END_S;
					break;
				}	
				ucState = CHECK_S;
				break;	
			case CHECK_S:
				SELECT_220V();
			  OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
			  DisplayString(2,1,"复测电源请等待  ");
				if(ScmDPS_Para.ucTestMode == MODE_1){//校正模式
				  DisplayString(4,3,"进度:8/9");	
				}else{
				  DisplayString(4,3,"进度:6/7");
				}			
			  if(DPS_SetDim(DPS_WritePara.uiID,0x8c) == 0){//调光70%
					DisplayString(2,1,"调光70% 失败    ");
					DisplayString(3,1,"错误代码 21 ");
				  ucState = END_S;
				  return;					
				}
				OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);// delay 4s
				//ReadPM9805Parameter(&PM9805Value);
				//IT8512B_RD_Parameter(&ScmIT8512_1);	
				if(IT8512B_RD_Parameter(&ScmIT8512_1) == 0){ //read output parameter
					DisplayString(2,1,"读取电子负载失败");
				  ucState = END_S;
				  break;				
				}	
				if(ReadPM9805Parameter(&PM9805Value) == 0){ // read input parameter
					DisplayString(2,1,"读取功率计失败  ");
				  ucState = END_S;
				  break;
				}
				
				if(InquireDPSPara(DPS_WritePara.uiID,&DPS_InquirePara)==0){ //查询电源
					DisplayString(2,1,"读取电源参数失败");
					ucState = END_S;
					break;
				}
				fTemp = fabs(DPS_InquirePara.fI_Voltage - PM9805Value.fVoltage);			
				if(fTemp > ScmDPSErrVal.fInputVol){
				  DisplayString(2,1,"输入电压不合格");
					DisplayString(3,1,"错误代码  8");
					sprintf(cPrintTable,"%4.1fV",fTemp);
					DisplayString(4,3,"误差");
					DisplayString(4,5,cPrintTable);
					ucState = END_S;
					break;
				}
				fTemp = fabs(DPS_InquirePara.fI_Current - PM9805Value.fCurrent);			
				if(fTemp > ScmDPSErrVal.fInputCur){
				  DisplayString(2,1,"输入电流不合格");
					DisplayString(3,1,"错误代码  9");
					sprintf(cPrintTable,"%4.3fA",fTemp);
					DisplayString(4,3,"误差");
					DisplayString(4,5,cPrintTable);
					ucState = END_S;
					break;
				}	
				fTemp = fabs(DPS_InquirePara.fPower - PM9805Value.fPower);			
				if(fTemp > ScmDPSErrVal.fInputPow){
				  DisplayString(2,1,"输入功率不合格");
					DisplayString(3,1,"错误代码 10");
					sprintf(cPrintTable,"%4.1fW",fTemp);
					DisplayString(4,3,"误差");
					DisplayString(4,5,cPrintTable);
					ucState = END_S;
					break;
				}	
				
				if(PM9805Value.fPowerFactory < ScmDPSErrVal.fPF ){
				  DisplayString(2,1,"功率因素偏低");
					DisplayString(3,1,"错误代码 23");
					sprintf(cPrintTable,"%3.2f%%",PM9805Value.fPowerFactory);
					DisplayString(4,3,"PF值");
					DisplayString(4,5,cPrintTable);
					ucState = END_S;
					break;
				}
				
				
				fTemp = fabs(DPS_InquirePara.fO_Voltage - ScmIT8512_1.fVoltage);	//		
				if(fTemp > ScmDPSErrVal.fOutputVol){
				  DisplayString(2,1,"输出电压不合格");
					DisplayString(3,1,"错误代码 25");
					sprintf(cPrintTable,"%4.1fV",fTemp);
					DisplayString(4,3,"误差");
					DisplayString(4,5,cPrintTable);
					ucState = END_S;
					break;
				}	
				fTemp = fabs(DPS_InquirePara.fO_Current - ScmIT8512_1.fCurrent);			
				if(fTemp > ScmDPSErrVal.fOutputCur){
				  DisplayString(2,1,"输出电流不合格");
					DisplayString(3,1,"错误代码 26");
					sprintf(cPrintTable,"%4.3fA",fTemp);
					DisplayString(4,3,"误差");
					DisplayString(4,5,cPrintTable);
					ucState = END_S;
					break;
				}
//        if(ScmDPS_Para.ucTestMode == MODE_1	)			
//				  DisplayString(2,1,"复测合格  结束");
				ucState = SETID_S;
				break;
			case SETID_S:				
				if(ScmDPS_Para.ucTestMode == MODE_2){//出厂测试不再设置ID
					  ucState = PRINT_S;
				    break;
				}
				DisplayString(4,3,"进度:9/9");
				if(DPS_WritePara.uiID != 0x01000001){         
				  KeyFlush();
					DisplayString(2,1,"非默认ID是否设置");
				  while(ucWait){
					  if(KeyHit() > 0){
	            switch(KeyGet()){   // get key value		  
				       case ENTER:								 
							   ucWait  = 0;
								 break;
							 case ESC:
								 ucState = END_S;
							   DisplayString(2,1,"测试结束  请关机");
							   return;
                 break;								 
							 }
						}
						g_ucBeepOnCnt = 0;
					  OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); // 200ms	
					}
				}
				DisplayString(2,1,"设置ID中请等待  ");
				if(SetDPS_ID_First(DPS_WritePara.uiID) == 0){//不成功
				  DisplayString(2,1,"设置DPS ID失败  ");
					ucState = END_S;
					break;
				}			
				//if(SetDPS_ID(DPS_WritePara.uiID,ScmAT24CxxIdData.uiCurentID)== 0){ //设置不成功  ScmAT24CxxIdData.uiCurentID 
				if(SetDPS_ID(DPS_WritePara.uiID,ScmJIG.uiCurentID)== 0){
					DisplayString(2,1,"设置DPS ID失败  ");
					ucState = END_S;
					break;					
				}
				if(SetDPS_ID_Third(DPS_WritePara.uiID) == 0){// 不成功
				  DisplayString(2,1,"设置DPS ID失败  ");
					ucState = END_S;
					break;
				}	
				uiTempID =  ScmJIG.uiCurentID;
				sprintf(cPrintTable,"ID:%08X",ScmJIG.uiCurentID);//
				DisplayString(3,1,cPrintTable);
				ScmJIG.uiCurentID += 0x01;
				//ScmAT24CxxIdData.uiCurentID += 0x01;
				//IdScm2Array(ScmAT24CxxIdData,&ucTempTable[0]);
				//AT24CXX_Write(0,ucTempTable,15);
				UnData.uiValue = ScmJIG.uiCurentID;
				for(i = 0;i < 3;i++){
				  AT24CXX_Write(CUR_ID_ADD,UnData.ucDataTable,4);
				  OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_HMSM_STRICT,&err); // 50ms	
				  AT24CXX_Read(CUR_ID_ADD,UnData.ucDataTable,4);//读取当前ID 
				  if(UnData.uiValue == ScmJIG.uiCurentID ){
            DisplayString(2,1,"设置ID成功      ");	
						ucState = PRINT_S;
					  return;
				  }								
				}
				DisplayString(2,1,"保存ID失败      ");
				ucState = END_S;																
				break;
			case PRINT_S:
				if(ScmDPS_Para.ucTestMode == MODE_1){//裸板测试不打印
					  GP9034T_PrintOneData(uiTempID,ScmJIG.ucDeviceCode,ScmJIG.ucYear,ScmJIG.ucmonth);//
					  DisplayString(2,1,"测试PASS! 请关机");
					  ucState = END_S;
				    break;
				}	
        DisplayString(4,3,"进度:7/7");				
				//GP9034T_PrintData(DPS_WritePara.uiID,ScmAT24CxxIdData.ucDeviceCode);
				GP9034T_PrintData(DPS_WritePara.uiID,ScmJIG.ucDeviceCode,ScmJIG.ucYear,ScmJIG.ucmonth);
				
				DisplayString(2,1,"测试PASS! 请关机");
				ucState = END_S;
        break;
			case END_S:				
			  if(s_ucFinishFlag > 0){				
				  s_ucFinishFlag = 0;
					OSTaskSuspend((OS_TCB*)&Led0TaskTCB,&err);//将校正函数挂起
					IT8512B_SetMode(SET_OFF);
				}
				g_ucBeepOnCnt = 0;
			  LED2_TOGGLE();
			  LED3_TOGGLE();
				break;
			default:
				break;			
		}
}


/*            KeyFlush();
							while(1){
								if(KeyHit() > 0){
									KeyGet();           // Read key value																		
								}	
								OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //延时100ms							
							}	
*/









