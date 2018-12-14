#ifndef __MENU_H
#define __MENU_H

//#define  KEY_SCANT_MAX_TIME 20
//#define  LED_MAX_TIME       500
//#define  BEEPER_MAX_TIME    10     //10ms
//#define  LIGHT_MAX_TIME     10000  //10s  turn off light
//#define  HOME_MAX_TIME      60000  //1min return home page
#define  FUNC_MAX_TIME      500    //0.5s

//#define CONFIG  0x01
//#define CONTROL 0x02
//#define INFO    0x04
//#define RECORD  0x08
//#define WARNING 0x10
//#define BACK    0x20

#define SIZE_OF_KEYBD_MENU 60 //菜单总长度
typedef struct
{
  unsigned char ucKeyStateIndex;  //当前状态索引号
//  unsigned char ucKeyLeftState;   //按下“向左”键时转向的状态索引号	
  unsigned char ucKeyUpState;     //按下“向上”键时转向的状态索引号
  unsigned char ucKeyEnterState;  //按下“确定”键时转向的状态索引号
	unsigned char ucKeyBackState;    //  
  unsigned char ucKeyDownState;   //按下“向下”键时转向的状态索引号
//  unsigned char ucKeyRightState;  //按下“向右”键时转向的状态索引号
	//unsigned char ucKeyEnterLongState; //长按下“确定”键时转向的状态索引号

  void (*CurrentOperate)();      //当前状态应该执行的功能操作
} ScmKbdTabStruct;


//typedef struct{                                         // 电表数据 --9
//     unsigned short int uiA_Voltage;   //*0.1V 2Bs
//     unsigned short int uiB_Voltage;         
//     unsigned short int uiC_Voltage;           
//	
//     unsigned short int uiA_Current;   //*1mA  2Bs        
//     unsigned short int uiB_Current;             
//     unsigned short int uiC_Current;            

//     unsigned short int uiA_Power;      //*0.1W   2Bs      
//     unsigned short int uiB_Power;            
//     unsigned short int uiC_Power;  
//     unsigned short int uiAll_Power;       

//     unsigned char ucA_PF;  //*0.01  1B    
//     unsigned char ucB_PF;             
//     unsigned char ucC_PF;   
//     unsigned char ucAll_PF; 
//	   unsigned long ulWattHour;   // 0.1kWh	
//}ScmMeterStruct;

void KeyOperation(void);
void HomePage(void);

//unsigned char* MenuTaskRequireBufOut(void);
//void MenuTaskRequireBufIn (unsigned char *pucTempBuf);
//unsigned char MenuTaskRequireFlag(void);
//unsigned char* MenuTaskAckBufOut(void);
//void MenuTaskAckBufIn (unsigned char *pucTempBuf);
//unsigned char MenuTaskAckFlag(void);
#endif
