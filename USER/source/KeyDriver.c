/******************************************************
按键操作说明：
1、总共5个按键，分别为左、右、上、下、确定
2、按键触发类型有短按，长按
3、长按又分为两种类型（连击和单次）
4、左、右、上、下长按为连击，确定长按为单次
******************************************************/
#include "stm32f10x.h"
#include "KeyDriver.h"
/*
用法:
1.每20ms~50ms调用KeyScan()扫描按键
2.调用KeyHit()判断是否有按键
3.调用KeyGet()获得按键值
*/
static unsigned char fg_ucPreScanKey  = 0;               //前次按键扫描值      
static unsigned char fg_ucPreReadKey  = 0;               //前次按键读取值        
static unsigned char fg_ucKeyShift    = 0;               //shift按键记录
static unsigned char fg_ucKeyMask     = 0;               //按键掩码
#if LONG_KEY_EN > 0
static  unsigned char fg_ucKeyPressTmr = KEY_PRESS_DLY; //长按键判断周期
#endif
static unsigned char  fg_ucKeyBuf[KEY_BUF_SIZE];         //环形buf
static unsigned char  fg_ucKeyBufInIx  = 0;              //buf入指针                   
static unsigned char  fg_ucKeyBufOutIx = 0;              //buf出指针
static unsigned char  fg_ucKeyNRead    = 0;              //buf中按键数量


/*******************************************************************************
* Function Name : Key_GPIO_Config
* Description   : 按键I/O 初始化 ，按下时为低电平                
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

	//将PB3配置为普通IO（默认为JTAG）
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB , ENABLE); 
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	
}
/*******************************************************************************
* Function Name : KeyIOread
* Description   : 判断按键是否按下，按下返回按键值               
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
* Description   : 从buf中取走一个扫描码                
*******************************************************************************/
static unsigned char KeyBufOut (void)
{
  unsigned char code = 0;
  if(fg_ucKeyNRead > 0){                           //查看是否有按键在buf中                                             
    fg_ucKeyNRead--;                               //-1表示取走一个按键值
    code = fg_ucKeyBuf[fg_ucKeyBufOutIx];          //从buf中取走一个扫描码
    fg_ucKeyBufOutIx++;
    if(fg_ucKeyBufOutIx >= KEY_BUF_SIZE){          //buf满则指针循环回到起点               
      fg_ucKeyBufOutIx = 0;
    }                     
    return (code);                            //返回按键扫描值
  }else{                      
    return (0);                            		//没有按键,则返回0
  }
}
/*******************************************************************************
* Function Name : KeyBufIn
* Description   : 在buf中写入一个扫描码                
*******************************************************************************/
static void  KeyBufIn (unsigned char code)
{                        
    if(fg_ucKeyNRead >= KEY_BUF_SIZE){                //buf满则放弃最早的一个按键值                   
      KeyBufOut();                                                 
    }                  
    fg_ucKeyNRead++;                              
    fg_ucKeyBuf[fg_ucKeyBufInIx++] = code;           //输入按键扫描码
    if(fg_ucKeyBufInIx >= KEY_BUF_SIZE){             //buf满则指针循环回到起点           
      fg_ucKeyBufInIx = 0;
    }                      
}

/*************************************************************************************************************
函数名称：KeyScan()
函数入口：无
函数出口：无
函数说明：
1.功能说明
  a.按键消抖
  b.捕捉长按键和短按键
  c.根据设置将按键分成短按键/长按连续建/短按shift键/长按shift键4种方式写入buf
2.算法说明
  a.按键获取算法
    1).
: 电平触发 
    2).NowKey ^ PreKey                                : 边缘触发 
    3).NowKey & (NowKey ^ PreKey)或(~PreKey) & NowKey : 上升沿触发
    4).PreKey & (NowKey ^ PreKey)或PreKey & (~NowKey) : 下降沿触发
  b.滤波算法
    1).PreScanKey & NowScanKey                        : 电平触发
    2).PreReadKey & (PreScanKey ^ NowScanKey)         : 采样保持
    3).NowReadKey = 1) | 2)                           : 带采样保持的电平触发
3.调用说明
  a.对下调用的KeyIOread()中,有效按键必须为高电平,且每个bit表示一个按键值
  b.应用调用该函数的间隔应该在10ms~20ms，在调用间隔内的毛刺均可滤除。
*************************************************************************************************************/
void KeyScan(void) 
{
  unsigned char ucCurrentScanKey = 0;                           //当前按键值扫描值
  unsigned char ucCurrentReadKey = 0;                           //当前按键值
//KEY_TYPE KeyPressDown = 0;                                    //按键按下                                    
  unsigned char ucKeyRelease = 0;                               //按键释放                                                           
  ucCurrentScanKey = KeyIOread();
  ucCurrentReadKey = (fg_ucPreScanKey & ucCurrentScanKey)|                      //连续再次电平触发
                      (fg_ucPreReadKey & (fg_ucPreScanKey^ucCurrentScanKey));   //采样保持(即消抖) 
//KeyPressDown  = NowReadKey & (NowReadKey ^ PreReadKey);                       //上升沿触发  
  ucKeyRelease = fg_ucPreReadKey & (ucCurrentReadKey ^ fg_ucPreReadKey);        //下降沿触发(非按键下降沿触发) 有按键值为1  

#if LONG_KEY_EN > 0                                         
  if(ucCurrentReadKey == fg_ucPreReadKey && ucCurrentReadKey){    //用电平触发做长按键的有效判据
    fg_ucKeyPressTmr--;
    if(!fg_ucKeyPressTmr){                                        //长按判断周期到,保存相应长按键值
      if(ucCurrentReadKey & ~(KEY_LONG_SHIFT)){                   //长按键模式一  #define KEY_LONG_SHIFT = ENTER|DOWN
        KeyBufIn(ucCurrentReadKey | fg_ucKeyShift);               //长按键重复输出
      }
      else if(ucCurrentReadKey & (KEY_LONG_SHIFT) & ~fg_ucKeyMask ){   //长按键模式二        
        KeyBufIn(~(ucCurrentReadKey | fg_ucKeyShift));                 //长按键反码输出作为第二功能键      
      }
      fg_ucKeyPressTmr = KEY_PRESS_TMR;                      //重置连按周期,准备获取下1个长按键
      fg_ucKeyMask = ucCurrentReadKey;
    }
  }
  else{
    fg_ucKeyPressTmr = KEY_PRESS_DLY;                        //按键变化,重置按键判断周期
  }
#endif
  if(ucKeyRelease){                                          //短按键判断 
      if(ucKeyRelease &(~fg_ucKeyMask)){ 
        fg_ucKeyShift ^= (ucKeyRelease & (KEY_SHORT_SHIFT)); //shift按键码(边缘触发) #define KEY_SHORT_SHIFT KEY_ESC|ENTER
        KeyBufIn(ucKeyRelease | fg_ucKeyShift);
      }else{
        fg_ucKeyMask = 0;
      }
  } 
  fg_ucPreScanKey = ucCurrentScanKey;
  fg_ucPreReadKey = ucCurrentReadKey; 
}

/*************************************************************************************************************
函数名称：KeyGet()
函数入口：无
函数出口：返回按键值，无按键返回0xFF
函数说明：从buf中获得一个按键值
*************************************************************************************************************/
unsigned char  KeyGet(void)
{                         
  return (unsigned char)KeyBufOut();
}     

/*************************************************************************************************************
函数名称：KeyHit()
函数入口：无
函数出口：有按键时返回TRUE,无按键时返回FALSE
函数说明：判断是否有按键按下
*************************************************************************************************************/
unsigned char  KeyHit(void)
{
    unsigned char hit = 0;
    hit = (unsigned char)(fg_ucKeyNRead > 0) ? 1 : 0;
    return (hit);
}

/*************************************************************************************************************
函数名称：KeyFlush()
函数入口：无
函数出口：无
函数说明：清空buff中所有按键值
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





