#ifndef __GP9034T_H__
#define __GP9034T_H__

#define PRINT_YEAR_MONTH     1609     //YY-MM格式
#define PRINT_DEVICE_CODE    0XC9     //十六进制格式 
void GP9034T_Print(unsigned long ulPhyID);
//void GP9034T_PrintData(unsigned long ulPhyID,unsigned char ucDeviceCode);
void GP9034T_PrintData(unsigned long ulPhyID,unsigned char ucDeviceCode,unsigned char ucYear,unsigned char ucMonth);
void GP9034T_PrintOneData(unsigned long ulPhyID,unsigned char ucDeviceCode,unsigned char ucYear,unsigned char ucMonth);
#endif

