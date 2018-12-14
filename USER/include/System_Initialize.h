#ifndef __System_Initialize_H
#define __System_Initialize_H
#include "stm32f10x.h"
#include "main.h"

#define LED1_OFF()    GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define LED1_ON()     GPIO_SetBits(GPIOB, GPIO_Pin_12) 
#define LED1_TOGGLE() GPIO_PinReverse(GPIOB, GPIO_Pin_12)

#define LED2_OFF()    GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define LED2_ON()     GPIO_SetBits(GPIOB, GPIO_Pin_13)
#define LED2_TOGGLE() GPIO_PinReverse(GPIOB, GPIO_Pin_13)

#define LED3_OFF()    GPIO_ResetBits(GPIOB, GPIO_Pin_14)
#define LED3_ON()     GPIO_SetBits(GPIOB, GPIO_Pin_14) 
#define LED3_TOGGLE() GPIO_PinReverse(GPIOB, GPIO_Pin_14)

#define TX_LED_OFF()    GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define TX_LED_ON()     GPIO_SetBits(GPIOB, GPIO_Pin_12) 

#define RX_LED_OFF()    GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define RX_LED_ON()     GPIO_SetBits(GPIOB, GPIO_Pin_13)

#define RX_D_LED_OFF()    GPIO_ResetBits(GPIOB, GPIO_Pin_14)
#define RX_D_LED_ON()     GPIO_SetBits(GPIOB, GPIO_Pin_14)


#define LED2_OFF()    GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define LED2_ON()     GPIO_SetBits(GPIOB, GPIO_Pin_13)
#define LED2_TOGGLE() GPIO_PinReverse(GPIOB, GPIO_Pin_13)





#define BEEP_OFF()    GPIO_ResetBits(GPIOC, GPIO_Pin_8)
#define BEEP_ON()     GPIO_SetBits(GPIOC, GPIO_Pin_8)
#define BEEP_TOGGLE() GPIO_PinReverse(GPIOC, GPIO_Pin_8)

#define SELECT_110V() GPIO_ResetBits(GPIOA, GPIO_Pin_11);\
                      GPIO_ResetBits(GPIOA, GPIO_Pin_12);\
											GPIO_ResetBits(GPIOA, GPIO_Pin_15)
											
#define SELECT_190V() GPIO_ResetBits(GPIOA, GPIO_Pin_11);\
                      GPIO_ResetBits(GPIOA, GPIO_Pin_12);\
											GPIO_SetBits(GPIOA, GPIO_Pin_15)
											
#define SELECT_220V() GPIO_SetBits(GPIOA, GPIO_Pin_11);\
                      GPIO_ResetBits(GPIOA, GPIO_Pin_12);\
											GPIO_ResetBits(GPIOA, GPIO_Pin_15)
											
#define SELECT_250V() GPIO_SetBits(GPIOA, GPIO_Pin_11);\
                      GPIO_SetBits(GPIOA, GPIO_Pin_12);\
											GPIO_ResetBits(GPIOA, GPIO_Pin_15)											

//#define AT_SCL_H      GPIO_SetBits(GPIOC, GPIO_Pin_11)
//#define AT_SCL_L      GPIO_ResetBits(GPIOC, GPIO_Pin_11)
//#define AT_SDA_H      GPIO_SetBits(GPIOC, GPIO_Pin_10)
//#define AT_SDA_L      GPIO_ResetBits(GPIOC, GPIO_Pin_10)
//#define AT_SDA_RD     GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_10)

void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed(void);
void GPIO_PinReverse(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void LED_GPIO_Init(void);
void BEEP_GPIO_Init(void);
void RELAY_GPIO_Init(void);
void AT24Cxx_GPIO_Init(void);
void TIM2_Configuration(void);

unsigned char CheckAT24CxxData(unsigned char *p,unsigned char ucNum);
void IdData2Scm(ScmAT24CxxTypeDef *scm,unsigned char*p);
void IdScm2Array(ScmAT24CxxTypeDef scm,unsigned char*p);
void ErrData2Scm(ScmAT24CxxErrTypeDef *scm,unsigned char*p);
void ErrScm2Array(ScmAT24CxxErrTypeDef scm,unsigned char*p);

void ReadDataFromFlash(unsigned char *RamAdrStar, unsigned long ulFlashAdr, unsigned short int uiSum);
void WriteDataToFlash(unsigned char *RamAdrStar, unsigned long ulFlashAdr, unsigned short int uiSum);
#endif
