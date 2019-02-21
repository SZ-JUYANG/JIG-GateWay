#ifndef __FLASH_OPERATE_H__
#define __FLASH_OPERATE_H__
#include "main.h"

typedef struct
{
	unsigned short int  uiStartFlag;    /* 开始标志 */
	unsigned short int  uiYear;         /* 年：   0Xyyyy */
	unsigned short int  uiMonthDate;    /* 月日：0xmmdd */
	unsigned short int  uiHourMin;      /* 时分：0xhhmm */
	unsigned short int  uiEndFlag;      /* 结束标志 */
}ScmModifyTime;

#define   FLASH_MODIFY_TIME_ADR   0x08078C00//
#define   START_FLAG              0X1984
#define   END_FLAG                0X9986

void SaveModifyTime(unsigned long ulFlashAdr, ScmModifyTime sSystemParam);
ScmModifyTime ReadModifyTime(unsigned long ulFlashAdr, unsigned char *pucRezult);

#endif


