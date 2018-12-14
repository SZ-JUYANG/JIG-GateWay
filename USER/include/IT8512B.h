#ifndef _IT8512B_H_
#define _IT8512B_H_


#define REMOTE  0x01
#define SET_OFF 0x02
#define SET_ON  0x03
#define CV_MODE 0x04
#define RD_CV_MODE     0x05
#define SET_VOL        0x06
#define RD_SET_VOL      0x07
#define SHORT_MODE     0x08
#define RD_SHORT_MODE  0x09
#define FIXED_MODE     0x0a
#define RD_PARA        0x0b

typedef struct 
{
	float fVoltage;  
  float fCurrent;
	float fPower;
}ScmIT8512TypeDef;

unsigned char IT8512B_SetMode(unsigned char ucCMD);
unsigned char IT8512B_RD_CV_Mode(void);
unsigned char IT8512B_SetVol(unsigned int uiVolmV );
unsigned int IT8512B_ReadSetVol(void);
unsigned char IT8512B_RD_ShortMode(void);
unsigned char IT8512B_RD_Parameter(ScmIT8512TypeDef *IT8512);

unsigned char CompareData(unsigned char *p1,const unsigned char *p2,unsigned char ucNum);
#endif

