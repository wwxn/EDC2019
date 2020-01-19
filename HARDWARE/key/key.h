#ifndef _KEY_H
#define _KEY_H

#include "sys.h"
#define SWITCH			PGout(15)
#define POINTS			260
#define MAXFREQ			400000
#define MINFREQ			100
#define SWEEPNUMBERS	((1000-MINFREQ)/10+1+(MAXFREQ-1000)/1000)

typedef struct 
{
	u16 A2;
	u16 middleInput;
	u16 highAmp;
	u16 lowAmp;
	u16 middleLoad;
	u16 out2hz;
}TestData;
void Key_Init(void);
void switchInit(void);
extern TestData initData;
extern TestData resultData;
u8 DataCompare(TestData inputData,TestData standardData);
void DataRead(TestData *data);
extern u8 seekFlag;
extern u8 sweepFlag;
extern u8 lcdString[50];

#endif
