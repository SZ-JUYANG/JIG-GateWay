#ifndef __HLK_M30_H
#define __HLK_M30_H

#define RST_N_L()  GPIO_ResetBits(GPIOC, GPIO_Pin_7)
#define RST_N_H()  GPIO_SetBits(GPIOC, GPIO_Pin_7) 
#define ES_RST_L() GPIO_ResetBits(GPIOC, GPIO_Pin_6)
#define ES_RST_H() GPIO_SetBits(GPIOC, GPIO_Pin_6)
#define HLK_RX_L() GPIO_ResetBits(GPIOA, GPIO_Pin_9)
#define HLK_RX_H() GPIO_SetBits(GPIOA, GPIO_Pin_9)
void HLK_M30_Init(void);
#endif
