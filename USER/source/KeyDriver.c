/******************************************************
��������˵����
1���ܹ�5���������ֱ�Ϊ���ҡ��ϡ��¡�ȷ��
2���������������ж̰�������
3�������ַ�Ϊ�������ͣ������͵��Σ�
4�����ҡ��ϡ��³���Ϊ������ȷ������Ϊ����
******************************************************/
#include "stm32f10x.h"
#include "KeyDriver.h"
/*
�÷�:
1.ÿ20ms~50ms����KeyScan()ɨ�谴��
2.����KeyHit()�ж��Ƿ��а���
3.����KeyGet()��ð���ֵ
*/
static unsigned char fg_ucPreScanKey  = 0;               //ǰ�ΰ���ɨ��ֵ      
static unsigned char fg_ucPreReadKey  = 0;               //ǰ�ΰ�����ȡֵ        
static unsigned char fg_ucKeyShift    = 0;               //shift������¼
static unsigned char fg_ucKeyMask     = 0;               //��������
#if LONG_KEY_EN > 0
static  unsigned char fg_ucKeyPressTmr = KEY_PRESS_DLY; //�������ж�����
#endif
static unsigned char  fg_ucKeyBuf[KEY_BUF_SIZE];         //����buf
static unsigned char  fg_ucKeyBufInIx  = 0;              //buf��ָ��                   
static unsigned char  fg_ucKeyBufOutIx = 0;              //buf��ָ��
static unsigned char  fg_ucKeyNRead    = 0;              //buf�а�������


/*******************************************************************************
* Function Name : Key_GPIO_Config
* Description   : ����I/O ��ʼ�� ������ʱΪ�͵�ƽ                
*******************************************************************************/
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  /* Configure in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3| GPIO_Pin_4| GPIO_Pin_5| GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	//��PB3����Ϊ��ͨIO��Ĭ��ΪJTAG��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB , ENABLE); 
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	
}
/*******************************************************************************
* Function Name : KeyIOread
* Description   : �жϰ����Ƿ��£����·��ذ���ֵ               
*******************************************************************************/
unsigned char  KeyIOread( void )
{
  unsigned char KeyScanCode = 0;
  KeyScanCode |= KEY_UP()    ? 0 : UP;
  KeyScanCode |= KEY_DOWN()  ? 0 : DOWN;
//  KeyScanCode |= KEY_LEFT()  ? 0 : LEFT;
//  KeyScanCode |= KEY_RIGHT() ? 0 : RIGHT;
  KeyScanCode |= KEY_ENTER() ? 0 : ENTER;
	KeyScanCode |= KEY_ESC() ? 0 : ESC;
  return KeyScanCode;
}
/*******************************************************************************
* Function Name : KeyBufOut
* Description   : ��buf��ȡ��һ��ɨ����                
*******************************************************************************/
static unsigned char KeyBufOut (void)
{
  unsigned char code = 0;
  if(fg_ucKeyNRead > 0){                           //�鿴�Ƿ��а�����buf��                                             
    fg_ucKeyNRead--;                               //-1��ʾȡ��һ������ֵ
    code = fg_ucKeyBuf[fg_ucKeyBufOutIx];          //��buf��ȡ��һ��ɨ����
    fg_ucKeyBufOutIx++;
    if(fg_ucKeyBufOutIx >= KEY_BUF_SIZE){          //buf����ָ��ѭ���ص����               
      fg_ucKeyBufOutIx = 0;
    }                     
    return (code);                            //���ذ���ɨ��ֵ
  }else{                      
    return (0);                            		//û�а���,�򷵻�0
  }
}
/*******************************************************************************
* Function Name : KeyBufIn
* Description   : ��buf��д��һ��ɨ����                
*******************************************************************************/
static void  KeyBufIn (unsigned char code)
{                        
    if(fg_ucKeyNRead >= KEY_BUF_SIZE){                //buf������������һ������ֵ                   
      KeyBufOut();                                                 
    }                  
    fg_ucKeyNRead++;                              
    fg_ucKeyBuf[fg_ucKeyBufInIx++] = code;           //���밴��ɨ����
    if(fg_ucKeyBufInIx >= KEY_BUF_SIZE){             //buf����ָ��ѭ���ص����           
      fg_ucKeyBufInIx = 0;
    }                      
}

/*************************************************************************************************************
�������ƣ�KeyScan()
������ڣ���
�������ڣ���
����˵����
1.����˵��
  a.��������
  b.��׽�������Ͷ̰���
  c.�������ý������ֳɶ̰���/����������/�̰�shift��/����shift��4�ַ�ʽд��buf
2.�㷨˵��
  a.������ȡ�㷨
    1).
: ��ƽ���� 
    2).NowKey ^ PreKey                                : ��Ե���� 
    3).NowKey & (NowKey ^ PreKey)��(~PreKey) & NowKey : �����ش���
    4).PreKey & (NowKey ^ PreKey)��PreKey & (~NowKey) : �½��ش���
  b.�˲��㷨
    1).PreScanKey & NowScanKey                        : ��ƽ����
    2).PreReadKey & (PreScanKey ^ NowScanKey)         : ��������
    3).NowReadKey = 1) | 2)                           : ���������ֵĵ�ƽ����
3.����˵��
  a.���µ��õ�KeyIOread()��,��Ч��������Ϊ�ߵ�ƽ,��ÿ��bit��ʾһ������ֵ
  b.Ӧ�õ��øú����ļ��Ӧ����10ms~20ms���ڵ��ü���ڵ�ë�̾����˳���
*************************************************************************************************************/
void KeyScan(void) 
{
  unsigned char ucCurrentScanKey = 0;                           //��ǰ����ֵɨ��ֵ
  unsigned char ucCurrentReadKey = 0;                           //��ǰ����ֵ
//KEY_TYPE KeyPressDown = 0;                                    //��������                                    
  unsigned char ucKeyRelease = 0;                               //�����ͷ�                                                           
  ucCurrentScanKey = KeyIOread();
  ucCurrentReadKey = (fg_ucPreScanKey & ucCurrentScanKey)|                      //�����ٴε�ƽ����
                      (fg_ucPreReadKey & (fg_ucPreScanKey^ucCurrentScanKey));   //��������(������) 
//KeyPressDown  = NowReadKey & (NowReadKey ^ PreReadKey);                       //�����ش���  
  ucKeyRelease = fg_ucPreReadKey & (ucCurrentReadKey ^ fg_ucPreReadKey);        //�½��ش���(�ǰ����½��ش���) �а���ֵΪ1  

#if LONG_KEY_EN > 0                                         
  if(ucCurrentReadKey == fg_ucPreReadKey && ucCurrentReadKey){    //�õ�ƽ����������������Ч�о�
    fg_ucKeyPressTmr--;
    if(!fg_ucKeyPressTmr){                                        //�����ж����ڵ�,������Ӧ������ֵ
      if(ucCurrentReadKey & ~(KEY_LONG_SHIFT)){                   //������ģʽһ  #define KEY_LONG_SHIFT = ENTER|DOWN
        KeyBufIn(ucCurrentReadKey | fg_ucKeyShift);               //�������ظ����
      }
      else if(ucCurrentReadKey & (KEY_LONG_SHIFT) & ~fg_ucKeyMask ){   //������ģʽ��        
        KeyBufIn(~(ucCurrentReadKey | fg_ucKeyShift));                 //���������������Ϊ�ڶ����ܼ�      
      }
      fg_ucKeyPressTmr = KEY_PRESS_TMR;                      //������������,׼����ȡ��1��������
      fg_ucKeyMask = ucCurrentReadKey;
    }
  }
  else{
    fg_ucKeyPressTmr = KEY_PRESS_DLY;                        //�����仯,���ð����ж�����
  }
#endif
  if(ucKeyRelease){                                          //�̰����ж� 
      if(ucKeyRelease &(~fg_ucKeyMask)){ 
        fg_ucKeyShift ^= (ucKeyRelease & (KEY_SHORT_SHIFT)); //shift������(��Ե����) #define KEY_SHORT_SHIFT KEY_ESC|ENTER
        KeyBufIn(ucKeyRelease | fg_ucKeyShift);
      }else{
        fg_ucKeyMask = 0;
      }
  } 
  fg_ucPreScanKey = ucCurrentScanKey;
  fg_ucPreReadKey = ucCurrentReadKey; 
}

/*************************************************************************************************************
�������ƣ�KeyGet()
������ڣ���
�������ڣ����ذ���ֵ���ް�������0xFF
����˵������buf�л��һ������ֵ
*************************************************************************************************************/
unsigned char  KeyGet(void)
{                         
  return (unsigned char)KeyBufOut();
}     

/*************************************************************************************************************
�������ƣ�KeyHit()
������ڣ���
�������ڣ��а���ʱ����TRUE,�ް���ʱ����FALSE
����˵�����ж��Ƿ��а�������
*************************************************************************************************************/
unsigned char  KeyHit(void)
{
    unsigned char hit = 0;
    hit = (unsigned char)(fg_ucKeyNRead > 0) ? 1 : 0;
    return (hit);
}

/*************************************************************************************************************
�������ƣ�KeyFlush()
������ڣ���
�������ڣ���
����˵�������buff�����а���ֵ
*************************************************************************************************************/
void KeyFlush(void)
{
  while(KeyGet());
}

/**********************************/
unsigned char  KeyTask(void)
{
    unsigned char hit = 0;
    hit = (unsigned char)(fg_ucKeyNRead > 0) ? 1 : 0;
    return (hit);
}





