#include "stm32f10x.h"
#include "main.h"
#include "System_Initialize.h"
#include "LoraC8000.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
 * ��ʼ���������Ź�
 * prer:��Ƶ��:0~7(ֻ�е� 3 λ��Ч!)
 * ��Ƶ����=4*2^prer.�����ֵֻ���� 256!
 * rlr:��װ�ؼĴ���ֵ:�� 11 λ��Ч.
 * ʱ�����(���):Tout=((4*2^prer)*rlr)/40 (ms).
 */
void IWDG_Init(u8 prer,u16 rlr)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); /* ʹ�ܶԼĴ���IWDG_PR��IWDG_RLR��д����*/
    IWDG_SetPrescaler(prer);    /*����IWDGԤ��Ƶֵ:����IWDGԤ��Ƶֵ*/
    IWDG_SetReload(rlr);        /*����IWDG��װ��ֵ*/
    IWDG_ReloadCounter();       /*����IWDG��װ�ؼĴ�����ֵ��װ��IWDG������*/
    IWDG_Enable();              /*ʹ��IWDG*/
}

/**
 * ι�������Ź�
 */
void IWDG_Feed(void)
{
    IWDG_ReloadCounter();    /*reload*/
}

/*Ӧ�þ���
		IWDG_Init(7,0xfff);//��ʼ�� 25s

		IWDG_Feed()��//  ι��
*/
/*******************************************************************************
  * @brief  ����ȡ��
  * @param  None
  * @retval None
*******************************************************************************/
void GPIO_PinReverse(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  /* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    assert_param(IS_GPIO_PIN(GPIO_Pin)); 
    GPIOx->ODR ^=  GPIO_Pin;
}
/*******************************************************************************
  * @brief  
  * @param  None
  * @retval None
*******************************************************************************/
void LED_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  /* Configure in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13| GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
}
/*******************************************************************************
  * @brief  
  * @param  None
  * @retval None
*******************************************************************************/
void BEEP_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  /* Configure in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/*******************************************************************************
  * @brief  
  * @param  None
  * @retval None
*******************************************************************************/
void RELAY_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  /* Configure in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	SELECT_250V();
}

/*******************************************************************************
  * @brief  
  * @param  None
  * @retval None
*******************************************************************************/
void TIM2_Configuration(void) 
{ 
    TIM_TimeBaseInitTypeDef TIM_BaseInitStructure; 
		NVIC_InitTypeDef     NVIC_InitStructure;
	
    TIM_DeInit(TIM2);                              //��λTIM2��ʱ�� 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   //ʹ�ܶ�ʱ��2   
    //TIM2 ʹ���ڲ�ʱ�� 
    //TIM_InternalClockConfig(TIM1); 
    //TIM2�������� 
    //����Ԥ��Ƶ����Ƶϵ��71����APB2=72M, TIM2_CLK=72/72=1MHz ����һ��1us
    //TIM_Period��TIM1_ARR��=1000�����������ϼ�����1000����������¼�������ֵ���� ��1ms
    //���ϼ���ģʽ 
    //TIM_RepetitionCounter(TIM1_RCR)=0��ÿ��������������������¼� 
    TIM_BaseInitStructure.TIM_Period = 1000;         //�����Զ����ؼĴ���ֵΪ���ֵ 0��65535֮��
    TIM_BaseInitStructure.TIM_Prescaler = 71;          //�Զ���Ԥ��Ƶ
    TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //����ʱ�ӷָ�
    TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   //���ϼ���ģʽ
    TIM_BaseInitStructure.TIM_RepetitionCounter = 0; 
    TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure); 
    
    //���жϣ�����һ�����жϺ����������ж� 
    TIM_ClearFlag(TIM2, TIM_FLAG_Update); 
    //ʹ��TIM2�ж�Դ 
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); 
    //TIM2�ܿ��أ����� 
    TIM_Cmd(TIM2, ENABLE); 
		
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;             //�����¼� 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;   //��ռ���ȼ�0 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          //��Ӧ���ȼ�1 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //�����ж� 
    NVIC_Init(&NVIC_InitStructure);                             //д������    
}


void AT24Cxx_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  /* Configure in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}


unsigned char CheckAT24CxxData(unsigned char *p,unsigned char ucNum)
{
  unsigned char ucCs=0,i;	
  if(*p !=0xaa){
    return 0;
  }
	for(i=0;i<(ucNum-1);i++){  
		ucCs += *p;
		p++;
	}
	
  if(ucCs != *p){
	  return 0;
	}
	return 1;
}
/*
 1.Start frame    1byte  0x00    //00 address set 0xaa 
 2.Device Code    1byte  0x01   
 3.Start ID       4bytes 0x02-0x05  
 4.ID number      2bytes 0x06-0x07 
 5.curent ID      4bytes 0x08-0x0b 
 6.The rest of ID 2bytes 0x0c-0x0d 
 7.check sum      1byte  0x0e  //   15bytes
*/
void IdData2Scm(ScmAT24CxxTypeDef *scm,unsigned char*p)
{
  unsigned char ucTemp[4] = {0x00};
	unsigned short int uiTempId = 0;
	unsigned char i;
  union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnData;	
	
	scm->ucDeviceCode = p[1];
	for(i = 0;i < 4;i++){
	  UnData.ucDataTable[i] = p[2+i];
	}
	scm->uiStartID = UnData.uiValue;
	uiTempId = p[6];
	scm->uiIDnumber = (uiTempId<<8) + p[7];
	for(i = 0;i < 4;i++){
	  UnData.ucDataTable[i] = p[8+i];
	}	
  scm->uiCurentID = UnData.uiValue;
	uiTempId = p[0x0c];
	scm->uiRestID = (uiTempId<<8) + p[0x0d];		
}

void IdScm2Array(ScmAT24CxxTypeDef scm,unsigned char*p)
{
	unsigned char i,cs=0;
  union 
  {
	  unsigned int uiValue;  
    unsigned char ucDataTable[4];
  }UnData;	
	p[0] = 0xaa;
	p[1] = scm.ucDeviceCode;
  UnData.uiValue = scm.uiStartID;
	for(i=0;i<4;i++){
	  p[2+i] = UnData.ucDataTable[i];
	}
	p[6] = scm.uiIDnumber>>8;
	p[7] = scm.uiIDnumber&0xff;
  UnData.uiValue = scm.uiCurentID;
	for(i=0;i<4;i++){
	  p[8+i] = UnData.ucDataTable[i];
	}	
	p[0x0c] = scm.uiRestID>>8;
	p[0x0d] = scm.uiRestID&0xff;	
	for(i = 0;i<14;i++){
	  cs += p[i];
	}
	p[0x0e] = cs;
}

/*
 8.Start frame          1byte 0x10  //10 address set 0xaa  
 9.Input voltage error  4bytes 0x11-0x14
10.Input current error  4bytes 0x15-0x18
11.Input power error    4bytes 0x19-0x1c
12.Output voltage error 4bytes 0x1d-0x20
13.Output current error 4bytes 0x21-0x24
14.Output power error   4bytes 0x25-0x28
15.check sum            1byte  0x29      //26bytes
*/
void ErrData2Scm(ScmAT24CxxErrTypeDef *scm,unsigned char*p)
{
	unsigned char i;
  union 
  {
	  float fValue;  
    unsigned char ucDataTable[4];
  }UnData;	
	
	for(i = 0;i < 4;i++){
	  UnData.ucDataTable[i] = p[1+i];
	}
	scm->fInputVol = UnData.fValue;
	
	for(i = 0;i < 4;i++){
	  UnData.ucDataTable[i] = p[5+i];
	}
	scm->fInputCur = UnData.fValue;
	
	for(i = 0;i < 4;i++){
	  UnData.ucDataTable[i] = p[9+i];
	}
	scm->fInputPow = UnData.fValue;
	
	for(i = 0;i < 4;i++){
	  UnData.ucDataTable[i] = p[13+i];
	}
	scm->fOutputVol = UnData.fValue;	

	for(i = 0;i < 4;i++){
	  UnData.ucDataTable[i] = p[17+i];
	}
	scm->fOutputCur = UnData.fValue;	

	for(i = 0;i < 4;i++){
	  UnData.ucDataTable[i] = p[21+i];
	}
	scm->fOutputPow = UnData.fValue;	
}


void ErrScm2Array(ScmAT24CxxErrTypeDef scm,unsigned char*p)
{
	unsigned char i,cs=0;
  union 
  {
	  float fValue;  
    unsigned char ucDataTable[4];
  }UnData;	
	p[0] = 0xaa;
  UnData.fValue = scm.fInputVol;
	for(i=0;i<4;i++){
	  p[1+i] = UnData.ucDataTable[i];
	}
  UnData.fValue = scm.fInputCur;
	for(i=0;i<4;i++){
	  p[5+i] = UnData.ucDataTable[i];
	}
  UnData.fValue = scm.fInputPow;
	for(i=0;i<4;i++){
	  p[9+i] = UnData.ucDataTable[i];
	}
  UnData.fValue = scm.fOutputVol;
	for(i=0;i<4;i++){
	  p[13+i] = UnData.ucDataTable[i];
	}	
  UnData.fValue = scm.fOutputCur;
	for(i=0;i<4;i++){
	  p[17+i] = UnData.ucDataTable[i];
	}
  UnData.fValue = scm.fOutputPow;
	for(i=0;i<4;i++){
	  p[21+i] = UnData.ucDataTable[i];
	}	
	for(i = 0;i<25;i++){
	  cs += p[i];
	}
	p[25] = cs;
}


/*******************************************************************************
* Function Name : ReadDataFromFlash
* Description   : Read data from Flash
* Input         : *RamAdrStar   uiSum: total of bytes reading from FLASH
* Output        : None.
* Return        : None.
*******************************************************************************/
void ReadDataFromFlash(unsigned char *RamAdrStar, unsigned long ulFlashAdr, unsigned short int uiSum)
{
    unsigned short int uiCnt;
    for(uiCnt = 0; uiCnt < uiSum; uiCnt++){
        *(RamAdrStar + uiCnt) = *(__IO uint8_t*)(ulFlashAdr + uiCnt);
    }
}

/*******************************************************************************
* Function Name : WriteDataToFlash
* Description   : Write data to Flash
* Input         : *RamAdrStar:RAM     uiSum: total of bytes writting to FLASH
* Output        : None.
* Return        : None.
*******************************************************************************/
void WriteDataToFlash(unsigned char *RamAdrStar, unsigned long ulFlashAdr, unsigned short int uiSum)
{
    unsigned short int uiCnt;
	  FLASH_Unlock();
	  FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR |FLASH_FLAG_WRPRTERR);
	  FLASH_ErasePage(FLASH_BACKUP_PAGE);
    for(uiCnt = 0; uiCnt < uiSum; uiCnt += 2){    /* write 2 bytes every time */
        FLASH_ProgramHalfWord(ulFlashAdr + uiCnt, (*(RamAdrStar + uiCnt + 1)<<8) + *(RamAdrStar + uiCnt));
    }
		FLASH_Lock();
}















