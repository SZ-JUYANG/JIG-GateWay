#ifndef __LoraC8000_H
#define __LoraC8000_H

#define TIME_OUT_MAX      150   // 分配ID等待溢时 200*150 = 30s
#define TIME_RESPONSE_MAX 5     // 响应延时时间   200*5 = 1s
#define INTERVAL_MAX      2000  // 发送间隔

//0xF FFFF 1MBytes
#define FLASH_BACKUP_PAGE       0X080FA000

#define NODE_NUM    3
#define TASK_MAX    NODE_NUM
#define FAIL_ID_MAX NODE_NUM

#define EEPROM_ERR_01 0x01 //读取分配失败的ID数量有误
#define EEPROM_ERR_02 0x02 //读取分配失败的ID号有误
#define EEPROM_ERR_03 0x03 //读取当前ID(未使用)有误

#define EEPROM_ERR_04 0x04 //写入分配失败的ID数量有误
#define EEPROM_ERR_05 0x05 //写入分配失败的ID号有误
#define EEPROM_ERR_06 0x06 //写入当前ID(未使用)有误

#define EEPROM_ERR_07 0x07
#define EEPROM_ERR_08 0x08

#define EEPROM_ERR_09 0x09 //ID分配完成


typedef struct {
	unsigned char ucJigID;       // 治具 Lora 编号
	unsigned int uiPLC_ID;       // 治具 分配的PLC ID
	unsigned char ucState;       // oneline/offline
	unsigned char ucTaskType;    // 

	unsigned char ucTimeOutCnt;  // 溢时计数
	unsigned char ucTimeOutFlag; // 溢时标志
	unsigned char ucRsponseTimeCnt;  //响应计数
	unsigned char ucRsponseType;     //响应类型
} ScmTask;



typedef struct {
    unsigned char ucJigID;    // 治具 Lora 编号
    unsigned int ucState;     // 0 无，1 分配ID , 2 确认分配
	  //unsigned char ucTimeOutCnt;  //溢时计数
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
