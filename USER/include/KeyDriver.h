#ifndef __KeyDriver_H
#define __KeyDriver_H
#include "stm32f10x.h"

#define KEY_ENTER()    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4)
#define KEY_ESC()      GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)

#define KEY_UP()       GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)
#define KEY_DOWN()     GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)
//#define KEY_LEFT()     GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)
//#define KEY_RIGHT()    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)
#define KEY_PRESS_DOWN 0

#define KEY_MAX_NUM       5                //������󰴼�����
#define KEY_BUF_SIZE      5                //���尴����������С(1~256)
#define LONG_KEY_EN       1                //�����Ƿ�֧�ֳ�����(1 or 0)
//#define KEY_SHORT_SHIFT   KEY_ESC|ENTER  //����shift��,�̰���Ч(0 or ����ɨ����)
#define KEY_SHORT_SHIFT   KEY_E //��
//#define KEY_SHORT_SHIFT   LEFT

#if LONG_KEY_EN > 0
#define KEY_PRESS_DLY     30           //���峤������ʱn��ɨ�����ڿ�ʼ�ж���Чֵ(1~256)
#define KEY_PRESS_TMR     15           //���峤����n��ɨ������Ϊ1����Чɨ��ֵ(1~256)
//#define KEY_LONG_SHIFT    ENTER|DOWN  //�������������(��һ��),δ����ĳ����������(0 or ����ɨ����)
#define KEY_LONG_SHIFT    ENTER
#endif

#define UP            0x01          //ÿ������ռ1bit,�����ظ�
#define DOWN          0x02
//#define LEFT          0x04
//#define RIGHT         0x08
#define ENTER         0x10
#define ESC           0x20

#define KEY_E         0x40
#define ENTER_LONG    0xEF      // ENTER����

void Key_GPIO_Config(void);
void KeyScan(void);
unsigned char  KeyGet(void);
unsigned char  KeyHit (void);
unsigned char  KeyIOread( void );
void KeyFlush(void);
#endif
