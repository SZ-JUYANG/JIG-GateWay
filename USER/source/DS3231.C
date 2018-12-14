/******************************************************************************

                  版权所有 (C), 2014-2024, 深圳美吉星科技有限公司

 ******************************************************************************
  文 件 名   : DS3231.C
  版 本 号   : 初稿
  作    者   : ty
  生成日期   : 2014年9月26日
  最近修改   :
  功能描述   : 时钟芯片DS331驱动程序，采用模拟IIC
  函数列表   :
              BCD2HEX
              delay_1us
              delay_us
              HEX2BCD
              I2C1_ReadByte
              I2C1_WriteByte
              I2C_DS3231_Config
              IIC_ack
              IIC_noack
              IIC_readbyte
              IIC_sendbyte
              IIC_start
              IIC_stop
              IIC_waitack
              ModifyTimeBCD
              ModifyTimeHEX
              ReadTimeHEX
              SDA_DIR_SET
  修改历史   :
  1.日    期   : 2014年9月26日
    作    者   : ty
    修改内容   : 创建文件

******************************************************************************/
#include "stm32f10x.h"
#include "DS3231.h"
//#include "SMITServerToMJXConcentrator.h"

unsigned char BCD2HEX(unsigned char ucValue);
unsigned char HEX2BCD(unsigned char ucValue);
unsigned char I2C1_ReadByte(unsigned char  id, unsigned char read_address);


 /*******************************************************************************
* Function Name : BCD2HEX
* Description   : BCD change to HEX.
* Input         : ucValue.
* Output        : None.
* Return        : The result of change
*******************************************************************************/
unsigned char BCD2HEX(unsigned char ucValue)   //BCD转换为Byte
{
     unsigned char i;
     i= ucValue & 0x0f;
     ucValue >>= 4;
     ucValue &= 0x0f;
     ucValue *= 10;
     i += ucValue;     
     return i;
}
 
/*******************************************************************************
* Function Name : delay_1us
* Description   : 延时1US
* Input         : ucValue.
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void delay_1us(void)
{
    __nop(); 	 //72M时钟频率，那么一个周期是 1/72微秒
    __nop();
    __nop();
    __nop();
    __nop();
    __nop(); 
    __nop();
    __nop();

}

 /*******************************************************************************
* Function Name : delay_us
* Description   : 延时N US
* Input         : ucValue.
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void delay_us(unsigned int uiCounter)
{
	while(uiCounter--){
		delay_1us();
	}

}
 
/*******************************************************************************
* Function Name : HEX2BCD
* Description   : HEX change to BCD.
* Input         : ucValue.
* Output        : None.
* Return        : The result of change
*******************************************************************************/
unsigned char HEX2BCD(unsigned char ucValue)
{
   unsigned char i,j,k;
   i = ucValue/10;
   j = ucValue%10;
   k = j+(i<<4);
   return k;
}
/*******************************************************************************
* Function Name : SDA_DIR_SET
* Description   : 设置SDA输入输出方向
* Input         : ucSdaDir
                    0--输出；1--输入
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void SDA_DIR_SET(unsigned char ucSdaDir) //SDA
{
    GPIO_InitTypeDef  GPIO_InitStructure;
//    if(ucSdaDir==0){
//        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;          // SDA 
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//        GPIO_Init(GPIOD, &GPIO_InitStructure); 
//    }
//    else if(ucSdaDir==1){
//        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;          // SDA 
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
//        GPIO_Init(GPIOD, &GPIO_InitStructure); 
//    }
    if(ucSdaDir==0){
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;          // SDA 
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure); 
    }
    else if(ucSdaDir==1){
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;          // SDA 
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
        GPIO_Init(GPIOB, &GPIO_InitStructure); 
    }	
		
}
/*******************************************************************************
* Function Name : IIC_start
* Description   : IIC起始信号
* Input         : 无
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void IIC_start(void)
{
	SDA_DIR_SET(0);   //SDA线输出
	IIC_SDA_HIGH;	 //SDA为高电平 	  
	IIC_SCL_HIGH;	 //SCL为高电平
	delay_us(4);
 	IIC_SDA_LOW; //SDA下降沿
    IIC_SCL_LOW;
}
/*******************************************************************************
* Function Name : IIC_stop
* Description   : IIC停止信号
* Input         : 无
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void IIC_stop(void)
{
	SDA_DIR_SET(0);   				 //SDA线输出
 	IIC_SDA_LOW; //SDA下降沿
    IIC_SCL_LOW;
	delay_us(4);
 	IIC_SCL_HIGH; //SDA下降沿
    IIC_SDA_HIGH;
}
/*******************************************************************************
* Function Name : IIC_waitack
* Description   : IIC等待应答信号
* Input         : 无
* Output        : None.
* Return        : The result of change
*******************************************************************************/
unsigned char IIC_waitack(void)
{
	unsigned char times=0;
	SDA_DIR_SET(1);     //SDA设置为输入  
	IIC_SDA_HIGH;//sda发送高点平
	delay_us(1);	   
	IIC_SCL_HIGH;	//时钟高电平，上升沿
	delay_us(1);	 
	while(IIC_SDA_Read){	//读数据，直到从机给出应答
		if(++times>250){
			IIC_stop(); //无应答，停止
			return 1;
		}
	}
	IIC_SCL_LOW;//时钟输出低电平，下降沿 	   
	return 0;  
}
/*******************************************************************************
* Function Name : IIC_ack
* Description   : IIC应答信号
* Input         : 无
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void IIC_ack(void)
{
    SDA_DIR_SET(0);
	IIC_SCL_LOW; //时钟低电平
	IIC_SDA_LOW; //数据低电平,表示有应答
	delay_us(2);
	IIC_SCL_HIGH;	  //时钟上升沿
	delay_us(2);
	IIC_SCL_LOW; //时钟下降沿
}
/*******************************************************************************
* Function Name : IIC_ack
* Description   : IIC无应答信号
* Input         : 无
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void IIC_noack(void)
{
    SDA_DIR_SET(0);
	IIC_SCL_LOW;  //时钟低电平
	IIC_SDA_HIGH;	   //数据高电平,表示无应答
	delay_us(2);
	IIC_SCL_HIGH;		//时钟上升沿
	delay_us(2);
	IIC_SCL_LOW;	//时钟下降沿
}
/*******************************************************************************
* Function Name : IIC_sendbyte
* Description   : IIC写数据
* Input         : 无
* Output        : None.
* Return        : The result of change
*******************************************************************************/
void IIC_sendbyte(unsigned char  ucData)
{                        
    unsigned char  t;      
    SDA_DIR_SET(0);
    IIC_SCL_LOW; //时钟低电平，只有SCL为低才可改变数据
    for(t=0;t<8;t++)//循环发送8个字节
    {              
 		if ((ucData&0x80)>>7) 	
            IIC_SDA_HIGH; //发出第7-t位
		else 
            IIC_SDA_LOW;
        ucData<<=1; 	  
		delay_us(2); 
        
		IIC_SCL_HIGH; //拉高SCL，发数据
		delay_us(2); 
		IIC_SCL_LOW; //SCL恢复低电平，准备修改SDA	
		delay_us(2);
    }	 
}
/*******************************************************************************
* Function Name : IIC_sendbyte
* Description   : IIC读数据
* Input         : 无
* Output        : None.
* Return        : The result of change
*******************************************************************************/
unsigned char IIC_readbyte(unsigned char ucDck)
{
	unsigned char i,receive=0;
	SDA_DIR_SET(1);//准备接收数据，SDA设置为输入
    for(i=0;i<8;i++ )			//循环接收8位
	{
        IIC_SCL_LOW; //SCL为低电平 
        delay_us(2);
		IIC_SCL_HIGH;   //SCL上升沿，开始读取数据
        receive<<=1;
        if(IIC_SDA_Read) //读取数据
			receive|=0x01; //如果读到的数据为1，receive最低位应为1  
		delay_us(1); 
    }					 
    if (ucDck) //如果需要应答
        IIC_ack();//发送应答
    else
        IIC_noack(); //发送无应答   
    return receive;
}

/*******************************************************************************
* Function Name : I2C1_WriteByte
* Description   : I2C1_WriteByte.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void I2C1_WriteByte(unsigned char id,unsigned char write_address,unsigned char byte)
{

    IIC_start();//产生起始条件
    IIC_sendbyte(id);
    IIC_waitack();
    IIC_sendbyte(write_address);
    IIC_waitack();
    IIC_sendbyte(byte);
    IIC_waitack();
    IIC_stop();
    
}
/*******************************************************************************
* Function Name : I2C1_ReadByte
* Description   : I2C1_ReadByte.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
unsigned char I2C1_ReadByte(unsigned char  id, unsigned char read_address)
 {  
     unsigned char temp;         

    IIC_start();//产生起始条件
    IIC_sendbyte(id);
    IIC_waitack();
    IIC_sendbyte(read_address);
    IIC_waitack();
     
    IIC_start(); 
    IIC_sendbyte(id+1);
    IIC_waitack();
    temp=IIC_readbyte(0);
    IIC_stop();
    return temp;    
 }
/*******************************************************************************
* Function Name : I2C_DS3231_Config
* Description   : I2C_DS3231_Config.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void I2C_DS3231_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);//PCB第一版 程序用PD0 PD1

//    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;/*PD1-SDA,PD0-SCL*/
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 /*50MHz*/
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_Init(GPIOD, &GPIO_InitStructure);
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//PCB第二版 程序用PB6 PB7

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7; /*PB7-SDA,PB6-SCL*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	      /*50MHz*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
	

}

/*******************************************************************************
* Function Name : ModifyTimeHEX
* Description   : ModifyTimeHEX.
* Input         : sTime.  HEX输入
* Output        : None.
* Return        : None.
*******************************************************************************/
 void ModifyTimeHEX(ScmDS3231Struct sTime)
 {
		unsigned char ucTemp;
	  /* 年设置 */
		ucTemp = HEX2BCD(sTime.ucYear);
		I2C1_WriteByte(0xd0,0x06,ucTemp);
	  /* 月设置 */
	 	ucTemp = HEX2BCD(sTime.ucMonth);
		I2C1_WriteByte(0xd0,0x05,ucTemp);
	  /* 日期设置 */
	  ucTemp = HEX2BCD(sTime.ucDate);
		I2C1_WriteByte(0xd0,0x04,ucTemp);
	  /* 星期几设置 */
	  ucTemp = HEX2BCD(sTime.ucDay);
		I2C1_WriteByte(0xd0,0x03,ucTemp);
	  /* 小时设置 */
	  ucTemp = HEX2BCD(sTime.ucHour);
		I2C1_WriteByte(0xd0,0x02,ucTemp);
	  /* 分钟设置 */
	  ucTemp = HEX2BCD(sTime.ucMinute);
		I2C1_WriteByte(0xd0,0x01,ucTemp);
	  /* 秒设置 */
	  ucTemp = HEX2BCD(sTime.ucSecond);
		I2C1_WriteByte(0xd0,0x00,0);
 }
 
/*******************************************************************************
* Function Name : ModifyTimeBCD
* Description   : ModifyTimeBCD.
* Input         : sTime. BCD码输入
* Output        : None.
* Return        : None.
*******************************************************************************/
 void ModifyTimeBCD(ScmDS3231Struct sTime)
 {
	  /* 年设置 */
		I2C1_WriteByte(0xd0,0x06,sTime.ucYear);
	  /* 月设置 */
		I2C1_WriteByte(0xd0,0x05,sTime.ucMonth);
	  /* 日期设置 */
		I2C1_WriteByte(0xd0,0x04,sTime.ucDate);
	  /* 小时设置 */
		I2C1_WriteByte(0xd0,0x02,sTime.ucHour);
	  /* 分钟设置 */
		I2C1_WriteByte(0xd0,0x01,sTime.ucMinute);
	  /* 秒设置 */
		I2C1_WriteByte(0xd0,0x00,sTime.ucSecond);
 } 
 /*******************************************************************************
* Function Name : ReadTimeHEX
* Description   : ReadTimeHEX.
* Input         : *psTime. 读出的参数为HEX
* Output        : None.
* Return        : None.
*******************************************************************************/
 void ReadTimeHEX(ScmDS3231Struct *psTime)
 {
	  /* 读出年 */
	  psTime->ucYear = BCD2HEX(I2C1_ReadByte(0xd0,0x06));
	  /* 读出月 */
	  psTime->ucMonth = BCD2HEX(I2C1_ReadByte(0xd0,0x05));
	  /* 读出日期 */
	  psTime->ucDate = BCD2HEX(I2C1_ReadByte(0xd0,0x04));
	  /* 读出星期几 */
	  psTime->ucDay = BCD2HEX(I2C1_ReadByte(0xd0,0x03));
	  /* 读出小时 */
	  psTime->ucHour = BCD2HEX(I2C1_ReadByte(0xd0,0x02));
	  /* 读出分钟 */
	  psTime->ucMinute = BCD2HEX(I2C1_ReadByte(0xd0,0x01));
	  /* 读出秒 */
	  psTime->ucSecond = BCD2HEX(I2C1_ReadByte(0xd0,0x00));
 } 
 
/* END:   Added by ty, 2014/9/26 */


