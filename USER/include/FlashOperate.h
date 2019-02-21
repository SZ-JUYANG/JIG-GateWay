#ifndef __FLASH_OPERATE_H__
#define __FLASH_OPERATE_H__
#include "main.h"

typedef struct
{
	unsigned short int  uiStartFlag;    /* ��ʼ��־ */
	unsigned short int  uiYear;         /* �꣺   0Xyyyy */
	unsigned short int  uiMonthDate;    /* ���գ�0xmmdd */
	unsigned short int  uiHourMin;      /* ʱ�֣�0xhhmm */
	unsigned short int  uiEndFlag;      /* ������־ */
}ScmModifyTime;

#define   FLASH_MODIFY_TIME_ADR   0x08078C00//
#define   START_FLAG              0X1984
#define   END_FLAG                0X9986

void SaveModifyTime(unsigned long ulFlashAdr, ScmModifyTime sSystemParam);
ScmModifyTime ReadModifyTime(unsigned long ulFlashAdr, unsigned char *pucRezult);

#endif


