/**
  ******************************************************************************
  * @file    main.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   Header for main.c module
  ******************************************************************************
1	 无法读出待测设备ID
2	 待测设备无数据返回
3	 无法读取功率计PM9805的参数
4	 校正输入电压失败
5	 校正输入电流失败
6	 校正输入功率失败
7  待测设备保存校正参数失败
8  输入电压误差过大
9	 输入电流误差过大
10 输入功率误差过大
11 恢复出厂设置失败
12	
13 组网失败
14 分配ID失败
15 未分配ID
16 灯不亮
17	
18 空载功率过大
19 测试治具节点ID使用完，请更换ID段号
20 无法读取电子负载IT8512+的参数
21 调光失败
22 额定功率误差大
23 功率因素偏低
24 短路不恢复
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#define ID_LIMIT_EN          1 /* 网关治具限制ID使用功能使能位：0：关闭；1：使能 */
#if ID_LIMIT_EN == 1
 #define ID_TOTAL            100 /* 允许使用的ID数量 */
 #define ID_END              (ID_START + ID_TOTAL)   /* 结束的ID */
#endif
#define ID_START             0x0100708A
#define WRITE_EEPROM_EN      0 /* 修改存储在EEPROM的ID使能位；0：关闭； 1：修改ID； 2：修改FLASH存储的修改时间为0 */
#if WRITE_EEPROM_EN == 1            
  /* 生成修改ID的烧录文件时，填写当时的时间 */
	#define MODIFY_TIME_YEAR           0x2019   /* 年：   0Xyyyy */
	#define MODIFY_TIME_MONTH_DATE     0x0221   /* 月日：0xmmdd */
	#define MODIFY_TIME_HOUR_MIN       0x1504   /* 时分：0xhhmm */
#endif

#define ID_RECYCLE_EN        0 /* 分配失败的ID回收功能使能位： 0：关闭；1：使能 */
/* Includes ------------------------------------------------------------------*/

//#include "stm32f10x.h"
#define UART_DEBUG 1

#define RCV_MAX_TIME   10 // 1->1ms
#define PM9805_RCV_NUM 30 // 接收最长字节个数
#define IT8512_RCV_NUM 30
#define GP9034_RCV_NUM 30
#define GC9200_RCV_NUM 40
#define C8000_RCV_NUM  20
#define C6000_RCV_NUM  20

#define BEEP_ON_TIME 200 // 按键按一次响100ms
/******************LED治具参数*********************************/
#define AT_ID_ADD   0x00 // 存储在AT24 EEPROM ID地址
#define AT_ERR_ADD  0x10 // 存储在AT24 EEPROM 误差系数地址

#define DEVICE_ADD  0x01 //存储设备码地址
#define CUR_ID_ADD  0x02 //存储当前电源ID
#define YEAR_ADD    0x06 //存储年
#define MONTH_ADD   0x07 //存储月
#define MODE_ADD    0x2A //存储测试模式

#define MODE_1      0x01 //裸板测试
#define MODE_2      0x02 //出厂测试

/******************治具集中器*********************************/
/******************
*A5 L Data cs 5A ,L = Length(Data),CS = Sum(L + Data)
*
*1. CurrentID -> (Data -> CurrentID) A5 04 01 00 00 02 07 5A
*2. FailID    -> (Data -> 4N)
*3. used ID Num ->(Data -> N)
********************/
#define ID_USED_NUM_ADD     0x01  //Start ADD 8Bytes
#define PLC_CURRENT_ID_ADD  0x09  //Start ADD 8Bytes

#define PLC_FAIL_ID_NUM_ADD 0x11  //Start ADD 5Bytes
#define PLC_FAIL_ID_ADD     0x16  //Start ADD 


/************************************************/
typedef struct  //电源的规格型号: 功率/电压/设备码
{
  unsigned char  ucTestMode;
	unsigned short int uiRatePow; 
  unsigned int uiRateVol; 
  unsigned char  ucDeviceCode;
}ScmDPS_PNTypeDef;

typedef struct 
{
  unsigned char ucDeviceCode;    // 设备码
	unsigned int  uiStartID;       // 起始ID
  unsigned int  uiCurentID;      // 当前ID（下一个已使用）
	unsigned short int 	uiIDnumber;// ID总数量
  unsigned short int  uiRestID;  // ID剩余数量	
}ScmAT24CxxTypeDef;

typedef struct 
{
  unsigned char ucDeviceCode;     // 设备码
	unsigned int  uiStartID;        // 起始ID
  unsigned int  uiCurentID;       // 当前ID（下一个使用）
	unsigned short int 	uiSegmentID;// ID段
  unsigned short int  uiRestID;   // ID剩余数量
	unsigned char ucYear;
	unsigned char ucmonth;
	unsigned char ucJIG_ID;
	unsigned char ucTestMode; //1:裸板测试；2：出厂测试
}ScmJIGTypeDef;

typedef struct 
{
  unsigned char ucDeviceCode;    // 设备码
	unsigned char ucHardVer;       // 硬件版本
  unsigned short int  uiSotfVer; // 软件版本
	unsigned char ucActiveState;
}ScmDPSInfoTypeDef;

typedef struct //误差系数 
{
  float fInputVol;    
  float fInputCur; 
  float fInputPow; 
  float fOutputVol;    
  float fOutputCur; 
  float fOutputPow;
 	float fPF;
}ScmAT24CxxErrTypeDef;

typedef struct //读取电源信息
{
	unsigned char ucDeviceCode;    // 设备码
	unsigned char ucHardware;      // 软件版本
	unsigned short int uifirmware; // 硬件版本
	unsigned char ucActivation;    // 激活状态	
}ScmDPS_VersionTypeDef;

//typedef struct //误差的最大最小值
//{
//  float fInputVolmMin;
//  float fInputVolMax;	
//  float fInputCurMin;
//  float fInputCurMax;	
//  float fInputPowMin;
//  float fInputPowMax;	
//  float fOutputVolMin;
//  float fOutputVolMax;	
//  float fOutputCurMin;
//  float fOutputCurMax;	
//  float fOutputPowMin; 
//  float fOutputPowMax;	
//}ScmErrTypeDef;
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

//const unsigned char PM9805_ReadPara[4]={0x55, 0x00, 0x10, 0x65}; 

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
