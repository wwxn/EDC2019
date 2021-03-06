#ifndef  __ADS115_H_
#define  __ADS115_H_
#include "sys.h"
 

/***************************************************************************************
*说明：当端输入时候正输入为输入信号，负输入为地但是输入信号不能为负电压(不能比地电位低)
*	   双端输入时候正输入为输入信号，负输入为负输入输入信号的差值可以为负电压
****************************************************************************************/
#define	 SDA_A1     PCout(11)=1   //SDA输出PA8
#define	 SDA_A0     PCout(11)=0
#define	 SCL_A1	    PCout(12)=1    //SCL PA9
#define	 SCL_A0	    PCout(12)=0
#define	 SDA_AI	    PCin(11)   //SDA读入

//#define	 SDA_A21     PAout(10)=1   //SDA输出
//#define	 SDA_A20     PAout(10)=0
//#define	 SCL_A21	   PAout(11)=1    //SCL
//#define	 SCL_A20	   PAout(11)=0
//#define	 SDA_A2I	   PAin(10)   //SDA读入

//#define	 SDA_A31     PBout(10)=1   //SDA输出
//#define	 SDA_A30     PBout(10)=0
//#define	 SCL_A31	   PBout(11)=1    //SCL
//#define	 SCL_A30	   PBout(11)=0
//#define	 SDA_A3I	   PBin(10)   //SDA读入

//#define	 SDA_A41     PBout(12)=1   //SDA输出
//#define	 SDA_A40     PBout(12)=0
//#define	 SCL_A41	   PBout(13)=1    //SCL
//#define	 SCL_A40	   PBout(13)=0
//#define	 SDA_A4I	   PBin(12)   //SDA读入

//I2C地址以及读写设置
#define  WR_REG 0x90       //写寄存器
#define  RE_REG 0x91       //读寄存器

/***********************************寄存器控制字**********************************************/
#define  DATA_REG  0x00		//转换数据寄存器
#define  CONF_REG  0x01     //控制字设置寄存器
#define  LOTH_REG  0x02		//最低阀值寄存器
#define  HITH_REG  0x03		//最高阀值寄存器

#define  ch0  0xc0       //通道0
#define  ch1  0xd0       //通道1
#define  ch2  0xe0       //通道2
#define  ch3  0xf0       //通道3

#define READAD(CH)		getad(0xe3,CH+0x02)
/***********************控制字申明*************************************************************
*|  OS | MUX2 | MUX1 | MUX0 | PGA2 | PGA1 | PGA0 | MODE  |------HCMD
*|  DR2| DR1  | DR0  | COMP_MODE | COMP_POL | COMP_LAT |  COMP_QUE1 | COMP_QUE0 |-----LCMD
***********************************************************************************************/
#define  HCMD1    0x64   //AIN0单端输入 +-4.096量程  连续模式  01000100b
#define  LCMD1	  0xf0	 //860sps 窗口比较器模式 输出低有效  不锁存信号至读 每周期检测阀值 11110000b

/************************函数申明****************************/
static void ADS1115_delay(u16 D);
void delay_nms(u16 ms);
void delay_nus(u16 us);
void ADS1115_Init(void);
void I2CStart_A(void);
void I2CStop_A(void);
void I2CWriteByte_A(u8 DATA);
u8 I2CReadByte_A(void);
void ADS1115Config_A(u8 LCMD,u8 HCMD);
void SetThresHold_A(u16 L_TH,u16 H_TH);        //高低阀门设置
u16 ReadAD_A(void);
u16 getad(u8 LCMD,u8 HCMD);
u16 lvbo(u8 LCMD,u8 HCMD);
u16 readAverage(u8 ch,u8 time);




#endif		
