#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "AD9910.h"
#include "ads115.h"
#include "dac.h"
/************************************************
 ALIENTEK ս��STM32F103������ʵ��13
 TFTLCD��ʾʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
u8 lcdString[50];
ulong frequency;
u16 dacNumber=1250;

int main(void)
{
		u8 result,result_last;
    delay_init();	    	 //��ʱ������ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
    LED_Init();			     //LED�˿ڳ�ʼ��
    LCD_Init();
    POINT_COLOR=BLACK;
    Init_ad9910();
    Freq_convert(1000);
    Key_Init();
    ADS1115_Init();
	switchInit();
	LCD_Display_Dir(U2D_L2R);
	Dac1_Init();
	LCD_ShowString(40,100,200,24,24,(u8*)"Welcome!");
	Dac1_Set_Vol(dacNumber);
    while(1)
    {
		if(seekFlag)
		{
			DataRead(&resultData);
			result=DataCompare(resultData,initData);
			if(result_last==2)
			{
				if(result==1)
				{
					LCD_ShowString(20,100,320,24,24,(u8*)"Error:C1 Double!         ");
					result_last=2;
				}
				else
				{
					result_last=result;
				}
			}
			else if(result==1)
			{
				LCD_ShowString(20,100,320,24,24,(u8*)"No Error!         ");
				result_last=result;
			}
			else
			{
				result_last=result;
			}
		}
    }
}
