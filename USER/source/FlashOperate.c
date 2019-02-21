#include "stm32f10x.h"
#include "FlashOperate.h"
#include "main.h"
/*******************************************************************************
* Function Name : ReadDataFromFlash
* Description   : Read data from Flash
* Input         : *RamAdrStar   uiSum: total of bytes reading from FLASH
* Output        : None.
* Return        : None.
*******************************************************************************/
static void ReadDataFromFlash(unsigned char *RamAdrStar, unsigned long ulFlashAdr, unsigned short int uiSum)
{
    unsigned short int uiCnt;
    for(uiCnt = 0; uiCnt < uiSum; uiCnt++){
        *(RamAdrStar + uiCnt) = *(__IO uint8_t*)(ulFlashAdr + uiCnt);
    }
}
/*******************************************************************************
* Function Name : WriteDataToFlash
* Description   : Write data to Flash
* Input         : *RamAdrStar:RAM     uiSum: total of bytes writting to FLASH
* Output        : None.
* Return        : None.
*******************************************************************************/
static void WriteDataToFlash(unsigned char *RamAdrStar, unsigned long ulFlashAdr, unsigned short int uiSum)
{
    unsigned short int uiCnt;
    for(uiCnt = 0; uiCnt < uiSum; uiCnt += 2){    /* write 2 bytes every time */
        FLASH_ProgramHalfWord(ulFlashAdr + uiCnt, (*(RamAdrStar + uiCnt + 1)<<8) + *(RamAdrStar + uiCnt));
    }
}
/*******************************************************************************
* Function Name : WriteHalfWordToFlash
* Description   : Write 16 bit to Flash
* Input         : uiWord
* Output        : None.
* Return        : None.
*******************************************************************************/
static void WriteHalfWordToFlash(unsigned short int uiWord, unsigned long ulFlashAdr)
{
    unsigned char ucTemp[2];
    ucTemp[0] = (unsigned char)uiWord;
    ucTemp[1] = (unsigned char)(uiWord>>8);
    WriteDataToFlash(&ucTemp[0], ulFlashAdr, 2);
}

/*******************************************************************************
* Function Name : WriteHalfWordToFlash
* Description   : Write 16 bit to Flash
* Input         : uiWord
* Output        : None.
* Return        : None.
*******************************************************************************/
void SaveModifyTime(unsigned long ulFlashAdr, ScmModifyTime sModifyTime)
{
	sModifyTime.uiStartFlag = START_FLAG;
	sModifyTime.uiEndFlag = END_FLAG;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR |FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(ulFlashAdr);
	WriteDataToFlash((unsigned char*)(&sModifyTime), ulFlashAdr, sizeof(ScmModifyTime));
	FLASH_Lock();
}
/*******************************************************************************
* Function Name : ReadPhyIdFromFlash
* Description   : Write 16 bit to Flash
* Input         : ulFlashAdr： FLASH adress； pucRezult：结果：0：数据非法； 1：数据合法
* Output        : None.
* Return        : None.
*******************************************************************************/
ScmModifyTime ReadModifyTime(unsigned long ulFlashAdr, unsigned char *pucRezult)
{
	ScmModifyTime sTemp;
	ReadDataFromFlash((unsigned char *)(&sTemp), ulFlashAdr, sizeof(ScmModifyTime)); 
	if(sTemp.uiStartFlag != START_FLAG || sTemp.uiEndFlag != END_FLAG){
		*pucRezult = 0; /* 数据不合法 */
	}else{/* 数据合法 */
		*pucRezult = 1;
	}
	return sTemp;
}

