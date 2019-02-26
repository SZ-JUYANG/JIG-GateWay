/**
  ******************************************************************************
  * @file    main.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   Header for main.c module
  ******************************************************************************
1	 �޷����������豸ID
2	 �����豸�����ݷ���
3	 �޷���ȡ���ʼ�PM9805�Ĳ���
4	 У�������ѹʧ��
5	 У���������ʧ��
6	 У�����빦��ʧ��
7  �����豸����У������ʧ��
8  �����ѹ������
9	 �������������
10 ���빦��������
11 �ָ���������ʧ��
12	
13 ����ʧ��
14 ����IDʧ��
15 δ����ID
16 �Ʋ���
17	
18 ���ع��ʹ���
19 �����ξ߽ڵ�IDʹ���꣬�����ID�κ�
20 �޷���ȡ���Ӹ���IT8512+�Ĳ���
21 ����ʧ��
22 ���������
23 ��������ƫ��
24 ��·���ָ�
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#define ID_LIMIT_EN          1 /* �����ξ�����IDʹ�ù���ʹ��λ��0���رգ�1��ʹ�� */
#if ID_LIMIT_EN == 1
 #define ID_TOTAL            100 /* ����ʹ�õ�ID���� */
 #define ID_END              (ID_START + ID_TOTAL)   /* ������ID */
#endif
#define ID_START             0x0100708A
#define WRITE_EEPROM_EN      0 /* �޸Ĵ洢��EEPROM��IDʹ��λ��0���رգ� 1���޸�ID�� 2���޸�FLASH�洢���޸�ʱ��Ϊ0 */
#if WRITE_EEPROM_EN == 1            
  /* �����޸�ID����¼�ļ�ʱ����д��ʱ��ʱ�� */
	#define MODIFY_TIME_YEAR           0x2019   /* �꣺   0Xyyyy */
	#define MODIFY_TIME_MONTH_DATE     0x0221   /* ���գ�0xmmdd */
	#define MODIFY_TIME_HOUR_MIN       0x1504   /* ʱ�֣�0xhhmm */
#endif

#define ID_RECYCLE_EN        0 /* ����ʧ�ܵ�ID���չ���ʹ��λ�� 0���رգ�1��ʹ�� */
/* Includes ------------------------------------------------------------------*/

//#include "stm32f10x.h"
#define UART_DEBUG 1

#define RCV_MAX_TIME   10 // 1->1ms
#define PM9805_RCV_NUM 30 // ������ֽڸ���
#define IT8512_RCV_NUM 30
#define GP9034_RCV_NUM 30
#define GC9200_RCV_NUM 40
#define C8000_RCV_NUM  20
#define C6000_RCV_NUM  20

#define BEEP_ON_TIME 200 // ������һ����100ms
/******************LED�ξ߲���*********************************/
#define AT_ID_ADD   0x00 // �洢��AT24 EEPROM ID��ַ
#define AT_ERR_ADD  0x10 // �洢��AT24 EEPROM ���ϵ����ַ

#define DEVICE_ADD  0x01 //�洢�豸���ַ
#define CUR_ID_ADD  0x02 //�洢��ǰ��ԴID
#define YEAR_ADD    0x06 //�洢��
#define MONTH_ADD   0x07 //�洢��
#define MODE_ADD    0x2A //�洢����ģʽ

#define MODE_1      0x01 //������
#define MODE_2      0x02 //��������

/******************�ξ߼�����*********************************/
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
typedef struct  //��Դ�Ĺ���ͺ�: ����/��ѹ/�豸��
{
  unsigned char  ucTestMode;
	unsigned short int uiRatePow; 
  unsigned int uiRateVol; 
  unsigned char  ucDeviceCode;
}ScmDPS_PNTypeDef;

typedef struct 
{
  unsigned char ucDeviceCode;    // �豸��
	unsigned int  uiStartID;       // ��ʼID
  unsigned int  uiCurentID;      // ��ǰID����һ����ʹ�ã�
	unsigned short int 	uiIDnumber;// ID������
  unsigned short int  uiRestID;  // IDʣ������	
}ScmAT24CxxTypeDef;

typedef struct 
{
  unsigned char ucDeviceCode;     // �豸��
	unsigned int  uiStartID;        // ��ʼID
  unsigned int  uiCurentID;       // ��ǰID����һ��ʹ�ã�
	unsigned short int 	uiSegmentID;// ID��
  unsigned short int  uiRestID;   // IDʣ������
	unsigned char ucYear;
	unsigned char ucmonth;
	unsigned char ucJIG_ID;
	unsigned char ucTestMode; //1:�����ԣ�2����������
}ScmJIGTypeDef;

typedef struct 
{
  unsigned char ucDeviceCode;    // �豸��
	unsigned char ucHardVer;       // Ӳ���汾
  unsigned short int  uiSotfVer; // ����汾
	unsigned char ucActiveState;
}ScmDPSInfoTypeDef;

typedef struct //���ϵ�� 
{
  float fInputVol;    
  float fInputCur; 
  float fInputPow; 
  float fOutputVol;    
  float fOutputCur; 
  float fOutputPow;
 	float fPF;
}ScmAT24CxxErrTypeDef;

typedef struct //��ȡ��Դ��Ϣ
{
	unsigned char ucDeviceCode;    // �豸��
	unsigned char ucHardware;      // ����汾
	unsigned short int uifirmware; // Ӳ���汾
	unsigned char ucActivation;    // ����״̬	
}ScmDPS_VersionTypeDef;

//typedef struct //���������Сֵ
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
