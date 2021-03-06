#include "WordLib.h"

/*********1、配置（齿轮）***********/
const unsigned char Config_24x24[72]=
{0x00,0x00,0x30,0x78,0xFC,0xFC,0xF8,0xF0,0x70,0x7C,0x3F,0x3F,0x3F,0x3F,0x7C,0x70,
0xF0,0xF8,0xFC,0xFC,0x78,0x30,0x00,0x00,0x3C,0x3C,0x7E,0x7E,0xFF,0xFF,0x81,0x00,
0x7E,0xFF,0xFF,0xE7,0xE7,0xFF,0xFF,0x7E,0x00,0x81,0xFF,0xFF,0x7E,0x7E,0x3C,0x3C,
0x00,0x00,0x0C,0x1E,0x3F,0x3F,0x1F,0x0F,0x0E,0x3E,0xFC,0xFC,0xFC,0xFC,0x3E,0x0E,
0x0F,0x1F,0x3F,0x3F,0x1E,0x0C,0x00,0x00};
/*********2、控制（工具）***********/
const unsigned char Control_24x24[72]=
{
0x78,0xF0,0xC0,0xC1,0xC3,0xE3,0xFF,0xFE,0xFC,0x90,0x00,0x00,0x00,0x00,0x80,0xC0,
0xE0,0xF0,0xF8,0x7C,0x3E,0x1F,0x0F,0x07,0x00,0x00,0x01,0x01,0x03,0x81,0xC1,0xE3,
0xE7,0xCF,0x9F,0x3E,0x7C,0xF9,0xF3,0xE7,0xC3,0x81,0x80,0xC0,0x80,0x80,0x00,0x00,
0x70,0xF8,0xFC,0xFE,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x01,0x00,0x00,0x00,0x09,0x3F,
0x7F,0xFF,0xC7,0xC3,0x83,0x03,0x0F,0x1E};
/*********3、设备信息（圆）***********/
const unsigned char Info_24x24[72]=
{0x80,0xC0,0xF0,0xF8,0xFC,0xFC,0xFE,0xFF,0xFF,0xFF,0xFF,0x1F,0x1F,0xFF,0xFF,0xFF,
0xFF,0xFE,0xFC,0xFC,0xF8,0xF0,0xC0,0x80,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0x03,0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x01,0x03,0x0F,0x1F,0x3F,0x3F,0x7F,0xFF,0xFF,0xFF,0xFF,0xF8,0xF8,0xFF,0xFF,0xFF,
0xFF,0x7F,0x3F,0x3F,0x1F,0x0F,0x03,0x01};
/*********4、记录（文本）***********/
const unsigned char Record_24x24[72]=
{0x00,0x00,0x00,0xFE,0xFF,0xFF,0xF0,0xF7,0xF7,0xF7,0xF7,0xF7,0xF7,0xF7,0xF7,0xF7,
0xF7,0xF0,0xFF,0xFF,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x0F,0xEF,0xFF,0x7F,0x3F,0x00,0x00,0x00};
/*********5、警告（感叹）***********/
const unsigned char Warning_24x24[72]=
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,0xF8,0xFC,0x7E,0x7E,0xFC,0xF8,0xE0,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xF0,0xFC,0xFF,0xFF,
0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0xF0,0xC0,0x80,0x00,0x00,
0x38,0x7E,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x73,0x73,0x7F,0x7F,0x7F,
0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7E,0x38};

/*********6、返回***********/
const unsigned char Back_24x24[72]=
{0x80,0xC0,0xF0,0xF8,0xFC,0x7C,0x3E,0x1F,0x0F,0x0F,0x0F,0x00,0x00,0x00,0xFF,0xFF,
0xFF,0x7F,0x7F,0xFF,0xFF,0xF7,0xC3,0x81,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x01,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
0x01,0x03,0x0F,0x1F,0x3F,0x3E,0x7C,0xF8,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,
0xF8,0x7C,0x3E,0x3F,0x1F,0x0F,0x03,0x01};
//const unsigned char MiniBack_16x16[32]=
//{0x00,0xF0,0xFC,0x1E,0x0E,0x06,0x06,0x00,0x00,0x7E,0x7E,0x3E,0x3E,0xFE,0xF6,0x00,
//0x00,0x1F,0x3F,0x78,0x70,0x60,0x60,0x60,0x60,0x60,0x60,0x70,0x38,0x1F,0x0F,0x00};
const unsigned char Triangle_16x9[18]=
{0x00,0xFE,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80,0x00,0x7F,0x7F,0x3F,0x1F,0x0F,0x07,
0x03,0x01};
const unsigned char Triangle_Blank[18]=
{0x00};

const unsigned char Refresh_16x16[32]=
{/*--  宽度x高度=16x16  --*/
0x00,0x78,0x7C,0x3E,0x0E,0x07,0x07,0x03,0x03,0x17,0x3F,0x3E,0x7E,0x7E,0x7F,0x00,
0x00,0xFE,0x7E,0x7E,0x7C,0xFC,0xE8,0xC0,0xC0,0xE0,0xE0,0x70,0x7C,0x3E,0x1E,0x00};
/*--  文字:  美  --*/
/*--  宋体22;  此字体下对应的点阵为：宽x高=30x29  --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=30x32  --*/
const unsigned char Mei_32x32[128]={
0x00,0x00,0x40,0x40,0x40,0x40,0x40,0x40,0x46,0x5E,0x7C,0x7C,0x78,0xF8,0xC0,0xC0,
0xF8,0x7E,0x7E,0x5E,0x4C,0x44,0x60,0x60,0x60,0x60,0x40,0x00,0x00,0x00,0x00,0x40,
0x40,0x40,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0xFF,0xFF,0xFF,0x7F,0x44,
0x44,0x44,0x44,0x46,0x46,0x66,0x64,0x64,0x60,0x60,0x40,0x00,0x00,0x00,0x04,0x04,
0x04,0x04,0x04,0x04,0x04,0x84,0xC4,0xF4,0xFF,0xFF,0x3F,0x3F,0x7F,0xF5,0xE4,0xC4,
0x84,0x84,0x06,0x06,0x06,0x06,0x04,0x00,0x00,0x00,0x00,0x08,0x08,0x08,0x0C,0x0C,
0x0E,0x06,0x07,0x03,0x03,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x03,0x07,
0x07,0x07,0x0E,0x0E,0x0E,0x06,0x02,0x00,};

/*--  文字:  吉  --*/
/*--  宋体22;  此字体下对应的点阵为：宽x高=30x29   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=30x32  --*/
const unsigned char Ji_32x32[128]={
0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFE,0xFE,0xFE,
0xFC,0x84,0x80,0x80,0x80,0x80,0x80,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,
0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x1F,0x1F,0x1F,0x1F,0x10,
0x10,0x10,0x10,0x10,0x18,0x18,0x18,0x18,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0xFF,0xFE,0xFE,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
0xFF,0xFF,0xFF,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x0F,0x0F,0x0F,0x07,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x07,0x07,
0x07,0x07,0x00,0x00,0x00,0x00,0x00,0x00,};

/*--  文字:  星  --*/
/*--  宋体22;  此字体下对应的点阵为：宽x高=30x29   --*/
/*--  高度不是8的倍数，现调整为：宽度x高度=30x32  --*/
const unsigned char Xing_32x32[128]={
0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0xFC,0xFC,0xF8,0x88,0x88,0x88,0x88,0x88,0x88,
0x88,0x88,0x88,0x88,0xF8,0xFC,0xFC,0xFC,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0xFF,0xFF,0xFF,0xDF,0x48,0x08,0x08,0xF8,0xF8,0xF8,0xE8,0x28,
0x08,0x08,0x1F,0x9F,0x9F,0x9F,0x9F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x78,
0x3C,0x1F,0x1F,0x17,0x13,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,0xFF,0x11,0x11,0x11,
0x19,0x19,0x19,0x19,0x11,0x01,0x01,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x02,0x02,
0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x02,0x02,
0x02,0x03,0x03,0x03,0x03,0x03,0x02,0x00,};

/*--  宋体12;  此字体下对应的点阵为：宽x高=9x16   --*/
//const unsigned char g_ucNumTab[46][18]={
//	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,
//   0x01,0x00,},//-  45
//	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x30,0x00,0x00,0x00,
//   0x00,0x00,},//. 
//	{0x00,0x00,0x00,0x00,0xC0,0xF8,0x3C,0x04,0x00,0x00,0x60,0x78,0x1F,0x07,0x00,0x00,
//   0x00,0x00,},// /
//	{0x00,0xE0,0xF0,0x18,0x08,0x18,0xF0,0xE0,0x00,0x00,0x0F,0x1F,0x30,0x20,0x30,0x1F,
//   0x0F,0x00,},//0  48
//	{0x00,0x00,0x10,0x10,0xF8,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x3F,0x20,
//   0x20,0x00,},//1
//  {0x00,0x70,0x78,0x08,0x08,0x08,0xF8,0xF0,0x00,0x00,0x30,0x38,0x2C,0x26,0x23,0x31,
//   0x30,0x00,},//2
//	{0x00,0x30,0x38,0x08,0x08,0x88,0xF8,0x70,0x00,0x00,0x18,0x38,0x21,0x21,0x23,0x3E,
//   0x1C,0x00,},//3
//	{0x00,0x00,0x80,0xC0,0x70,0xF8,0xF8,0x00,0x00,0x00,0x06,0x07,0x25,0x24,0x3F,0x3F,
//   0x24,0x24,},//4
//	{0x00,0xF8,0xF8,0x88,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x39,0x20,0x20,0x31,0x1F,
//   0x0E,0x00,},//5
//	{0x00,0xE0,0xF0,0x98,0x88,0x98,0x90,0x00,0x00,0x00,0x0F,0x1F,0x31,0x20,0x20,0x3F,
//   0x1F,0x00},//6
//	{0x00,0x18,0x18,0x08,0x88,0xE8,0x78,0x18,0x00,0x00,0x00,0x00,0x3E,0x3F,0x01,0x00,
//   0x00,0x00,},//7
//	{0x00,0x70,0xF8,0x88,0x08,0x88,0xF8,0x70,0x00,0x00,0x1C,0x3E,0x23,0x21,0x23,0x3E,
//   0x1C,0x00,},//8
//	{0x00,0xF0,0xF8,0x08,0x08,0x18,0xF0,0xE0,0x00,0x00,0x01,0x13,0x32,0x22,0x33,0x1F,
//   0x0F,0x00,},//9 ASCII 57
//	{0x00,0x00,0x00,0xC0,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x30,0x00,
//   0x00,0x00,},//：
//	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//   0x00,0x00,},//; ASCII 59 字形为空  第15个
//  /*--  文字:  <  --*/
//  {0x00,0x00,0x80,0xC0,0x60,0x30,0x18,0x08,0x00,0x00,0x01,0x03,0x06,0x0C,0x18,0x30,
//   0x20,0x00},
///*--  文字:  =  --*/
//{0x00,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x00,0x02,0x02,0x02,0x02,0x02,0x02,
//0x02,0x00},
///*--  文字:  >  --*/
//{0x00,0x08,0x18,0x30,0x60,0xC0,0x80,0x00,0x00,0x00,0x20,0x30,0x18,0x0C,0x06,0x03,
//0x01,0x00},
///*--  文字:  ?  --*/
//{0x00,0x70,0x78,0x48,0x08,0x88,0xF8,0x70,0x00,0x00,0x00,0x00,0x30,0x37,0x37,0x00,
//0x00,0x00},
///*--  文字:  @  --*/
//{0xC0,0xF0,0xF8,0xE8,0xE8,0xF8,0xF0,0xE0,0x00,0x07,0x1F,0x3F,0x2F,0x2F,0x2F,0x3F,
//0x17,0x00},
///*--  文字:  A  --*/
//{0x00,0x00,0xC0,0xF8,0xF8,0xE0,0x00,0x00,0x00,0x20,0x3C,0x3F,0x23,0x02,0x27,0x3F,
//0x38,0x20},
///*--  文字:  B  --*/
//{0x08,0xF8,0xF8,0x88,0x88,0xF8,0x70,0x00,0x00,0x20,0x3F,0x3F,0x20,0x20,0x31,0x1F,
//0x0E,0x00},
///*--  文字:  C  --*/
//{0xC0,0xF0,0x38,0x08,0x08,0x08,0x38,0x38,0x00,0x07,0x1F,0x38,0x20,0x20,0x30,0x18,
//0x08,0x00},
///*--  文字:  D  --*/
//{0x08,0xF8,0xF8,0x08,0x08,0x18,0xF0,0xE0,0x00,0x20,0x3F,0x3F,0x20,0x20,0x30,0x1F,
//0x0F,0x00},
///*--  文字:  E  --*/
//{0x08,0xF8,0xF8,0x88,0xE8,0xE8,0x18,0x10,0x00,0x20,0x3F,0x3F,0x20,0x23,0x23,0x38,
//0x18,0x00},
///*--  文字:  F  --*/
//{0x08,0xF8,0xF8,0x88,0xE8,0xE8,0x18,0x10,0x00,0x20,0x3F,0x3F,0x20,0x03,0x03,0x00,
//0x00,0x00},
///*--  文字:  G  --*/
//{0xC0,0xF0,0x38,0x08,0x08,0x38,0x38,0x00,0x00,0x07,0x1F,0x38,0x20,0x22,0x3E,0x1E,
//0x02,0x00},
///*--  文字:  H  --*/
//{0x08,0xF8,0xF8,0x08,0x00,0x08,0xF8,0xF8,0x08,0x20,0x3F,0x3F,0x21,0x01,0x21,0x3F,
//0x3F,0x20},
///*--  文字:  I  --*/
//{0x00,0x08,0x08,0xF8,0xF8,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x3F,0x3F,0x20,0x20,
//0x00,0x00},
///*--  文字:  J  --*/
//{0x00,0x00,0x08,0x08,0xF8,0xF8,0x08,0x08,0x00,0xC0,0xC0,0x80,0x80,0xFF,0x7F,0x00,
//0x00,0x00},
///*--  文字:  K  --*/
//{0x08,0xF8,0xF8,0xC8,0xE8,0x38,0x18,0x08,0x00,0x20,0x3F,0x3F,0x21,0x27,0x3E,0x38,
//0x20,0x00},
///*--  文字:  L  --*/
//{0x08,0xF8,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x3F,0x20,0x20,0x20,0x30,
//0x30,0x00},
///*--  文字:  M  --*/
//{0x08,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0x08,0x00,0x20,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
//0x20,0x00},
///*--  文字:  N  --*/
//{0x08,0xF8,0xF8,0xF0,0xC0,0x08,0xF8,0xF8,0x08,0x20,0x3F,0x3F,0x20,0x07,0x1F,0x3F,
//0x3F,0x00},
///*--  文字:  O  --*/
//{0xE0,0xF0,0x18,0x08,0x08,0x18,0xF0,0xE0,0x00,0x0F,0x1F,0x30,0x20,0x20,0x30,0x1F,
//0x0F,0x00},
///*--  文字:  P  --*/
//{0x08,0xF8,0xF8,0x08,0x08,0x08,0xF8,0xF0,0x00,0x20,0x3F,0x3F,0x21,0x01,0x01,0x01,
//0x00,0x00},
///*--  文字:  Q  --*/
//{0xE0,0xF0,0x18,0x08,0x08,0x18,0xF0,0xE0,0x00,0x0F,0x1F,0x38,0x28,0x38,0x70,0x5F,
//0x4F,0x00},
///*--  文字:  R  --*/
//{0x08,0xF8,0xF8,0x88,0x88,0x88,0xF8,0x70,0x00,0x20,0x3F,0x3F,0x20,0x03,0x0F,0x3C,
//0x30,0x20},
///*--  文字:  S  --*/
//{0x00,0x70,0xF8,0x88,0x08,0x08,0x38,0x38,0x00,0x00,0x38,0x38,0x21,0x21,0x23,0x3E,
//0x1C,0x00},
///*--  文字:  T  --*/
//{0x18,0x18,0x08,0xF8,0xF8,0x08,0x18,0x18,0x00,0x00,0x00,0x20,0x3F,0x3F,0x20,0x00,
//0x00,0x00},
///*--  文字:  U  --*/
//{0x08,0xF8,0xF8,0x08,0x00,0x08,0xF8,0xF8,0x08,0x00,0x1F,0x3F,0x20,0x20,0x20,0x3F,
//0x1F,0x00},
///*--  文字:  V  --*/
//{0x08,0x78,0xF8,0x88,0x00,0xC8,0xF8,0x38,0x08,0x00,0x00,0x07,0x3F,0x3E,0x0F,0x01,
//0x00,0x00},
///*--  文字:  W  --*/
//{0x08,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0x08,0x00,0x00,0x03,0x3F,0x3F,0x3F,0x3F,0x03,
//0x00,0x00},
///*--  文字:  X  --*/
//{0x08,0x18,0x78,0xE8,0x80,0xE8,0x78,0x18,0x08,0x20,0x30,0x3C,0x2F,0x03,0x2F,0x3C,
//0x30,0x20},
///*--  文字:  Y  --*/
//{0x08,0x38,0xF8,0xC8,0xC8,0xF8,0x38,0x08,0x00,0x00,0x00,0x20,0x3F,0x3F,0x20,0x00,
//0x00,0x00},
///*--  文字:  Z  --*/
//{0x10,0x18,0x08,0x08,0xC8,0xF8,0x38,0x08,0x00,0x20,0x38,0x3E,0x27,0x21,0x20,0x38,
//0x18,0x00},	
//	  
//};

const unsigned char g_15x32[60]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0xC0,0xE0,0xF0,0x30,0x10,0x10,0x30,0x70,0xF0,0xF0,0xB0,0x30,0x30,0x00,0x00,
0xB3,0xFF,0xFF,0x4C,0xC8,0xC8,0xCC,0xCE,0xC7,0x87,0x81,0x00,0x00,0x00,0x07,0x0F,
0x1F,0x18,0x10,0x10,0x10,0x10,0x10,0x18,0x1F,0x0F,0x07,0x00,};

const unsigned char r_15x32[60]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,
0x20,0x20,0x20,0xF0,0xF0,0xF0,0xE0,0x60,0x30,0x10,0x70,0x70,0x70,0x00,0x00,0x00,
0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,};
const unsigned char i_15x32[60]={0x00,0x00,0x00,0x00,0x00,0x40,0xE0,0xE0,0xE0,0x40,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x10,0x10,0x10,0xF8,0xF8,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,};
const unsigned char d_15x32[60]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0xE0,0xF0,0xF0,0x00,0x00,0x00,
0x00,0xC0,0xE0,0xF0,0x30,0x10,0x10,0x10,0x30,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x3F,
0x7F,0xFF,0xE1,0x80,0x00,0x00,0x80,0xC0,0xFF,0xFF,0xFF,0x80,0x80,0x00,0x00,0x00,
0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,};
const unsigned char C_15x32[60]={0x00,0x00,0x00,0x80,0xC0,0xE0,0x60,0x20,0x20,0x20,0x60,0xC0,0xE0,0xE0,0x00,0xF0,
0xFE,0xFF,0xFF,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x02,0x03,0x3F,
0x7F,0xFF,0xF0,0xC0,0x80,0x00,0x00,0x00,0x80,0xC0,0xF0,0x38,0x08,0x00,0x00,0x00,
0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,};
const unsigned char o_15x32[60]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x80,0xC0,0xE0,0xF0,0x30,0x10,0x10,0x10,0x30,0xF0,0xE0,0xC0,0x80,0x00,0x00,0x3F,
0x7F,0xFF,0xE0,0x80,0x00,0x00,0x00,0x80,0xE0,0xFF,0x7F,0x3F,0x00,0x00,0x00,0x00,
0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,};
const unsigned char m_15x32[60]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,
0xF0,0xF0,0xF0,0x30,0x10,0xF0,0xF0,0xE0,0x30,0x10,0xF0,0xF0,0xF0,0x00,0x00,0xFF,
0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,};

