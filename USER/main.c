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
 ALIENTEK 战舰STM32F103开发板实验13
 TFTLCD显示实验
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司
 作者：正点原子 @ALIENTEK
************************************************/
u8 lcdString[50];
ulong frequency;
u16 dacNumber=1250;

int main(void)
{
		u8 result,result_last;
    delay_init();	    	 //延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    uart_init(115200);	 	//串口初始化为115200
    LED_Init();			     //LED端口初始化
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
