#ifndef __LCD12864B_H
#define __LCD12864B_H

#define INPUT 0
#define OUTPUT 1
#define NORMAL  0
#define INVERSE 1

#define LCD_CLEAR_CMD 0x01//
#define INVERSE 1

#define OUT 0x01
#define IN  0x00

#define LCD12864B_CLK_H()   GPIO_SetBits(GPIOE, GPIO_Pin_10) 
#define LCD12864B_CLK_L()   GPIO_ResetBits(GPIOE, GPIO_Pin_10)
#define LCD12864B_SID_H()   GPIO_SetBits(GPIOE, GPIO_Pin_11)
#define LCD12864B_SID_L()   GPIO_ResetBits(GPIOE, GPIO_Pin_11)
#define READ_SID()          GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)
#define LCD12864B_CS_H()    GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define LCD12864B_CS_L()    GPIO_ResetBits(GPIOB, GPIO_Pin_12)

//void LCD12864G_GPIO_Config(void);
void Lcd12864_init(void);
//void Lcd12864_string(unsigned char ucAdd,const char *p);
void DisplayString(unsigned char ucLine,unsigned char ucColumn,unsigned char *s);
void Lcd12864_clear(void) ;
void DisplayChar(unsigned char ucLine,unsigned char ucColumn,unsigned char s);
void LCD_delay_ms(unsigned int Counter);
void FillSpace(void);
#endif

