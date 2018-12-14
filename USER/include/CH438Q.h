
#ifndef __CH438Q_H
#define __CH438Q_H


#ifdef __CX51__
#ifndef __C51__
#define __C51__    1
#endif
#endif

#ifdef __C51__
typedef bit bdata                     BOOL1;
#else
#define data
#define idata
#define xdata
typedef unsigned char                 BOOL1;
#endif
#ifndef    BOOL
typedef    unsigned char              BOOL;
#endif
#ifndef    TRUE
#define    TRUE 1
#define    FALSE 0
#endif
#ifndef UINT8
typedef unsigned char                 UINT8;
#endif
#ifndef UINT16
typedef unsigned short                UINT16;
#endif
#ifndef UINT32
typedef unsigned long                 UINT32;
#endif
#ifndef PUINT8
typedef unsigned char                 *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short                *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long                 *PUINT32;
#endif
#ifndef UINT8V
typedef unsigned char volatile        UINT8V;
#endif

void CH438Q_GPIO_Config(void);
void WriteCH438Data( unsigned char addr, unsigned char dat);
unsigned char ReadCH438Data( unsigned char addr );
void WriteCH438Block( unsigned char mAddr, unsigned char mLen, unsigned char *mBuf ) ;
/******************************************************************************************/
#define CH438_WR_H() GPIO_SetBits(GPIOE, GPIO_Pin_9)
#define CH438_WR_L() GPIO_ResetBits(GPIOE, GPIO_Pin_9)
#define CH438_RD_H() GPIO_SetBits(GPIOE, GPIO_Pin_8)
#define CH438_RD_L() GPIO_ResetBits(GPIOE, GPIO_Pin_8)


#define CH438_OUT 0x01
#define CH438_IN  0x00

/* оƬ���� */
/* CH438����0�Ĵ�����ַ */

#define REG_RBR_ADDR        0x00      /* ����0���ջ���Ĵ�����ַ */
#define REG_THR_ADDR        0x00      /* ����0���ͱ��ּĴ�����ַ */
#define REG_IER_ADDR        0x01      /* ����0�ж�ʹ�ܼĴ�����ַ */
#define REG_IIR_ADDR        0x02      /* ����0�ж�ʶ��Ĵ�����ַ */
#define REG_FCR_ADDR        0x02      /* ����0FIFO���ƼĴ�����ַ */
#define REG_LCR_ADDR        0x03      /* ����0��·���ƼĴ�����ַ */
#define REG_MCR_ADDR        0x04      /* ����0MODEM���ƼĴ�����ַ */
#define REG_LSR_ADDR        0x05      /* ����0��·״̬�Ĵ�����ַ */
#define REG_MSR_ADDR        0x06      /* ����0MODEM״̬�Ĵ�����ַ */
#define REG_SCR_ADDR        0x07      /* ����0�û��ɶ���Ĵ�����ַ */
#define REG_DLL_ADDR        0x00      /* �����ʳ�����������8λ�ֽڵ�ַ */
#define REG_DLM_ADDR        0x01      /* �����ʳ�����������8λ�ֽڵ�ַ */

/* CH438����1�Ĵ�����ַ */

#define REG_RBR1_ADDR       0x10      /* ����1���ջ���Ĵ�����ַ */
#define REG_THR1_ADDR       0x10      /* ����1���ͱ��ּĴ�����ַ */
#define REG_IER1_ADDR       0x11      /* ����1�ж�ʹ�ܼĴ�����ַ */
#define REG_IIR1_ADDR       0x12      /* ����1�ж�ʶ��Ĵ�����ַ */
#define REG_FCR1_ADDR       0x12      /* ����1FIFO���ƼĴ�����ַ */
#define REG_LCR1_ADDR       0x13      /* ����1��·���ƼĴ�����ַ */
#define REG_MCR1_ADDR       0x14      /* ����1MODEM���ƼĴ�����ַ */
#define REG_LSR1_ADDR       0x15      /* ����1��·״̬�Ĵ�����ַ */
#define REG_MSR1_ADDR       0x16      /* ����1MODEM״̬�Ĵ�����ַ */
#define REG_SCR1_ADDR       0x17      /* ����1�û��ɶ���Ĵ�����ַ */
#define REG_DLL1_ADDR       0x10      /* �����ʳ�����������8λ�ֽڵ�ַ */
#define REG_DLM1_ADDR       0x11      /* �����ʳ�����������8λ�ֽڵ�ַ */


/* CH438����2�Ĵ�����ַ */

#define REG_RBR2_ADDR       0x20      /* ����2���ջ���Ĵ�����ַ */
#define REG_THR2_ADDR       0x20      /* ����2���ͱ��ּĴ�����ַ */
#define REG_IER2_ADDR       0x21      /* ����2�ж�ʹ�ܼĴ�����ַ */
#define REG_IIR2_ADDR       0x22      /* ����2�ж�ʶ��Ĵ�����ַ */
#define REG_FCR2_ADDR       0x22      /* ����2FIFO���ƼĴ�����ַ */
#define REG_LCR2_ADDR       0x23      /* ����2��·���ƼĴ�����ַ */
#define REG_MCR2_ADDR       0x24      /* ����2MODEM���ƼĴ�����ַ */
#define REG_LSR2_ADDR       0x25      /* ����2��·״̬�Ĵ�����ַ */
#define REG_MSR2_ADDR       0x26      /* ����2MODEM״̬�Ĵ�����ַ */
#define REG_SCR2_ADDR       0x27      /* ����2�û��ɶ���Ĵ�����ַ */
#define REG_DLL2_ADDR       0x20      /* �����ʳ�����������8λ�ֽڵ�ַ */
#define REG_DLM2_ADDR       0x21      /* �����ʳ�����������8λ�ֽڵ�ַ */



/* CH438����3�Ĵ�����ַ */

#define REG_RBR3_ADDR       0x30      /* ����3���ջ���Ĵ�����ַ */
#define REG_THR3_ADDR       0x30      /* ����3���ͱ��ּĴ�����ַ */
#define REG_IER3_ADDR       0x31      /* ����3�ж�ʹ�ܼĴ�����ַ */
#define REG_IIR3_ADDR       0x32      /* ����3�ж�ʶ��Ĵ�����ַ */
#define REG_FCR3_ADDR       0x32      /* ����3FIFO���ƼĴ�����ַ */
#define REG_LCR3_ADDR       0x33      /* ����3��·���ƼĴ�����ַ */
#define REG_MCR3_ADDR       0x34      /* ����3MODEM���ƼĴ�����ַ */
#define REG_LSR3_ADDR       0x35      /* ����3��·״̬�Ĵ�����ַ */
#define REG_MSR3_ADDR       0x36      /* ����3MODEM״̬�Ĵ�����ַ */
#define REG_SCR3_ADDR       0x37      /* ����3�û��ɶ���Ĵ�����ַ */
#define REG_DLL3_ADDR       0x30      /* �����ʳ�����������8λ�ֽڵ�ַ */
#define REG_DLM3_ADDR       0x31      /* �����ʳ�����������8λ�ֽڵ�ַ */


/* CH438����4�Ĵ�����ַ */

#define REG_RBR4_ADDR       0x08      /* ����4���ջ���Ĵ�����ַ */
#define REG_THR4_ADDR       0x08      /* ����4���ͱ��ּĴ�����ַ */
#define REG_IER4_ADDR       0x09      /* ����4�ж�ʹ�ܼĴ�����ַ */
#define REG_IIR4_ADDR       0x0A      /* ����4�ж�ʶ��Ĵ�����ַ */
#define REG_FCR4_ADDR       0x0A      /* ����4FIFO���ƼĴ�����ַ */
#define REG_LCR4_ADDR       0x0B      /* ����4��·���ƼĴ�����ַ */
#define REG_MCR4_ADDR       0x0C      /* ����4MODEM���ƼĴ�����ַ */
#define REG_LSR4_ADDR       0x0D      /* ����4��·״̬�Ĵ�����ַ */
#define REG_MSR4_ADDR       0x0E      /* ����4MODEM״̬�Ĵ�����ַ */
#define REG_SCR4_ADDR       0x0F      /* ����4�û��ɶ���Ĵ�����ַ */
#define REG_DLL4_ADDR       0x08      /* �����ʳ�����������8λ�ֽڵ�ַ */
#define REG_DLM4_ADDR       0x09      /* �����ʳ�����������8λ�ֽڵ�ַ */



/* CH438����5�Ĵ�����ַ */

#define REG_RBR5_ADDR       0x18      /* ����5���ջ���Ĵ�����ַ */
#define REG_THR5_ADDR       0x18      /* ����5���ͱ��ּĴ�����ַ */
#define REG_IER5_ADDR       0x19      /* ����5�ж�ʹ�ܼĴ�����ַ */
#define REG_IIR5_ADDR       0x1A      /* ����5�ж�ʶ��Ĵ�����ַ */
#define REG_FCR5_ADDR       0x1A      /* ����5FIFO���ƼĴ�����ַ */
#define REG_LCR5_ADDR       0x1B      /* ����5��·���ƼĴ�����ַ */
#define REG_MCR5_ADDR       0x1C      /* ����5MODEM���ƼĴ�����ַ */
#define REG_LSR5_ADDR       0x1D      /* ����5��·״̬�Ĵ�����ַ */
#define REG_MSR5_ADDR       0x1E      /* ����5MODEM״̬�Ĵ�����ַ */
#define REG_SCR5_ADDR       0x1F      /* ����5�û��ɶ���Ĵ�����ַ */
#define REG_DLL5_ADDR       0x18      /* �����ʳ�����������8λ�ֽڵ�ַ */
#define REG_DLM5_ADDR       0x19      /* �����ʳ�����������8λ�ֽڵ�ַ */


/* CH438����6�Ĵ�����ַ */

#define REG_RBR6_ADDR       0x28      /* ����6���ջ���Ĵ�����ַ */
#define REG_THR6_ADDR       0x28      /* ����6���ͱ��ּĴ�����ַ */
#define REG_IER6_ADDR       0x29      /* ����6�ж�ʹ�ܼĴ�����ַ */
#define REG_IIR6_ADDR       0x2A      /* ����6�ж�ʶ��Ĵ�����ַ */
#define REG_FCR6_ADDR       0x2A      /* ����6FIFO���ƼĴ�����ַ */
#define REG_LCR6_ADDR       0x2B      /* ����6��·���ƼĴ�����ַ */
#define REG_MCR6_ADDR       0x2C      /* ����6MODEM���ƼĴ�����ַ */
#define REG_LSR6_ADDR       0x2D      /* ����6��·״̬�Ĵ�����ַ */
#define REG_MSR6_ADDR       0x2E      /* ����6MODEM״̬�Ĵ�����ַ */
#define REG_SCR6_ADDR       0x2F      /* ����6�û��ɶ���Ĵ�����ַ */
#define REG_DLL6_ADDR       0x28      /* �����ʳ�����������8λ�ֽڵ�ַ */
#define REG_DLM6_ADDR       0x29      /* �����ʳ�����������8λ�ֽڵ�ַ */


/* CH438����7�Ĵ�����ַ */

#define REG_RBR7_ADDR       0x38      /* ����7���ջ���Ĵ�����ַ */
#define REG_THR7_ADDR       0x38      /* ����7���ͱ��ּĴ�����ַ */
#define REG_IER7_ADDR       0x39      /* ����7�ж�ʹ�ܼĴ�����ַ */
#define REG_IIR7_ADDR       0x3A      /* ����7�ж�ʶ��Ĵ�����ַ */
#define REG_FCR7_ADDR       0x3A      /* ����7FIFO���ƼĴ�����ַ */
#define REG_LCR7_ADDR       0x3B      /* ����7��·���ƼĴ�����ַ */
#define REG_MCR7_ADDR       0x3C      /* ����7MODEM���ƼĴ�����ַ */
#define REG_LSR7_ADDR       0x3D      /* ����7��·״̬�Ĵ�����ַ */
#define REG_MSR7_ADDR       0x3E      /* ����7MODEM״̬�Ĵ�����ַ */
#define REG_SCR7_ADDR       0x3F      /* ����7�û��ɶ���Ĵ�����ַ */
#define REG_DLL7_ADDR       0x38      /* �����ʳ�����������8λ�ֽڵ�ַ */
#define REG_DLM7_ADDR       0x39      /* �����ʳ�����������8λ�ֽڵ�ַ */



/* CH438�ڲ�����0~7 ר��״̬�Ĵ��� */
#define REG_SSR_ADDR        0x4F       /* ר��״̬�Ĵ�����ַ */


/* IER�Ĵ�����λ */

#define BIT_IER_RESET       0x80      /* ��λ��1������λ�ô��� */
#define BIT_IER_LOWPOWER    0x40      /* ��λΪ1��رոô��ڵ��ڲ���׼ʱ�� */
#define BIT_IER_SLP         0x20      /* ����0��SLP,Ϊ1��ر�ʱ������ */
#define BIT_IER1_CK2X       0x20      /* ����1��CK2X,Ϊ1��ǿ�ƽ��ⲿʱ���ź�2��Ƶ����Ϊ�ڲ���׼ʱ�� */
#define BIT_IER_IEMODEM     0x08      /* ��λΪ1����MODEM����״̬�仯�ж� */
#define BIT_IER_IELINES     0x04      /* ��λΪ1����������·״̬�ж� */
#define BIT_IER_IETHRE      0x02      /* ��λΪ1�������ͱ��ּĴ������ж� */
#define BIT_IER_IERECV      0x01      /* ��λΪ1�������յ������ж� */

/* IIR�Ĵ�����λ */

#define BIT_IIR_FIFOENS1    0x80
#define BIT_IIR_FIFOENS0    0x40      /* ��2λΪ1��ʾ����FIFO */

/* �ж����ͣ�0001û���жϣ�0110������·״̬�жϣ�0100�������ݿ����жϣ�
1100�������ݳ�ʱ�жϣ�0010THR�Ĵ������жϣ�0000MODEM����仯�ж� */
#define BIT_IIR_IID3        0x08
#define BIT_IIR_IID2        0x04
#define BIT_IIR_IID1        0x02
#define BIT_IIR_NOINT       0x01

/* FCR�Ĵ�����λ */

/* �����㣺 00��Ӧ1���ֽڣ�01��Ӧ16���ֽڣ�10��Ӧ64���ֽڣ�11��Ӧ112���ֽ� */
#define BIT_FCR_RECVTG1     0x80      /* ����FIFO���жϺ��Զ�Ӳ�������ƵĴ����� */
#define BIT_FCR_RECVTG0     0x40      /* ����FIFO���жϺ��Զ�Ӳ�������ƵĴ����� */

#define BIT_FCR_TFIFORST    0x04      /* ��λ��1����շ���FIFO�е����� */
#define BIT_FCR_RFIFORST    0x02      /* ��λ��1����ս���FIFO�е����� */
#define BIT_FCR_FIFOEN      0x01      /* ��λ��1������FIFO,Ϊ0�����FIFO */

/* LCR�Ĵ�����λ */

#define BIT_LCR_DLAB        0x80      /* Ϊ1���ܴ�ȡDLL��DLM��Ϊ0���ܴ�ȡRBR/THR/IER */
#define BIT_LCR_BREAKEN     0x40      /* Ϊ1��ǿ�Ʋ���BREAK��·���*/

/* ����У���ʽ����PARENΪ1ʱ��00��У�飬01żУ�飬10��־λ��MARK����1)��11�հ�λ��SPACE����0) */
#define BIT_LCR_PARMODE1    0x20      /* ������żУ��λ��ʽ */
#define BIT_LCR_PARMODE0    0x10      /* ������żУ��λ��ʽ */

#define BIT_LCR_PAREN       0x08      /* Ϊ1����������ʱ�����ͽ���У����żУ��λ */
#define BIT_LCR_STOPBIT     0x04      /* Ϊ1������ֹͣλ,Ϊ0һ��ֹͣλ */

/* �����ֳ��ȣ�00��5������λ��01��6������λ��10��7������λ��11��8������λ */
#define BIT_LCR_WORDSZ1     0x02      /* �����ֳ����� */
#define BIT_LCR_WORDSZ0     0x01

/* MCR�Ĵ�����λ */

#define BIT_MCR_AFE         0x20      /* Ϊ1����CTS��RTSӲ���Զ������� */
#define BIT_MCR_LOOP        0x10      /* Ϊ1ʹ���ڲ���·�Ĳ���ģʽ */
#define BIT_MCR_OUT2        0x08      /* Ϊ1�����ô��ڵ��ж�������� */
#define BIT_MCR_OUT1        0x04      /* Ϊ�û������MODEM����λ */
#define BIT_MCR_RTS         0x02      /* ��λΪ1��RTS���������Ч */
#define BIT_MCR_DTR         0x01      /* ��λΪ1��DTR���������Ч */

/* LSR�Ĵ�����λ */

#define BIT_LSR_RFIFOERR    0x80      /* Ϊ1��ʾ�ڽ���FIFO�д�������һ������ */
#define BIT_LSR_TEMT        0x40      /* Ϊ1��ʾTHR��TSRȫ�� */
#define BIT_LSR_THRE        0x20      /* Ϊ1��ʾTHR��*/
#define BIT_LSR_BREAKINT    0x10      /* ��λΪ1��ʾ��⵽BREAK��·��� */
#define BIT_LSR_FRAMEERR    0x08      /* ��λΪ1��ʾ��ȡ����֡���� */
#define BIT_LSR_PARERR      0x04      /* ��λΪ1��ʾ��żУ����� */
#define BIT_LSR_OVERR       0x02      /* Ϊ1��ʾ����FIFO��������� */
#define BIT_LSR_DATARDY     0x01      /* ��λΪ1��ʾ����FIFO���н��յ������� */

/* MSR�Ĵ�����λ */

#define BIT_MSR_DCD         0x80      /* ��λΪ1��ʾDCD������Ч */
#define BIT_MSR_RI          0x40      /* ��λΪ1��ʾRI������Ч */
#define BIT_MSR_DSR         0x20      /* ��λΪ1��ʾDSR������Ч */
#define BIT_MSR_CTS         0x10      /* ��λΪ1��ʾCTS������Ч */
#define BIT_MSR_DDCD        0x08      /* ��λΪ1��ʾDCD��������״̬�����仯�� */
#define BIT_MSR_TERI        0x04      /* ��λΪ1��ʾRI��������״̬�����仯�� */
#define BIT_MSR_DDSR        0x02      /* ��λΪ1��ʾDSR��������״̬�����仯�� */
#define BIT_MSR_DCTS        0x01      /* ��λΪ1��ʾCTS��������״̬�����仯�� */

/* �ж�״̬�� */

#define INT_NOINT           0x01      /* û���ж� */
#define INT_THR_EMPTY       0x02      /* THR���ж� */
#define INT_RCV_OVERTIME    0x0C      /* ���ճ�ʱ�ж� */
#define INT_RCV_SUCCESS     0x04      /* �������ݿ����ж� */
#define INT_RCV_LINES       0x06      /* ������·״̬�ж� */
#define INT_MODEM_CHANGE    0x00      /* MODEM����仯�ж� */

#define CH438_IIR_FIFOS_ENABLED 0xC0  /* ����FIFO */

/*�����ֳ���ֹͣλ��У��λ����
WriteCH438Data( CH438_LCR_PORT, BIT_LCR_WORDSZ1 
                              | BIT_LCR_WORDSZ0 );      // 0x03,�ֳ�8λ��1λֹͣλ,����żУ��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT 
                              | BIT_LCR_WORDSZ1 
                              | BIT_LCR_WORDSZ0 );      // 0x0F,�ֳ�8λ��2λֹͣλ,��У��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT 
                              | BIT_LCR_WORDSZ1 );      // 0x0E,�ֳ�7λ��2λֹͣλ����У��


WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT 
                              | BIT_LCR_WORDSZ0 );      // 0x0D,�ֳ�6λ��2λֹͣλ����У��


WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT );      // 0x0C,�ֳ�5λ��2λֹͣλ����У��


WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE0 
                              | BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT 
                              | BIT_LCR_WORDSZ1 
                              | BIT_LCR_WORDSZ0 );      // 0x1F,�ֳ�8λ��2λֹͣλ��żУ��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE0 
                              | BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT 
                              | BIT_LCR_WORDSZ1 );      // 0x1E,�ֳ�7λ��2λֹͣλ��żУ��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE0 
                              | BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT 
                              | BIT_LCR_WORDSZ0 );      // 0x1D,�ֳ�6λ��2λֹͣλ��żУ��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE0 
                              | BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT );      // 0x1C,�ֳ�5λ��2λֹͣλ��żУ��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE1 
                              | BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT 
                              | BIT_LCR_WORDSZ1 
                              | BIT_LCR_WORDSZ0 );      // 0x2F,�ֳ�8λ��2λֹͣλ����־1У��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE1 
                              | BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT 
                              | BIT_LCR_WORDSZ1 );      // 0x2E,�ֳ�7λ��2λֹͣλ����־1У��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE1 
                              | BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT 
                              | BIT_LCR_WORDSZ0 );      // 0x2D,�ֳ�6λ��2λֹͣλ����־1У��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE1 
                              | BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT );      // 0x2C,�ֳ�5λ��2λֹͣλ����־1У��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE1 
                              | BIT_LCR_PARMODE0
                              | BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT 
                              | BIT_LCR_WORDSZ1 
                              | BIT_LCR_WORDSZ0 );      // 0x3F,�ֳ�8λ��2λֹͣλ���հ�0У��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE1 
                              | BIT_LCR_PARMODE0
                              | BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT 
                              | BIT_LCR_WORDSZ1 );      // 0x3E,�ֳ�7λ��2λֹͣλ���հ�0У��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE1
                              | BIT_LCR_PARMODE0
                              | BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT 
                              | BIT_LCR_WORDSZ0 );      // 0x3D,�ֳ�6λ��2λֹͣλ���հ�0У��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE1
                              | BIT_LCR_PARMODE0
                              | BIT_LCR_PAREN 
                              | BIT_LCR_STOPBIT );      // 0x3C,�ֳ�5λ��2λֹͣλ���հ�0У��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE0 
                              | BIT_LCR_PAREN 
                              | BIT_LCR_WORDSZ1 
                              | BIT_LCR_WORDSZ0 );      // 0x1B,�ֳ�8λ��1λֹͣλ��żУ��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE0 
                              | BIT_LCR_PAREN 
                              | BIT_LCR_WORDSZ1 );      // 0x1A,�ֳ�7λ��1λֹͣλ��żУ��


WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE0 
                              | BIT_LCR_PAREN 
                              | BIT_LCR_WORDSZ0 );      // 0x19,�ֳ�6λ��1λֹͣλ��żУ��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_PARMODE0 
                              | BIT_LCR_PAREN );      // 0x18,�ֳ�5λ��1λֹͣλ��żУ��

WriteCH438Data( CH438_LCR_PORT, BIT_LCR_STOPBIT 
                              | BIT_LCR_WORDSZ1 
                              | BIT_LCR_WORDSZ0 );      // 0x07,�ֳ�8λ��2λֹͣλ,����żУ��
*/







#endif