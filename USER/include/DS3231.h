#include "stm32f10x.h"
#ifndef __D3231_H__
#define __D3231_H__

/*****************************************************************************
**                            宏定义
******************************************************************************/
#define DS3231_SLAVE_ADDR           0xD0

//DS3231的寄存器（开始）地址
#define DS3231_REG_BEGIN_ADDR       0x00
#define DS3231_TIME_BEGIN_ADDR      0x00
#define DS3231_ALARM1_BEGIN_ADDR    0x07
#define DS3231_ALARM2_BEGIN_ADDR    0x0B
#define DS3231_CTRL_ADDR            0x0E
#define DS3231_CTRL_STATUS_ADDR     0x0F
#define DS3231_OFFSET_ADDR          0x10
#define DS3231_TEMP_BEGIN_ADDR      0x11

#define DS3231_SUBA_EN_0   	0x00
#define DS3231_SUBA_EN_1    0x01
#define DS3231_SUBA_EN_2    0x02

#define I2C_PORT_0  				0x00
#define I2C_PORT_1  				0x01
#define I2C_PORT_2  				0x02

//#define IIC_SCL_HIGH  GPIO_SetBits(GPIOD, GPIO_Pin_0)
//#define IIC_SCL_LOW   GPIO_ResetBits(GPIOD, GPIO_Pin_0)
//#define IIC_SDA_HIGH  GPIO_SetBits(GPIOD, GPIO_Pin_1)
//#define IIC_SDA_LOW   GPIO_ResetBits(GPIOD, GPIO_Pin_1)
//#define IIC_SDA_Read  GPIOD->IDR&GPIO_Pin_1 

#define IIC_SCL_HIGH  GPIO_SetBits(GPIOB, GPIO_Pin_6)
#define IIC_SCL_LOW   GPIO_ResetBits(GPIOB, GPIO_Pin_6)
#define IIC_SDA_HIGH  GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define IIC_SDA_LOW   GPIO_ResetBits(GPIOB, GPIO_Pin_7)

#define IIC_SDA_Read  GPIOB->IDR&GPIO_Pin_7 

/*****************************************************************************
**                            结构定义
******************************************************************************/
/*****************************************************************************
    DS3231BUFtag结构中的各个变量的位置不可更改
    因为位置是与DS3231中的寄存器顺序一一对应的            
******************************************************************************/
typedef struct{
    unsigned char cTimeSecond;           //[00]
    unsigned char cTimeMinute;           //[01]
    unsigned char cTimeHour;             //[02]
    unsigned char cTimeDay;              //[03]
    unsigned char cTimeDate;             //[04]
    unsigned char cTimeMonth;            //[05]
    unsigned char cTimeYear;             //[06]
                                
    unsigned char cAlarm1Second;         //[07]
    unsigned char cAlarm1Minute;         //[08]
    unsigned char cAlarm1Hour;           //[09]
    unsigned char cAlarm1DayDate;        //[0A]
                                
    unsigned char cAlarm2Minute;         //[0B]
    unsigned char cAlarm2Hour;           //[0C]
    unsigned char cAlarm2DayDate;        //[0D]

    unsigned char cCtlReg;               //[0E]
    unsigned char cCtlStatusReg;         //[0F]

    unsigned char cOffsetReg;            //[10]
    unsigned char cMSBofTemp;            //[11]
    unsigned char cLSBofTemp;            //[12]
    unsigned char cFlag1;                //reserved
    unsigned char cFlag2;                //reserved
}DS3231BUFtag;

typedef struct{
    unsigned char ucSecond;           //[00]
    unsigned char ucMinute;           //[01]
    unsigned char ucHour;             //[02]
    unsigned char ucDay;              //[03]
    unsigned char ucDate;             //[04]
    unsigned char ucMonth;            //[05]
    unsigned char ucYear;             //[06]
}ScmDS3231Struct;


typedef struct{
    unsigned char cSecond;           //[07]
    unsigned char cMinute;           //[08]
    unsigned char cHour;             //[09]
    unsigned char cDayDate;          //[0A]    
}DS3231_ALARM1tag;


typedef struct{
    unsigned charcMinute;           //[0B]
    unsigned char cHour;             //[0C]
    unsigned char cDayDate;          //[0D]  
}DS3231_ALARM2tag;

typedef struct{
    unsigned char cTempMSB;          //[11]
    unsigned char cTempLSB;          //[12]
}DS3231_TEMPtag;

/* 对外函数接口 */
void I2C_DS3231_Config(void);
void delay_1us(void);
void delay_us(unsigned int uiCounter);
void SDA_DIR_SET(unsigned char ucSdaDir);
void IIC_start(void);
void IIC_stop(void);
unsigned char IIC_waitack(void);
void IIC_ack(void);
void IIC_noack(void);
void IIC_sendbyte(unsigned char  ucData);
unsigned char IIC_readbyte(unsigned char ucDck);

void ModifyTimeHEX(ScmDS3231Struct sTime);
void ReadTimeHEX(ScmDS3231Struct *psTime);
void I2C1_WriteByte(unsigned char id,unsigned char write_address,unsigned char byte);
#endif
