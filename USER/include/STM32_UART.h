#ifndef __STM32_UART_H
#define __STM32_UART_H
void STM32_UART_Init(void);

void GC9200_SendByte(unsigned char byte); 
void GC9200_SendString(unsigned char *pString,unsigned char ucLen);
void GC9200_SendArray(const unsigned char *pString,unsigned char ucLen);

void IT8512_SendString(unsigned char *pString,unsigned char ucLen);
void IT8512_SendArray(const unsigned char *pString,unsigned char ucLen);
void PM9805_SendString(unsigned char *pString,unsigned char ucLen);

//void GP9034T_SendString(unsigned char *pString,unsigned char ucLen);
void GP9034T_SendArray(unsigned char *pString,unsigned char ucLen);

void C8000_SendString(unsigned char *pString,unsigned char ucLen);

#endif
