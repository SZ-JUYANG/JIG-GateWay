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

#define KEY_MAX_NUM       5                //定义最大按键数量
#define KEY_BUF_SIZE      5                //定义按键缓冲区大小(1~256)
#define LONG_KEY_EN       1                //定义是否支持长按键(1 or 0)
//#define KEY_SHORT_SHIFT   KEY_ESC|ENTER  //定义shift键,短按有效(0 or 按键扫描码)
#define KEY_SHORT_SHIFT   KEY_E //改
//#define KEY_SHORT_SHIFT   LEFT

#if LONG_KEY_EN > 0
#define KEY_PRESS_DLY     30           //定义长按键延时n个扫描周期开始判定有效值(1~256)
#define KEY_PRESS_TMR     15           //定义长按键n个扫描周期为1次有效扫描值(1~256)
//#define KEY_LONG_SHIFT    ENTER|DOWN  //长按键反码输出(仅一次),未定义的长按连续输出(0 or 按键扫描码)
#define KEY_LONG_SHIFT    ENTER
#endif

#define UP            0x01          //每个按键占1bit,不得重复
#define DOWN          0x02
//#define LEFT          0x04
//#define RIGHT         0x08
#define ENTER         0x10
#define ESC           0x20

#define KEY_E         0x40
#define ENTER_LONG    0xEF      // ENTER反码

void Key_GPIO_Config(void);
void KeyScan(void);
unsigned char  KeyGet(void);
unsigned char  KeyHit (void);
unsigned char  KeyIOread( void );
void KeyFlush(void);
#endif
