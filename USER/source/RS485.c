#include "STM32_UART.h"
#include "main.h"
#include "LoraC8000.h"
#include "AT24Cxx.h"
#include "System_Initialize.h"
/***************
#define ID_USED_NUM_ADD     0x01  //Start ADD 8Bytes
#define PLC_CURRENT_ID_ADD  0x09  //Start ADD 8Bytes

#define PLC_FAIL_ID_NUM_ADD 0x11  //Start ADD 
#define PLC_FAIL_ID_ADD     0x16  //Start ADD 

1 Read Current ID
2 Read used ID number

3 Read fail id number
4 Read fail ID

1 A1 A1 1A (0x01000001)
2 A1 A2 1A (0x00000000)
3 A1 A3 1A (0x00)
4 A1 A4 1A (4 * num)

5 A1 A5 1A read backup data

PS:A1 A1 A1 default set ID

***************/
extern unsigned char ucBackupDataFlash[2+9+4*FAIL_ID_MAX];


void ReadEepromData(unsigned char *ucpData)
{
	unsigned char ucTemp;  
	unsigned char ucCmd = 0;
	unsigned char ucTempBuf[FAIL_ID_MAX*4+4]= {0};
	unsigned char ucAck[2]={0x68,0x16};
	ucCmd = ucpData[1];
	if((ucpData[0]==0xA1)&&(ucpData[2]==0x1A)){
		if(ucCmd == 0xA1){      //Read Current ID
			AT24CXX_Read(PLC_CURRENT_ID_ADD,ucTempBuf,8);	
			GC9200_SendString(ucTempBuf,8);
		}else if(ucCmd == 0xA2){//Read used ID number
			AT24CXX_Read(ID_USED_NUM_ADD,ucTempBuf,8);
			GC9200_SendString(ucTempBuf,8);
		}else if(ucCmd == 0xA3){//Read fail id number
			AT24CXX_Read(PLC_FAIL_ID_NUM_ADD,ucTempBuf,5);
			GC9200_SendString(ucTempBuf,5);
		}else if(ucCmd == 0xA4){//Read fail ID
			AT24CXX_Read(PLC_FAIL_ID_NUM_ADD,ucTempBuf,5);
			ucTemp = ucTempBuf[2];
			if(ucTemp == 0 ){
				GC9200_SendString(ucTempBuf,5);
			}else if(ucTemp <= FAIL_ID_MAX){
				AT24CXX_Read(PLC_FAIL_ID_ADD,ucTempBuf,ucTemp*4+4);
				GC9200_SendString(ucTempBuf,ucTemp*4+4);
			}
		}else if(ucCmd == 0xA5){
			ReadDataFromFlash(ucBackupDataFlash,FLASH_BACKUP_PAGE,2);//
			ucTemp = ucBackupDataFlash[1];//length
			if(ucBackupDataFlash[0] == 0x68){
				ReadDataFromFlash(ucBackupDataFlash,FLASH_BACKUP_PAGE,ucTemp);
				GC9200_SendString(ucBackupDataFlash,ucTemp);
			}else{ //ucAck
				GC9200_SendString(ucAck,2);
			}
		} 						
	}			
}





