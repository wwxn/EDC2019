/*-----------------------------------------------
 名称：AD9854并口驱动
 编写：Liu
 日期：2014.3
 修改：无
 内容：
------------------------------------------------*/
#ifndef __AD9910_H__
#define __AD9910_H__

#include "stm32f10x.h"

#define uchar unsigned char
#define uint  unsigned int
#define ulong  unsigned long int
/*
sbit MASTER_RESET= P1^3;	//
sbit IORESET  = P2^6;	  //
sbit SCLK = P2^4;	  //
sbit SDIO    = P2^2;	 //
sbit OSK     = P2^0;	//
sbit IOUPDATA   = P0^4;	 //
sbit PROFILE2  = P0^2;	 //
sbit PROFILE1    = P0^0;	//
sbit PROFILE0 = P1^1;	 //
*/

//新增IO
//#define SDO PAout(4)
//#define PDCLK PCout(6)//PDCLK
//#define PLL_LOCK PCout(7)//悬空
//#define SYNC_CLK PCout(9)//悬空
//#define TXENABLE PCout(11)//TXEN
//#define RSO PCout(12)//RAM-SWP-OVR
#define AD9910_PWR PGout(8)//0
//原始IO
#define SDO PBout(14)//SDIO
#define DRHOLD PDout(11)
#define DROVER PDout(13)
#define UP_DAT PBout(15)
#define PROFILE1 PGout(6)
#define MAS_REST PGout(7)//
#define SCLK PDout(12)//
#define DRCTL  PGout(2)//
#define OSK PGout(4)
#define PROFILE0 PGout(3)
#define PROFILE2 PGout(5)
#define CS  PBout(12)// 

//void Init_AD9910(void);  //INIT 9910
//void AD9910WriteByte (unsigned char commond, unsigned char *databyte,unsigned char lengh);
void AD9110_IOInit(void);
void Init_ad9910(void);
void Freq_convert(ulong Freq);										//写频率

void write_Amplitude(uint Amp);
//void Write_Amplitude(bit Channel,unsigned int Amplitude);	//写幅度

#endif


