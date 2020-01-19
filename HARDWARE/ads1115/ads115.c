#include "sys.h"
#include "stm32f10x_i2c.h"
#include "ads115.h"

static void ADS1115_delay(u16 D)
{
	while(--D);
}

void delay_nms(u16 ms)
{
	u16 i;
	u32 M = 0;//720W
	for(i = 0;i < ms; i++)
	for(M=12000;M > 0;M--);
}

void delay_nus(u16 us)
{
	u16 i;
	u16 M = 0;//720W
	for(i = 0;i < us; i++)
	for(M=72;M > 0;M--);
}



/////////////////PA8 SDA////PA9 SCL///////////////////////////////////
void ADS1115_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC ,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;//A SCL SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

    SDA_A1;
    SCL_A1;
	delay_nms(5);

}

//I2C��������
void I2CStart_A(void)
{
   SDA_A1;
   ADS1115_delay(5);
   SCL_A1;
   ADS1115_delay(5);
   SDA_A0;
   ADS1115_delay(5);//MIN 160ns
   SCL_A0;
   ADS1115_delay(5);
}




//I2Cֹͣ����
void I2CStop_A(void)
{
   SDA_A0;
   ADS1115_delay(5);
   SCL_A1;
   ADS1115_delay(5);
   SDA_A1;
   ADS1115_delay(5);//MIN 160ns
}



//I2C дһ�ֽ�
void I2CWriteByte_A(u8 DATA)
{
      u8 i;
      SCL_A0;
      for(i = 0;i < 8; i++)
      {
         if(DATA&0x80)
         {
              SDA_A1;
         }
         else
         {
              SDA_A0;
         }
         SCL_A1;//�����ֲ᲻����ʱ
         ADS1115_delay(5);
         SCL_A0;
         ADS1115_delay(5);
         DATA	= DATA << 1;  
      }
      SDA_A1;
      SCL_A1;
      ADS1115_delay(5);
      SCL_A0;
}


//I2C ��һ�ֽ�
u8 I2CReadByte_A(void)
{
    u8 TData=0,i;
    for(i=0;i<8;i++)
    {
		SCL_A1;
		ADS1115_delay(5);
		TData=TData<<1;
		if(SDA_AI)
		{
			TData|=0x01;
		}
		SCL_A0;
		ADS1115_delay(5);
    }
    SCL_A0;
    ADS1115_delay(5);
    SDA_A0;
    ADS1115_delay(5);
    SCL_A1;
    ADS1115_delay(5);
    SCL_A0;
    ADS1115_delay(5);
    SDA_A1;
    return TData;
}



/*********************************************************************
*��������:  ADS1115Config
*��	   ���� ����ADS1115����ͨ�����ã�����ʱ��ȵ�
*��	   ���� HCMD �������ָ�8λ(ͨ�������̣�ת��ģʽ)
			LCMD : �����ֵ�8λ(���������� �Ƚ�ģʽ ��Ч��ƽ �ź��������)
*��	   �أ� ��
********************************************************************/
void ADS1115Config_A(u8 LCMD,u8 HCMD)
{
    u8 i=0;
    u8 Initdata[4];

    Initdata[0] = WR_REG;  // ��ַ0x90  ����ADR�ӵ� д�Ĵ���
    Initdata[1] = CONF_REG;// ���üĴ���
    Initdata[2] = HCMD;    // �����ָ��ֽ�
    Initdata[3] = LCMD;    // �����ֵ��ֽ�
    SCL_A1;
    I2CStart_A();        //����
    for(i=0;i<4;i++)
    {
    	I2CWriteByte_A(Initdata[i]);
    	ADS1115_delay(10);
    }
    I2CStop_A();         //�ر�
}

void SetThresHold_A(u16 L_TH,u16 H_TH)        //�ߵͷ�������
{
   SCL_A1;
   I2CStart_A();      // ����
   I2CWriteByte_A(WR_REG);
   I2CWriteByte_A(LOTH_REG);//��ͷ�ֵ�Ĵ���
   I2CWriteByte_A((L_TH>>8));
   I2CWriteByte_A(L_TH);
   I2CStop_A();       //�ر�

   I2CStart_A();     //����
   I2CWriteByte_A(WR_REG);
   I2CWriteByte_A(HITH_REG);//��߷�ֵ�Ĵ���
   I2CWriteByte_A((H_TH>>8));
   I2CWriteByte_A(H_TH);
   I2CStop_A();      //�ر�
}


/*******************************************************************
*��������:  ReadAD_A
*��	   ���� ��ȡADת����ֵ
*��	   ���� ��ȡ��ֵΪ��ǰ�����õ��Ǹ�ͨ��
*��	   �أ� ��
********************************************************************/
u16 ReadAD_A(void)
{
   u16 Data[2]={0,0};

   SCL_A1;
   I2CStart_A();
   I2CWriteByte_A(WR_REG);
   I2CWriteByte_A(DATA_REG);
   I2CStop_A();
   
   I2CStart_A();
   I2CWriteByte_A(RE_REG);
   Data[0] = I2CReadByte_A();
   Data[1] = I2CReadByte_A();
   I2CStop_A();
   
   Data[0] = Data[0]<<8 | Data[1];
   return  (Data[0]);//&0x7fff
}

u16 getad(u8 LCMD,u8 HCMD)
{
   float value=0;
 //   ADS1115_Init();  
  //  delay_nms(5);
	//delay_nms(50);
    ADS1115Config_A(LCMD,HCMD);		  //AINP = AIN0 and AINN = AIN1 (default)
    delay_nms(5); 
    value=ReadAD_A();
    return value;
}

u16 lvbo(u8 LCMD,u8 HCMD)        //��ƽ��ֵ
{
	int k;
	float U=0,temp;
	
	 
	for(k=0;k<500;k++)
	{
		U+=getad(LCMD,HCMD);	
	}
	temp=U;
		U=0;
	return (temp/500);	
}

u16 readAverage(u8 ch,u8 time)
{
	u8 i;
	int sum;
	for(i=0;i<time;i++)
	{
		sum+=READAD(ch);
	}
	return sum/time;
}



