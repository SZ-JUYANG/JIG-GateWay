#ifndef _TaskApp_H_
#define _TaskApp_H_
#include"main.h"

typedef struct 
{
	float fVoltage;  
  float fCurrent;
	float fPower;
	float fPowerFactory;
}ScmPM9805TypeDef;

typedef struct 
{
  char cTemperature;
	unsigned char ucDeviceCode;
	float fO_Voltage;  
  float fO_Current;
	float fI_Voltage;  
  float fI_Current;	
	float fPower;
	float fPowerFactory;
}ScmDPSTypeDef;

typedef union 
{
	float fValue;  
  unsigned char ucDataTable[4];
}UnTypeDef;

typedef struct 
{
  unsigned int uiID;
//	unsigned char ucWR;
//	unsigned char ucCMD;	
	float  fI_PowSlope;
  float  fI_PowIntercept;
	float  fI_VolSlope;
  float  fI_VolIntercept;
	float  fI_CurSlope;
  float  fI_CurIntercept;
	
	float  fO_VolSlope;
	float  fO_VolIntercept;
	float  fO_CurSlope;	
	float  fO_CurIntercept;
	
	float fI_PowerVal;
	float fI_VoltageVal;
	float fI_CurrentVal;
	
	unsigned char ucSendDataTable[50];	
}ScmCorrectionTypeDef;


typedef struct 
{	
	unsigned short int  uiI_PowFre;
	unsigned short int  uiI_VolFre;
	unsigned short int  uiI_CurFre;
	unsigned short int  uiO_VolVal;
	unsigned short int  uiO_CurVal;	
}ScmReadParaTypeDef;


unsigned char ReadPM9805Parameter(ScmPM9805TypeDef *pPM9805);

unsigned char GC9200GetNetworkNum(void);
unsigned char GC9200GetNetworkID(ScmCorrectionTypeDef *scm);

unsigned char InquireDPSPara(unsigned int uiID,ScmDPSTypeDef *pscm);//∂¡»° Inquire

unsigned char ReadDPSPara(unsigned int uiID,ScmReadParaTypeDef *scm); //∂¡»° Fre vol

void CorrectionTask(void);

unsigned char SetFactory(unsigned int uiID);
unsigned char ReadDPSInfo(unsigned int uiID,ScmDPSInfoTypeDef *ScmDPSInfo);
#endif

