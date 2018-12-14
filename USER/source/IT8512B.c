
#include "stm32f10x.h"
#include "delay.h"
#include "System_Initialize.h"
#include "main.h"
#include "STM32_UART.h"
#include <string.h>
#include "includes.h"
#include "TaskApp.h"
#include "IT8512B.h"

//#define REMOTE  0x01
//#define SET_OFF 0x02
//#define SET_ON  0x03
//#define CV_MODE 0x04
//#define RD_CV_MODE     0x05
//#define SET_VOL        0x06
//#define RD_SET_VOL      0x07
//#define SHORT_MODE     0x08
//#define RD_SHORT_MODE  0x09
//#define FIXED_MODE     0x0a
//#define RD_PARA        0x0b

extern unsigned char  g_ucPM9805_RcvTime ; 
extern unsigned char  g_ucIT8512_RcvTime ;
extern unsigned char  g_ucGP9034_RcvTime ;
extern unsigned char  g_ucGC9200_RcvTime ;

extern unsigned char  g_ucPM9805_RcvFlag ; 
extern unsigned char  g_ucIT8512_RcvFlag ;
extern unsigned char  g_ucGP9034_RcvFlag ;
extern unsigned char  g_ucGC9200_RcvFlag ;

extern unsigned char g_ucPM9805_RecBuf[PM9805_RCV_NUM];
extern unsigned char g_ucIT8512_RecBuf[IT8512_RCV_NUM];
extern unsigned char g_ucGP9034_RecBuf[GP9034_RCV_NUM];
extern unsigned char g_ucGC9200_RecBuf[GC9200_RCV_NUM];

ScmIT8512TypeDef ScmIT8512_1; //读两组数据 求slope intercept
ScmIT8512TypeDef ScmIT8512_2;

const unsigned char ucCMD_Table[12][26] = {
0xaa,0x00,0x6a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x14, //0 Read version
0xaa,0x00,0x20,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xcb, //1 Remote mode
0xaa,0x00,0x21,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xcb, //2 OFF
0xaa,0x00,0x21,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xcc, //3 ON
0xaa,0x00,0x28,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd3, //4 Set CV Mode
0xaa,0x00,0x29,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd3, //5 Read Mode
0xaa,0x00,0x2C,0x80,0x3E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x94, //6 Set voltage Bit4-7 16V
0xaa,0x00,0x2D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd7, //7 Read Set voltage Bit4-7
0xaa,0x00,0x5D,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08, //8 set short mode
0xaa,0x00,0x5E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08, //9 Read short mode
0xaa,0x00,0x5D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07, //10 Set Fixed Mode
0xaa,0x00,0x5F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09, //11 Read V(4-7) I(8-11)  P(9-12) 		
};
const unsigned char ucRespondCMD[26] = {
0xaa,0x00,0x12,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3c	
};



/*
aa 00 6a 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 14 //0 Read version
AA 00 6A 38 35 31 32 42 12 01 30 37 30 37 37 31 30 30 34 33 00 00 00 00 00 36

aa 00 20 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 cb //1 Set Remote
AA 00 12 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 3C

aa 00 21 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 cb //2 OFF
AA 00 12 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 3C

aa 00 21 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 cc //3 ON
AA 00 12 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 3C

aa 00 28 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 d3 //4 Set CV Mode
aa 00 12 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 3c 

aa 00 29 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 d3 //5 Read Mode(CC CV CW CR)
AA 00 29 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 D4 

aa 00 2C 80 3E 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 94  //6 Set voltage Bit4-7 16V
AA 00 12 80 3E 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 7A

aa 00 2D 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 d7  //7 Read Set voltage Bit4-7
AA 00 2D 80 3E 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 95 

aa 00 5D 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 08  //8 set short mode
AA 00 12 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 3C

aa 00 5e 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 08  //9 Read short mode
AA 00 5E 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 09 

aa 00 5D 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 07  //10 set Fixed mode
AA 00 12 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 3C

aa 00 5F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 09  //11 Read V(4-7) I(8-11) P(9-12) 	
AA 00 5F 00 00 00 00 05 00 00 00 00 00 00 00 15 80 00 80 00 21 00 00 00 00 44 

*/

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void IT8512B_SendCmd(unsigned char ucCMD )
{
	//IT8512_SendString(ucCMD_Table[ucCMD],26);
  IT8512_SendArray(ucCMD_Table[ucCMD],26);	
}

/*******************************************************************************
* Function Name : 
* Description   : 
*******************************************************************************/
unsigned char CompareData(unsigned char *p1,const unsigned char *p2,unsigned char ucNum)
{
  unsigned char i = 0;
  for(i = 0;i < ucNum;i++){
	  if(p1[i] != p2[i]){
		  return 0;	
		}		
	}	
  return 1;	
}
/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval 0 fail, 1 success
#define REMOTE     0x01
#define SET_OFF    0x02
#define SET_ON     0x03
#define CV_MODE    0x04
#define SHORT_MODE 0x08
#define FIXED_MODE 0x0a
  */
unsigned char IT8512B_SetMode(unsigned char ucCMD) //SHORT_MODE
{
    unsigned char i,j;
	  OS_ERR err;
    for(j = 0;j < 3;j++){
			  memset(g_ucIT8512_RecBuf,0,IT8512_RCV_NUM);
		    g_ucIT8512_RcvFlag = 0;
			 // IT8512_SendString(ucCMD_Table[ucCMD],26);
        IT8512_SendArray(ucCMD_Table[ucCMD],26);
			  for(i = 0;i < 70;i++){    // 70 *15 = 1.05s
				    if(g_ucIT8512_RcvFlag > 0){ //
							  if(CompareData(g_ucIT8512_RecBuf,ucRespondCMD,26)){ //正确响应
								    return 1;
								}	
								g_ucIT8512_RcvFlag = 0;
								memset(g_ucIT8512_RecBuf,0,IT8512_RCV_NUM);
						}							
					
				    OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);	
				}			
		}
    return 0;
}
/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None   RD_CV_MODE 0x05
0xaa,0x00,0x29,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD4
  */
unsigned char IT8512B_RD_CV_Mode(void)
{
    unsigned char i,j;
    const unsigned char ucCV_Respond[26] = {0xaa,0x00,0x29,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
			        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD4};		
	  OS_ERR err;
    for(j = 0;j < 3;j++){
			  memset(g_ucIT8512_RecBuf,0,IT8512_RCV_NUM);
		    g_ucIT8512_RcvFlag = 0;
		    //IT8512_SendString(ucCMD_Table[RD_CV_MODE],26);
			  IT8512_SendArray(ucCMD_Table[RD_CV_MODE],26);
			  for(i = 0;i < 70;i++){
				    if(g_ucIT8512_RcvFlag > 0){ //
							  if(CompareData(g_ucIT8512_RecBuf,ucCV_Respond,26)){ //正确响应
								    return 1;
								}	
								g_ucIT8512_RcvFlag = 0;
								memset(g_ucIT8512_RecBuf,0,IT8512_RCV_NUM);								
						}											
				    OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);	
				}			
		}
    return 0;
}

/**
  * @brief  set voltage Dec unit mV .
  * @param  None
  * @retval None  
#define SET_VOL        0x06
AA 00 12 80 3E 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 7A
  */
unsigned char IT8512B_SetVol(unsigned int uiVolmV )
{
  union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnVol;
	OS_ERR err;
  unsigned char i,j =0,cs = 0;
  unsigned char ucCMD[26] = {0xaa,0x00,0x2C,0x80,0x3E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                           0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x94,};
  unsigned char ucRespondCMD[26] = {0xaa,0x00,0x12,0x80,0x3E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                           0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x94,};	
	UnVol.uiValue = uiVolmV;
	for(i = 0;i < 4;i++){
	  ucCMD[3+i] = UnVol.ucDataTable[i]; 
		ucRespondCMD[3+i]= UnVol.ucDataTable[i];
	}	
	ucRespondCMD[3] = 0x80;
	for(i=0;i<25;i++){
	  cs += ucCMD[i];
		j += ucRespondCMD[i];
	}	
	ucCMD[25] = cs;
	ucRespondCMD[25] = j;
	
	for(i = 0;i < 3;i++){
		memset(g_ucIT8512_RecBuf,0,IT8512_RCV_NUM);
		g_ucIT8512_RcvFlag = 0;
	  IT8512_SendString(ucCMD,26);
		for(j = 0;j < 70;j++){
				if(g_ucIT8512_RcvFlag > 0){ 
					if(CompareData(g_ucIT8512_RecBuf,ucRespondCMD,26)){ //正确响应
						return 1;
					}
					g_ucIT8512_RcvFlag = 0;
					memset(g_ucIT8512_RecBuf,0,IT8512_RCV_NUM);					
				}			
		    OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);	
		}	
	}
	return 0;	
}

/**
  * @brief  
  * @param  None
  * @retval None
RD_SET_VOL      0x07
aa 00 2D 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 d7  // 7 Read Set voltage Bit4-7
AA 00 2D 80 3E 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 95 
  */

unsigned int IT8512B_ReadSetVol(void)
{
  union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnVol;
	OS_ERR err;
  unsigned char i,j = 0,k,n,cs = 0;

	
	for(i = 0;i < 3;i++){
		memset(g_ucIT8512_RecBuf,0,IT8512_RCV_NUM);
		g_ucIT8512_RcvFlag = 0;
	 // IT8512_SendString(ucCMD_Table[RD_SET_VOL],26);
		IT8512_SendArray(ucCMD_Table[RD_SET_VOL],26);
		for(j = 0;j < 70;j++){
				if(g_ucIT8512_RcvFlag > 0){ 
          for(n = 0;n < 25;n++){
					  cs += g_ucIT8512_RecBuf[n];
					}
					if((cs == g_ucIT8512_RecBuf[25])&&(g_ucIT8512_RecBuf[2]==0x2d)){
						for(k = 0;k < 4;k++){
						  UnVol.ucDataTable[k] = g_ucIT8512_RecBuf[3+k];
						}
						return UnVol.uiValue;
					}
					g_ucIT8512_RcvFlag = 0;
					memset(g_ucIT8512_RecBuf,0,IT8512_RCV_NUM);					
				}			
		    OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);	
		}	
	}
  return 0;
}
/**
  * @brief  
  * @param  None
  * @retval None
#define RD_SHORT_MODE  0x09
aa 00 5e 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 08  //9 Read short mode
AA 00 5E 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 09 
  */
unsigned char IT8512B_RD_ShortMode(void)
{
    unsigned char i,j;
    const unsigned char ucCV_Respond[26] = {0xaa,0x00,0x5e,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
			        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09};		
	  OS_ERR err;
    for(j = 0;j < 3;j++){
			  memset(g_ucIT8512_RecBuf,0,IT8512_RCV_NUM);
		    g_ucIT8512_RcvFlag = 0;
		    //IT8512_SendString(ucCMD_Table[RD_SHORT_MODE],26);
			  IT8512_SendArray(ucCMD_Table[RD_SHORT_MODE],26);
			  for(i = 0;i < 70;i++){
				    if(g_ucIT8512_RcvFlag > 0){ //
							  if(CompareData(g_ucIT8512_RecBuf,ucCV_Respond,26)){ //正确响应
								    return 1;
								}	
								g_ucIT8512_RcvFlag = 0;
								memset(g_ucIT8512_RecBuf,0,IT8512_RCV_NUM);								
						}											
				    OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);	
				}			
		}
    return 0;
}
/**
  * @brief  
  * @param  None
  * @retval None
#define RD_PARA        0x0b
aa 00 5F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 09  //11 Read V(4-7) I(8-11) P(12-15) 	
AA 00 5F 00 00 00 00 05 00 00 00 00 00 00 00 15 80 00 80 00 21 00 00 00 00 44 
  */
unsigned char IT8512B_RD_Parameter(ScmIT8512TypeDef *IT8512)
{
  union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnVol;
	OS_ERR err;
  unsigned char i,j =0,k,cs = 0;
	for(i = 0;i < 5;i++){
		memset(g_ucIT8512_RecBuf,0,IT8512_RCV_NUM);
		g_ucIT8512_RcvFlag = 0;
	  //IT8512_SendString(ucCMD_Table[RD_PARA],26);
		IT8512_SendArray(ucCMD_Table[RD_PARA],26);
		for(j=0;j<80;j++){
				if(g_ucIT8512_RcvFlag > 0){ 
          for(k=0;k<25;k++){
					  cs += g_ucIT8512_RecBuf[k];
					}
					if((cs == g_ucIT8512_RecBuf[25])&&(g_ucIT8512_RecBuf[2]==0x5f)){
						for(k = 0;k < 4;k++){
						  UnVol.ucDataTable[k] = g_ucIT8512_RecBuf[3+k];
						}
						IT8512->fVoltage = (float)UnVol.uiValue/1000;
						for(k = 0;k < 4;k++){
						  UnVol.ucDataTable[k] = g_ucIT8512_RecBuf[7+k];
						}
						IT8512->fCurrent = (float)UnVol.uiValue/10000;		
						for(k = 0;k < 4;k++){
						  UnVol.ucDataTable[k] = g_ucIT8512_RecBuf[11+k];
						}
						IT8512->fPower = (float)UnVol.uiValue/1000;						
						return 1;
					}
					g_ucIT8512_RcvFlag = 0;
					memset(g_ucIT8512_RecBuf,0,IT8512_RCV_NUM);					
				}			
		    OSTimeDlyHMSM(0,0,0,15,OS_OPT_TIME_HMSM_STRICT,&err);	
		}	
	}
  return 0;
}

/*
20H  设置负载的操作模式 
21H  设置负载的输出状态(on/off)  
22H  设置负载的最大输入电压值 
23H  读取负载的最大输入电压设置值 
24H  设置负载的最大输入电流值 
25H  读取负载的最大输入电流设置值 
26H  设置负载的最大输入功率值 
27H  读取负载的最大输入功率设置值 
28H  设置负载的操作模式(CC, CV, CW, CR) 
29H  读取负载的操作模式 
2AH  设置负载的定电流值 
2BH  读取负载的定电流值 
2CH  设置负载的定电压值 
2DH  读取负载的定电压值 
2EH  设置负载的定功率值 
2FH  读取负载的定功率值 
30H  设置负载的定电阻值 
31H  读取负载的定电阻值 
32H  设置负载的动态定电流参数 
33H  读取负载的动态定电流参数 
34H  设置负载的动态定电压参数 
35H  读取负载的动态定电压参数 
36H  设置负载的动态定功率参数 
37H  读取负载的动态定功率参数 
38H  设置负载的动态定电阻参数 
39H  读取负载的动态定电阻参数 
3AH  设置负载的LIST操作模式(CC, CV, CW, CR) 
3BH  读取负载的LIST操作模式 (CC, CV, CW, CR) 
3CH  设置负载的LIST循环模式(ONCE, REPEAT) 
3DH  读取负载的LIST循环模式(ONCE, REPEAT) 
3EH  设置负载的LIST步数 
3FH  读取负载的LIST步数 
40H  设置负载的相应单步的电流值及时间值 
41H  读取负载的相应单步的电流值及时间值 
42H  设置负载的相应单步的电压值及时间值 
43H  读取负载的相应单步的电压值及时间值 
45H  读取负载的相应单步的功率值及时间值 
46H  设置负载的相应单步的电阻值及时间值   
47H  读取负载的相应单步的电阻值及时间值 
48H  设置负载的LIST文件名 
49H  读取负载的LIST文件名 
4AH  设置负载的LIST储存区的划分模式 
4BH  读取负载的LIST储存区的划分模式 
4CH  保存负载的LIST文件到指定的存储区 
4DH  从指定的负载的LIST文件存储区取出LIST檔 
4EH  设置负载工作在电池测试模式下的最小电压值 
4FH  读取负载工作在电池测试模式下的最小电压值 
50H  设置负载的FOR LOAD ON定时器时间值 
51H  读取负载的FOR LOAD ON定时器时间值 
52H  设置负载的 FOR LOAD ON定时器状态 
53H  读取负载的 FOR LOAD ON定时器状态 
54H  设置负载的新通讯地址 
55H 
56H  设置负载的远程测量模式的状态 
57H  读取负载的远程测量模式的状态 
58H  设置负载的触发模式 
59H  读取负载的触发模式 
5AH  发送给负载一个触发信号 
5BH  保存负载的相关设置到指定的存储区 
5CH  从指定的负载存储区取出已保存的相关设置 
5DH  设置负载的工作模式(FIXED,  SHORT,  TRAN,  LIST, BATTERY) 
5EH  读取负载的工作模式 
5FH  读取负载的输入电压,输入电流,输入功率及相关状态 
60H  设置负载的校准保护状态 
61H  读取负载的校准保护状态 
62H  校准负载的电压点 
63H  返回给负载当前的实际输入电压 
64H  校准负载的电流点 
65H  返回给负载当前的实际输入电流 
66H  保存负载校准数据到EEPROM中,供用户校准时使用 
67H  设置负载的校准信息 
68H  读取负载的校准信息 
69H  恢复校准资料为出厂时的值 
*/
