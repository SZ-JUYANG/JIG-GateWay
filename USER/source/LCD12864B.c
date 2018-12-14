#include "stm32f10x.h"
#include "LCD12864B.h"

#include "System_Initialize.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define LCD_E_0()    GPIOD->BRR = GPIO_Pin_7
#define LCD_E_1()    GPIOD->BSRR = GPIO_Pin_7
#define LCD_WR_0()   GPIOD->BRR = GPIO_Pin_5
#define LCD_WR_1()   GPIOD->BSRR = GPIO_Pin_5
#define LCD_RS_0()   GPIOD->BRR = GPIO_Pin_4
#define LCD_RS_1()   GPIOD->BSRR = GPIO_Pin_4
#define LCD_RST_0()  GPIOA->BRR =  GPIO_Pin_0
#define LCD_RST_1()  GPIOA->BSRR = GPIO_Pin_0
/*******************************************************************************
* Function Name : Delay
* Description   : 延时
*******************************************************************************/

static void LCD_delay(unsigned int nCount)
{
	for(; nCount != 0; nCount--);
}

void LCD_delay_ms(unsigned int Counter)
{
		unsigned int i;		
		for(; Counter !=0; Counter--){		
			i = 10301;
			while(i--);
		}	
}
/*******************************************************************************
* Function Name : LCD12864G_GPIO_Config
* Description   : 配置12864端口
*******************************************************************************/
void LCD12864G_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    //设置PD0,1,4,5,7,14,15为输出
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//设置PE7,8,9,10为输出
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);		
}

//将一个字节的数据放到LCD数据总线
void Lcd_wbyte(unsigned char ucData)
{
	unsigned int uiTemp1, uiTemp2;
	unsigned int uiDataTemp;
  uiTemp1 = GPIOD->ODR;
	uiTemp1 &= 0x3ffc;
	uiDataTemp = ucData;
	uiTemp1 |= ((uiDataTemp & 0x0003)<<14); //取data的DB0,DB1
	uiTemp1 |= ((uiDataTemp & 0x000c)>>2);  //取data的DB2,DB3
	uiTemp2 = GPIOE->ODR;
	uiTemp2 &= 0xf87f;
	uiTemp2 |= ((uiDataTemp & 0x00f0)<<3); //取data的DB4~DB7
  GPIOD->ODR = uiTemp1;
	GPIOE->ODR = uiTemp2;
}
//写指令
void lcd_wcmd(unsigned char cmd)
{ 	
   LCD_RS_0();//LCD_RS = 0;
   LCD_WR_0();//LCD_RW = 0;
   LCD_E_1();//LCD_EN = 1; 
   LCD_delay(600);
   Lcd_wbyte(cmd);//LcdDataPort = cmd;
   LCD_delay(600);
   LCD_E_0();//LCD_EN = 0;  
}
//写数据
void lcd_wdat(unsigned char  dat)
{
   LCD_RS_1();//LCD_RS=1;
   LCD_WR_0();//LCD_RW=0;
   LCD_E_1();//LCD_EN=1;
   LCD_delay(600);
   Lcd_wbyte(dat);//LcdDataPort=dat; 
   LCD_delay(600);
   LCD_E_0();//LCD_EN = 0; 
}

//液晶清屏
void Lcd12864_clear(void) 
{  
	unsigned char i,j; 
	 //清上半屏
	for(j=0;j<32;j++) 
	{ 
		for(i=0;i<8;i++) 
		{  
			lcd_wcmd(0x34);//扩展指令集
			//连续写两个字节完成垂直于水平的坐标
			lcd_wcmd(0x80+j);//先设定垂直地址(0-32) 
			lcd_wcmd(0x80 + i);//再设定水平地址(0-8) 
			lcd_wcmd(0x30);//基本指令集
			lcd_wdat(0x00);//lcd_wdat(0x00);
			lcd_wdat(0x00);//lcd_wdat(0x00);
		}
	}
	  //清下半屏
	for(j=0;j<32;j++) 
	{ 
		for(i=0;i<8;i++) 
		{  
			lcd_wcmd(0x34);//扩展指令集
			//连续写两个字节完成垂直于水平的坐标
			lcd_wcmd(0x80+j);//先设定垂直地址(0-32) 
			lcd_wcmd(0x88 + i);//再设定水平地址(0-8) 
			lcd_wcmd(0x30);//基本指令集
			lcd_wdat(0x00);
			lcd_wdat(0x00);
		}
	}
}
//液晶初始化
void Lcd12864_init(void)
{ 
	LCD12864G_GPIO_Config();
	
	LCD_E_0();//LCD_EN=0;
	LCD_RST_0();//   RST = 0;  	
  LCD_delay_ms(500);  
  LCD_RST_1(); //   RST = 1;
  LCD_delay_ms(100);
  lcd_wcmd(0x30);      //基本指令操作
  LCD_delay(1);
	lcd_wcmd(0x30);      //基本指令操作
  LCD_delay(1);
	lcd_wcmd(0x08);      //基本指令操作
  LCD_delay(1);
  lcd_wcmd(0x01);      //清除LCD的显示内容
  LCD_delay(800);
	lcd_wcmd(0x06);      
  LCD_delay(5);
  lcd_wcmd(0x0C);      //显示开，关光标
  LCD_delay(5);
	
	LCD_delay_ms(100);
  Lcd12864_clear();//清屏
  LCD_delay_ms(10);	
}
//显示字符串
void Lcd12864_string(unsigned char ucAdd,const char *p)
{
	unsigned char i;
	i = strlen(p);
	lcd_wcmd(0x30);//基本指令集
	lcd_wcmd(ucAdd);//设置地址
	//lcd_wcmd(0x30);//基本指令集
	for(;i;i--){
		lcd_wdat(*p++);
	}
}

/*******************************************************************************
  * @brief  
  * @param  ucLine(1-4),ucColumn(1 - 8)
  * @retval None
*******************************************************************************/
void DisplayString(unsigned char ucLine,unsigned char ucColumn,unsigned char *s)
{
  unsigned char ucTempCom;
	lcd_wcmd(0x03);// set ??
  switch(ucLine){  
		case 1:
			ucTempCom = 0x80+ucColumn -1;
      break;
		case 2:
			ucTempCom = 0x90+ucColumn -1;
      break;
		case 3:
			ucTempCom = 0x88+ucColumn -1;
      break;	
		case 4:
			ucTempCom = 0x98+ucColumn -1;
      break;
    default:break;		
  }
	lcd_wcmd(ucTempCom);// set start Address
	while(*s != '\0'){
	  lcd_wdat(*s++);
	}	
}

void DisplayChar(unsigned char ucLine,unsigned char ucColumn,unsigned char s)
{
  unsigned char ucTempCom;
	lcd_wcmd(0x03);// set ??
  switch(ucLine){  
		case 1:
			ucTempCom = 0x80+ucColumn -1;
      break;
		case 2:
			ucTempCom = 0x90+ucColumn -1;
      break;
		case 3:
			ucTempCom = 0x88+ucColumn -1;
      break;	
		case 4:
			ucTempCom = 0x98+ucColumn -1;
      break;
    default:break;		
  }
	lcd_wcmd(ucTempCom);// set start Address	
	  lcd_wdat(s);	
}

void FillSpace(void)
{                  //1234567812345678
  DisplayString(1,1,"                ");
  DisplayString(2,1,"                ");
  DisplayString(3,1,"                ");
  DisplayString(4,1,"                ");
}
