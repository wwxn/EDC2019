/**********************************************************
                       康威电子

功能：stm32f103rct6控制
			显示：12864
接口：按键接口请参照key.h
时间：2015/11/3
版本：1.0
作者：康威电子
其他：程序借鉴正点原子，添加自己的驱动，未经作者许可，不得用于其它任何用途

更多电子需求，请到淘宝店，康威电子竭诚为您服务 ^_^
https://shop110336474.taobao.com/?spm=a230r.7195193.1997079397.2.Ic3MRJ

**********************************************************/

#include "stm32f10x.h"
#include "AD9910.h"
#include "delay.h"
#include "sys.h"
/*-----------------------------------------------
 名称：AD9910串行驱动
 编写：Liu
 日期：2014.6
 修改：无
 内容：
------------------------------------------------*/
uchar profile0[]={0x3f,0xff,0x00,0x00,0x25,0x09,0x7b,0x42}; 
const uchar cfr1[]= {0x00,0x40,0x00,0x00};      //cfr1控制字
const uchar cfr2[]= {0x01,0x00,0x00,0x00};      //cfr2控制字
const uchar cfr3[]= {0x05,0x0F,0x41,0x32};      //cfr3控制字  40M输入  25倍频  VC0=101   ICP=001;
uchar profile11[]= {0x3f,0xff,0x00,0x00,0x25,0x09,0x7b,0x42};      //profile1控制字 0x25,0x09,0x7b,0x42
//01振幅控制 23相位控制 4567频率调谐字
void AD9110_IOInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD, ENABLE);	 //PA,PB,PC端口时钟使能

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_7|GPIO_Pin_5|GPIO_Pin_3|GPIO_Pin_8|GPIO_Pin_6|GPIO_Pin_4;//初始化管脚PA2.3.4.5.6.7.8.9.10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO口速度为2MHz
    GPIO_Init(GPIOG, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_11;//初始化管脚PE0.1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO口速度为2MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOE
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_12;//初始化管脚PE0.1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO口速度为2MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);		
}

//=====================================================================


//======================发送8位数据程序===================================
void txd_8bit(uchar txdat)
{   uchar i,sbt;
    sbt=0x80;
    SCLK=0;
    for (i=0; i<8; i++)
    {   if ((txdat & sbt)==0) SDO=0;
        else SDO=1;
        SCLK=1;
        sbt=sbt>>1;
        SCLK=0;
    }
}

//======================ad9910初始化程序================================
void Init_ad9910(void)
{
    uchar k,m;

    AD9110_IOInit();//IO初始化
    AD9910_PWR = 0;//软件拉低

    PROFILE2=PROFILE1=PROFILE0=0;
    DRCTL=0;
    DRHOLD=0;
    MAS_REST=1;
    delay_ms(5);
    MAS_REST=0;

    CS=0;
    txd_8bit(0x00);    //发送CFR1控制字地址
    for (m=0; m<4; m++)
        txd_8bit(cfr1[m]);
    CS=1;
    for (k=0; k<10; k++);

//	CS=0;
//txd_8bit(0x01);    //发送CFR2控制字地址
// for (m=0;m<4;m++)
// txd_8bit(cfr2[m]);
//CS=1;
//for (k=0;k<10;k++);

    CS=0;
    txd_8bit(0x02);    //发送CFR3控制字地址
    for (m=0; m<4; m++)
        txd_8bit(cfr3[m]);
    CS=1;
    for (k=0; k<10; k++);

    UP_DAT=1;
    for(k=0; k<10; k++);
    UP_DAT=0;
    delay_ms(1);

}
//=====================================================================

//======================ad9910发送频率值程序===================================
void Txfrc(void)
{   uchar m;

    CS=0;
    txd_8bit(0x0e);    //发送profile1控制字地址
    for (m=0; m<8; m++)
        txd_8bit(profile11[m]);
    CS=1;
// for(k=0;k<10;k++);


    UP_DAT=1;
// for(k=0;k<10;k++);
    UP_DAT=0;
// Delay_ms(1);
}
//=====================================================================
//===================计算频偏字、频率字和发送程序======================
void Freq_convert(ulong Freq)
{
    ulong Temp;
    Temp=(ulong)Freq*4.294967296;	   //将输入频率因子分为四个字节  4.294967296=(2^32)/1000000000
    profile11[7]=(uchar)Temp;
    profile11[6]=(uchar)(Temp>>8);
    profile11[5]=(uchar)(Temp>>16);
    profile11[4]=(uchar)(Temp>>24);
    Txfrc();
}

void write_Amplitude(uint Amp)
{
    ulong Temp;
    Temp = (ulong)Amp * 25.20615385;  //25.20615385 = 2^14/650;
    if(Temp > 0x3fff)
       Temp = 0x3fff;
    Temp &= 0x3fff;
    profile0[1] = (uchar)Temp;
    profile0[0] = (uchar)(Temp >> 8);
    Txfrc();
}
