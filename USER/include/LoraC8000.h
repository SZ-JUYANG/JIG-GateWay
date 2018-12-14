#ifndef __LoraC8000_H
#define __LoraC8000_H

#define TIME_OUT_MAX      150   // ����ID�ȴ���ʱ 200*150 = 30s
#define TIME_RESPONSE_MAX 5     // ��Ӧ��ʱʱ��   200*5 = 1s
#define INTERVAL_MAX      2000  // ���ͼ��

//0xF FFFF 1MBytes
#define FLASH_BACKUP_PAGE       0X080FA000

#define NODE_NUM    3
#define TASK_MAX    NODE_NUM
#define FAIL_ID_MAX NODE_NUM

#define EEPROM_ERR_01 0x01 //��ȡ����ʧ�ܵ�ID��������
#define EEPROM_ERR_02 0x02 //��ȡ����ʧ�ܵ�ID������
#define EEPROM_ERR_03 0x03 //��ȡ��ǰID(δʹ��)����

#define EEPROM_ERR_04 0x04 //д�����ʧ�ܵ�ID��������
#define EEPROM_ERR_05 0x05 //д�����ʧ�ܵ�ID������
#define EEPROM_ERR_06 0x06 //д�뵱ǰID(δʹ��)����

#define EEPROM_ERR_07 0x07
#define EEPROM_ERR_08 0x08

#define EEPROM_ERR_09 0x09 //ID�������


typedef struct {
	unsigned char ucJigID;       // �ξ� Lora ���
	unsigned int uiPLC_ID;       // �ξ� �����PLC ID
	unsigned char ucState;       // oneline/offline
	unsigned char ucTaskType;    // 

	unsigned char ucTimeOutCnt;  // ��ʱ����
	unsigned char ucTimeOutFlag; // ��ʱ��־
	unsigned char ucRsponseTimeCnt;  //��Ӧ����
	unsigned char ucRsponseType;     //��Ӧ����
} ScmTask;



typedef struct {
    unsigned char ucJigID;    // �ξ� Lora ���
    unsigned int ucState;     // 0 �ޣ�1 ����ID , 2 ȷ�Ϸ���
	  //unsigned char ucTimeOutCnt;  //��ʱ����
} ScmResponseTask;


void DataInit(void);
void TimeOutCounter(void);
unsigned char ReadRegisterNodeID(void);
void C8000SendData(unsigned char ucNodeID,unsigned char *pucInputData,unsigned char ucDataLen);
void CheckReceiveData(unsigned char *pucInputData,unsigned char ucDataLen);
void TaskOperation(void);
void SaveFailID_ToEEPROM(void);
unsigned char  SaveDadaToEEPROM(unsigned short int uiStartAdd,unsigned char *pucData,unsigned char ucLen);
#endif
