#include "key.h"
#include "sys.h"
#include "delay.h"
#include "lcd.h"
#include "stdio.h"
#include "math.h"
#include "ads115.h"
#include "AD9910.h"
#include "dac.h"

#define UP  1
#define LOW 2
#define EQUAL 0



TestData initData;
TestData resultData;
u8 Key_Value;
u8 sweepFlag;
u8 DataCompare(TestData inputData,TestData standardData);

u8 Key_Value;
u8 sweepFlag;
void DrawLowFreq(void);
void testData(void);
void DataRead(TestData *data);
float findVrms(u8 ch,u16 time);
void measureInputRes(void);
void measureOutputRes(void);
void measureAmp(void);
void lcdDrawAxis(void);
u16 result;
u16 adMax=0;
u16 adMin=65535;
u32 inputData,outputData;
u16 r3short=1904,r4short=61,r3open=284,c3open=152,c2double=99,c3double=65;
void switchInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);                //使能PORTC
    GPIO_InitStructure.GPIO_Pin= GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
}
void Key_Init(void)
{

    /******************PE的0 1 2 3作为输出**********************************/
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);                //使能PORTC
    GPIO_InitStructure.GPIO_Pin= GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    /******************PE的 4 5 6 7作为下拉输入*****************************/

    GPIO_InitStructure.GPIO_Pin= GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;  //PA0设置成输入，默认下拉
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    GPIO_SetBits(GPIOF,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2 |GPIO_Pin_3);
    GPIO_ResetBits(GPIOF,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);





    /****************配置中断线***********************************************/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟


    GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource4);

    EXTI_InitStructure.EXTI_Line=EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);


    GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource5);
    EXTI_InitStructure.EXTI_Line=EXTI_Line5;
    EXTI_Init(&EXTI_InitStructure);


    GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource6);
    EXTI_InitStructure.EXTI_Line=EXTI_Line6;
    EXTI_Init(&EXTI_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource7);
    EXTI_InitStructure.EXTI_Line=EXTI_Line7;
    EXTI_Init(&EXTI_InitStructure);



    /******************************配置中断优先级**************************/
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);                //使能PORTC
    GPIO_InitStructure.GPIO_Pin= GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}


/*****************键盘扫描*********************************/
u8 Read_KeyValue(void)
{
    u8 KeyValue=0;
    if((GPIO_ReadInputData(GPIOF)&0xff)!=0x0f)
    {
        //delay_ms(10);
        if((GPIO_ReadInputData(GPIOF)&0xff)!=0x0f)
        {
            GPIO_SetBits(GPIOF,GPIO_Pin_0);
            GPIO_ResetBits(GPIOF,GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);
            switch(GPIO_ReadInputData(GPIOF)&0xff)
            {
            case 0x11:
                KeyValue=1;
                break;
            case 0x21:
                KeyValue=5;
                break;
            case 0x41:
                KeyValue=9;
                break;
            case 0x81:
                KeyValue=13;
                break;
            }
            GPIO_SetBits(GPIOF,GPIO_Pin_1);
            GPIO_ResetBits(GPIOF,GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_3);
            switch(GPIO_ReadInputData(GPIOF)&0xff)
            {
            case 0x12:
                KeyValue=2;
                break;
            case 0x22:
                KeyValue=6;
                break;
            case 0x42:
                KeyValue=10;
                break;
            case 0x82:
                KeyValue=14;
                break;
            }
            GPIO_SetBits(GPIOF,GPIO_Pin_2);
            GPIO_ResetBits(GPIOF,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3);
            switch(GPIO_ReadInputData(GPIOF)&0xff)
            {
            case 0x14:
                KeyValue=3;
                break;
            case 0x24:
                KeyValue=7;
                break;
            case 0x44:
                KeyValue=11;
                break;
            case 0x84:
                KeyValue=15;
                break;
            }
            GPIO_SetBits(GPIOF,GPIO_Pin_3);
            GPIO_ResetBits(GPIOF,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2);
            switch(GPIO_ReadInputData(GPIOF)&0xff)
            {
            case 0x18:
                KeyValue=4;
                break;
            case 0x28:
                KeyValue=8;
                break;
            case 0x48:
                KeyValue=12;
                break;
            case 0x88:
                KeyValue=16;
                break;
            }
            GPIO_SetBits(GPIOF,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2 |GPIO_Pin_3);
            GPIO_ResetBits(GPIOF, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 |GPIO_Pin_7);
            while((GPIO_ReadInputData(GPIOF)&0xff)!=0x0f);
            return KeyValue;
        }
    }
    return 0;
}

float adcResult_outputRes2;
float adcResult_outputRes1;
float adcResult_InputRes;
float inputRes;
float outputRes;
float amp;
u8 seekFlag;
u8 initTime;
void Key_Deal()
{
	
    Key_Value=Read_KeyValue();
    switch(Key_Value)
    {
    case 1:
    {
		LCD_Clear(WHITE);
        measureInputRes();
        break;
    }
    case 2:
    {
		LCD_Clear(WHITE);
        measureOutputRes();
        break;
    }
    case 4:
    {
        LCD_Clear(WHITE);
		lcdDrawAxis();
        break;
    }
//    case 4:
//    {
//        sweepFlag=~sweepFlag;
//        break;
//    }
    case 3:
    {
		LCD_Clear(WHITE);
		measureAmp();
		break;
    }
//	case 6:
//	{
//		LCD_Clear(WHITE);
//		break;
//	}
	case 7:
	{
		if(initTime==0)
		{
			r3open=readAverage(ch2,5);
			initTime++;
		}
		else if(initTime==1)
		{
			r3short=readAverage(ch0,5);
			
			initTime++;
		}
		else if(initTime==2)
		{
			r4short=readAverage(ch2,5);
			initTime++;
		}
		else if(initTime==3)
		{
			Freq_convert(200000);
			delay_ms(100);
			c3open=readAverage(ch1,5);
			c3open=c3open*22/readAverage(ch0,5);
			initTime++;
		}
		else if(initTime==4)
		{
			Freq_convert(200000);
			delay_ms(100);
			c3double=readAverage(ch1,5);
			c3double=c3double*22/readAverage(ch0,5);
			initTime++;
		}
		else if(initTime==5)
		{
			Freq_convert(140);
			delay_ms(200);
			c2double=readAverage(ch1,5);
			c2double=c2double*22/readAverage(ch0,5);
			initTime++;
		}
		Freq_convert(1000);
		LCD_ShowxNum(20,20,initTime,1,12,0);
		break;
	}
	case 5:
	{
		dacNumber+=5;
		Dac1_Set_Vol(dacNumber);
		LCD_ShowxNum(20,20,dacNumber,5,16,16);
		break;
	}
	case 6:
	{
		dacNumber-=5;
		Dac1_Set_Vol(dacNumber);
		LCD_ShowxNum(20,20,dacNumber,5,16,16);
		break;
	}
	case 8:
	{
		LCD_Clear(WHITE);
		DrawLowFreq();
	}
		case 9:
	{
		DataRead(&initData);
		break;
	}
	case 10:
	{
		LCD_Clear(WHITE);
		seekFlag=~seekFlag;
		
	}
    }

}


void EXTI4_IRQHandler(void)
{
    delay_ms(100);
    if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_4)==1)
    {
        Key_Deal();
    }
    EXTI_ClearITPendingBit(EXTI_Line4);
}


void EXTI9_5_IRQHandler(void)
{
    delay_ms(100);
    if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7)==1)
    {
        Key_Deal();
    }
    EXTI_ClearFlag(EXTI_Line5|EXTI_Line6|EXTI_Line7);
}

float findVrms(u8 ch,u16 time)
{
    u16 i;

    //ADS1115Config_A(0xe3,ch+0x02);
    for(i=0; i<time; i++)
    {
        result=READAD(ch0);
        if(result>adMax)
        {
            adMax=result;
        }
        if(result<adMin)
        {
            adMin=result;
        }

    }
    return 0.707*4.096*2*(adMax-adMin)/65535*1000;
}

void measureInputRes(void)
{
    u16 adResult=0;
    adResult=READAD(ch0);
    adcResult_InputRes=adResult*4.096*2/65535*1000/22;
    inputRes=adcResult_InputRes/(20.6-adcResult_InputRes)*2000;
    sprintf((char*)lcdString,"Input Res:%.2f      ",inputRes);
	if(inputRes>=0)
	{
		LCD_ShowString(20,100,320,24,24,(u8*)lcdString);
	}
	else
	{
		LCD_ShowString(20,100,320,24,24,(u8*)"Error!          ");
	}
}
void measureOutputRes(void)
{
    u16 adResult=0;
    SWITCH=1;
    delay_ms(500);
    adResult=READAD(ch1);
    adcResult_outputRes2=adResult*4.096*2/65535*1000;
    SWITCH=0;
    delay_ms(500);
    adResult=READAD(ch1);
    adcResult_outputRes1=adResult*4.096*2/65535*1000;
    outputRes=((float)adcResult_outputRes1/adcResult_outputRes2-1)*3000.0*2000/1785;
    sprintf((char*)lcdString,"Output Res:%.2f      ",outputRes);
    if(outputRes>=0)
	{
		LCD_ShowString(20,100,320,24,24,(u8*)lcdString);
	}
	else
	{
		LCD_ShowString(20,100,320,24,24,(u8*)"Error!          ");
	}
}

void measureAmp(void)
{
    
    inputData=READAD(ch0);
    outputData=READAD(ch1);
    amp=(float)outputData/inputData*22;
    sprintf((char*)lcdString,"Amp:%.2f      ",amp);
    LCD_ShowString(20,100,320,24,24,(u8*)lcdString);
}
u16 result;
u16 maxNum;
float maxVol;
u16 maxPoint,minPoint;
u32 maxFreq,minFreq;
float adData[SWEEPNUMBERS];
void lcdDrawAxis(void)
{
	u16 i;
	LCD_Clear(WHITE);
	LCD_Display_Dir(U2D_L2R);
	LCD_DrawLine(40,30,40,210);
	LCD_DrawLine(40,210,280,210);
	sprintf((char*)lcdString,"%d/KHz     ",400);
	LCD_ShowString(270,215,50,12,12,(u8*)lcdString);
	LCD_ShowxNum(160,215,200,3,12,0);
	LCD_ShowxNum(40,215,0,3,12,0);
	LCD_ShowxNum(100,215,100,3,12,0);
	LCD_ShowxNum(220,215,300,3,12,0);
//	for(i=0;i<6;i++)
//	{
//		LCD_ShowxNum(15,190-i*34,i,1,12,0);
//	}
//	for(i=0;i<5;i++)
//	{
//		LCD_ShowxNum(15+i*60,220,i*50,3,12,0);
//	}
//	i=0;
	for(i=0;i<(1000-MINFREQ)/10;i++)
	{
		Freq_convert(i*10+MINFREQ);
		adData[i]=READAD(ch1);
		adData[i]=adData[i]*22.0/READAD(ch0);
		//adData[i]=65530;
	}
	for(i=(1000-MINFREQ)/10;i<(1000-MINFREQ)/10+1+(MAXFREQ-1000)/1000;i++)
	{
		Freq_convert((i-(1000-MINFREQ)/10)*1000+1000);
		adData[i]=READAD(ch1);
		adData[i]=adData[i]*22.0/READAD(ch0);
		//adData[i]=65530;
	}
	for(i=0;i<SWEEPNUMBERS;i++)
	{
		if(maxVol<adData[i])
		{
			maxNum=i;
			maxVol=adData[i];
		}
	}
	maxVol=maxVol*80/100;
	for(i=maxNum;i<SWEEPNUMBERS;i++)
	{
		if(adData[i]<maxVol)
		{
			maxPoint=i;
			break;
		}
		maxPoint=SWEEPNUMBERS-1;
	}
	for(i=0;i<30;i+=2)
	{
		LCD_DrawLine(40+i*10,225-adData[maxPoint],40+(i+1)*10,225-adData[maxPoint]);
	}
	LCD_ShowString(5,225-adData[maxPoint],30,12,12,(u8*)"-3dB");
	for(i=maxNum;i>0;i--)
	{
		if(adData[i]<maxVol)
		{
			minPoint=i;
			break;
		}
		minPoint=0;
	}
	if(maxPoint>(1000-MINFREQ)/10)
	{
		maxFreq=(maxPoint-(1000-MINFREQ)/10)*1000+1000;
	}
	else
	{
		maxFreq=1000-((1000-MINFREQ)/10-maxPoint)*10;
	}
	if(minPoint>(1000-MINFREQ)/10)
	{
		minFreq=(minPoint-(1000-MINFREQ)/10)*1000+1000;
	}
	else
	{
		minFreq=1000-((1000-MINFREQ)/10-minPoint)*10;
	}
	for(i=0;i<POINTS-1;i++)
	{
		LCD_DrawLine(40+i,240-adData[90+i],40+i+1,240-adData[90+i+1]);
		//LCD_DrawPoint(40+i,240-adData[i*(SWEEPNUMBERS/POINTS)]);
	}
	sprintf((char*)lcdString,"MaxFrequency:%d      ",maxFreq);
	LCD_ShowString(200,20,200,12,12,(u8*)lcdString);
//	sprintf((char*)lcdString,"MinFrequency:%d      ",minFreq);
//	LCD_ShowString(20,40,200,12,12,(u8*)lcdString);
	Freq_convert(1000);
}


u8 AboutEqual(u16 data,u16 standard,float error)
{
	if(standard==0)
	{
		if(data<=40||data>=60000)
		{
			return EQUAL;
		}
		else
		{
			return UP;
		}
	}
	else
	{
		if(data>standard*(1+error))
		{
			return UP;
		}
		else if(data<standard*(1-error))
		{
			return LOW;
		}
		else 
		{
			return EQUAL;
		}
	}
}
u16 data;
void DataRead(TestData *data)
{
	Freq_convert(1000);
	delay_ms(100);
	(*data).A2=readAverage(ch2,5);
	(*data).middleInput=READAD(ch0);
	Freq_convert(140);
	delay_ms(100);
	(*data).lowAmp=readAverage(ch1,5)*22/readAverage(ch0,5);
	Freq_convert(200000);
	delay_ms(100);
	(*data).highAmp=readAverage(ch1,5)*22/readAverage(ch0,5);
	Freq_convert(1000);
	Dac1_Set_Vol(dacNumber);
	
}

u8 DataCompare(TestData inputData,TestData standardData)
{
	u16 LoadData;
	if(AboutEqual(inputData.A2,10256,0.1)==EQUAL)
	{
		if(AboutEqual(inputData.highAmp,c3open,0.2)==EQUAL)
		{
			LCD_ShowString(20,100,320,24,24,(u8*)"Error:C3 Open!    ");
			return 0;
		}
		else if(inputData.highAmp>0.5&&inputData.highAmp<5)
		{
			LCD_ShowString(20,100,320,24,24,(u8*)"Error:C2 Open!    ");
			return 0;
		}
		
		else if(inputData.highAmp==0)
		{
			LCD_ShowString(20,100,320,24,24,(u8*)"Error:C1 Open!    ");
			return 2;
		}
		else if(AboutEqual(inputData.lowAmp,c2double,0.15)==EQUAL)
		{
			LCD_ShowString(20,100,320,24,24,(u8*)"Error:C2 Double!    ");
			return 0;
		}
		else if(AboutEqual(inputData.highAmp,c3double,0.15)==EQUAL)
		{
			LCD_ShowString(20,100,320,24,24,(u8*)"Error:C3 Double!    ");
			return 0;
		}
		else 
		{
			//LCD_ShowString(20,100,320,24,24,(u8*)"No Error!        ");
			return 1;
		}
	}
	else
	{
		if(AboutEqual(inputData.A2,5764,0.5)==EQUAL)
		{
			LCD_ShowString(20,100,320,24,24,(u8*)"Error:R2 Open!    ");
			return 0;
		}
		else if(AboutEqual(inputData.A2,r3open,0.1)==EQUAL)
		{
			LCD_ShowString(20,100,320,24,24,(u8*)"Error:R3 Open!    ");
			return 0;
		}
		else if(AboutEqual(inputData.A2,r4short,0.1)==EQUAL)
		{
			LCD_ShowString(20,100,320,24,24,(u8*)"Error:R4 Short!    ");
			return 0;
		}
		else if(AboutEqual(inputData.middleInput,r3short,0.1)==EQUAL)
		{
			LCD_ShowString(20,100,320,24,24,(u8*)"Error:R3 Short!    ");
			return 0;
		}
		else if(inputData.middleInput<1000)
		{
			SWITCH=1;
			delay_ms(500);
			LoadData=readAverage(ch2,5);
			SWITCH=0;
			//delay_ms(500);
			if(AboutEqual(LoadData,14400,0.2)==EQUAL)
			{
				LCD_ShowString(20,100,320,24,24,(u8*)"Error:R1 Short!    ");
				return 0;
			}
			else
			{
				LCD_ShowString(20,100,320,24,24,(u8*)"Error:R2 Short!    ");
				return 0;
			}
		}
		else
		{
			Dac1_Set_Vol(3290);
			delay_ms(100);
			data=READAD(ch1);
			if(data>1500)
			{
				LCD_ShowString(20,100,320,24,24,(u8*)"Error:R1 Open!    ");
				Dac1_Set_Vol(dacNumber);
				//delay_ms(100);
				return 0;
			}
			else
			{
				LCD_ShowString(20,100,320,24,24,(u8*)"Error:R4 Open!    ");
				Dac1_Set_Vol(dacNumber);
				//delay_ms(100);
				return 0;
			}
		}
	}
	
}

void DrawLowFreq(void)
{
	u8 i;
	LCD_DrawLine(40,30,40,210);
	sprintf((char*)lcdString,"%d/Hz     ",1000);
	LCD_ShowString(270,215,50,12,12,(u8*)lcdString);
	LCD_ShowxNum(40,215,0,3,12,0);
	LCD_ShowxNum(100,215,250,3,12,0);
	LCD_ShowxNum(160,215,500,3,12,0);
	LCD_ShowxNum(220,215,750,3,12,0);
	LCD_DrawLine(40,210,280,210);
	for(i=0;i<90;i++)
	{
		LCD_DrawLine(40+i*3,240-adData[+i],40+(i+1)*3,240-adData[i+1]);
	}
	sprintf((char*)lcdString,"MinFrequency:%d      ",minFreq);
	LCD_ShowString(200,20,200,12,12,(u8*)lcdString);
		for(i=0;i<30;i+=2)
	{
		LCD_DrawLine(40+i*10,240-adData[minPoint],40+(i+1)*10,240-adData[minPoint]);
	}
	LCD_ShowString(5,240-adData[minPoint],30,12,12,(u8*)"-3dB");
}

