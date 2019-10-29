//数码管驱动芯片7219相关控制

#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "LED_Digital_Tube.h"

#define Max7219_pinCLK PAout(6)
#define Max7219_pinCS PAout(5)
#define Max7219_pinDIN PAout(4)

extern u8 g_Robot_Position;
extern u8 g_Target_Number;

//写入字节
void Write_Max7219_byte(u8 DATA)
{
	u8 i;
	Max7219_pinCS = 0;
	for(i=8;i>=1;i--)
	{
		Max7219_pinCLK = 0;
		//Max7219_pinDIN = DATA&0x80;
		if(DATA&0x80)
		{
			Max7219_pinDIN = 1;
		}
		else
		{
			Max7219_pinDIN = 0;
		}
		
		DATA = DATA<<1;
		Max7219_pinCLK = 1;
	}
}

//写入数据（多字节）
void Write_Max7219(u8 address, u8 data)
{
	Max7219_pinCS = 0;
	Write_Max7219_byte(address);
	Write_Max7219_byte(data);
	Max7219_pinCS = 1;
}


void Init_MAX7219(void)//7219初始化  CS - PA5 DIN - PA4 CLK - PA6
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PB,PE端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_4 | GPIO_Pin_5;				 //LED0-->PB.5 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	
	Write_Max7219(0x09, 0xff);//BCD译码
	Write_Max7219(0x0a, 0x03);//亮度
	Write_Max7219(0x0b, 0x07);//全部扫描显示
	Write_Max7219(0x0c, 0x01);//掉电模式：0 普通模式：1
	Write_Max7219(0x0f, 0x01);//显示测试：1 测试结束正常显示：0
}

void MAX7219_Ready(void)
{
	Write_Max7219(0x0f,0x00);//结束LED测试，正常显示
	Max7219_pinCLK = 1;
	Max7219_pinCS = 1;
}

void LED_Display(float input_Adata,float input_Cdata)
{
	u8 Display_Buffer[8] = {0,0,0,0,0,0,0,0};
	u8 Display_i = 0;
	
	if(input_Adata>=100 || input_Adata <= -100 || input_Cdata >=100 || input_Cdata <= -100)
	{
			if(input_Cdata >=800&&input_Cdata<=1400)//转速，表示状态显示
			{
				Display_Buffer[7] = g_Robot_Position;
				Display_Buffer[6] = 10;
				Display_Buffer[5] = g_Target_Number;
				Display_Buffer[4] = 10;
				if(input_Cdata<1000)
				{
					Display_Buffer[3] =	0;
					Display_Buffer[2] = (u8)(input_Cdata/100);
					Display_Buffer[1] = (u8)(((input_Cdata) - Display_Buffer[2] * 100) / 10);
					Display_Buffer[0] = (u8)(input_Cdata - Display_Buffer[2] * 100 - Display_Buffer[1] * 10);
				}
				else
				{
					Display_Buffer[3] =	(u8)(input_Cdata/1000);
					Display_Buffer[2] = (u8)((input_Cdata - Display_Buffer[3] * 1000) / 100);
					Display_Buffer[1] = (u8)((input_Cdata - Display_Buffer[3] * 1000 - Display_Buffer[2] * 100) / 10);
					Display_Buffer[0] = (u8)(input_Cdata - Display_Buffer[3] * 1000 - Display_Buffer[2] * 100 - Display_Buffer[1] * 10);
				}
			}
			else
			{
				for(Display_i = 0;Display_i <8;Display_i ++)
				{
					//Write_Max7219(Display_i+1,10);
				}
				return;
			}
	}
	else
	{
		if(input_Adata<0)//X轴俯仰旋转角
		{
			input_Adata *= -1;
			Display_Buffer[7] = 10;
			Display_Buffer[6] = (u8)(input_Adata/10);
			Display_Buffer[5] = (u8)input_Adata-Display_Buffer[6] * 10;
			Display_Buffer[4] = (u8)(input_Adata * 10) - Display_Buffer[5] * 10 - Display_Buffer[6] * 100;
			Display_Buffer[5] = Display_Buffer[5] | 0x80;//添加小数点
		}
		else
		{
			Display_Buffer[7] = (u8)(input_Adata/10);
			Display_Buffer[6] = (u8)input_Adata-Display_Buffer[7] * 10;
			Display_Buffer[5] = (u8)(input_Adata * 10) - Display_Buffer[6] * 10 - Display_Buffer[7] * 100;
			Display_Buffer[4] = (u8)(input_Adata * 100) - Display_Buffer[5] * 10 - Display_Buffer[6] * 100 - Display_Buffer[7] * 1000;
			Display_Buffer[6] = Display_Buffer[6] | 0x80;
		}
		
		if(input_Cdata<0)//Z轴偏航旋转角
		{
			input_Cdata *= -1;
			Display_Buffer[3] = 10;
			Display_Buffer[2] = (u8)(input_Cdata/10);
			Display_Buffer[1] = (u8)input_Cdata - Display_Buffer[2] * 10;
			Display_Buffer[0] = (u8)(input_Cdata * 10) - Display_Buffer[1] * 10 - Display_Buffer[2] * 100;
			Display_Buffer[1] = Display_Buffer[1] | 0x80;
		}
		else
		{
			Display_Buffer[3] = (u8)(input_Cdata/10);
			Display_Buffer[2] = (u8)input_Cdata-Display_Buffer[3] * 10;
			Display_Buffer[1] = (u8)(input_Cdata * 10) - Display_Buffer[2] * 10 - Display_Buffer[3] * 100;
			Display_Buffer[0] = (u8)(input_Cdata * 100) - Display_Buffer[1] * 10 - Display_Buffer[2] * 100 - Display_Buffer[3] * 1000;
			Display_Buffer[2] = Display_Buffer[2] | 0x80;
		}
	}
	
	//8个数码管显示
	Write_Max7219(0x0f,0x00);//结束LED测试，正常显示
	for(Display_i = 0;Display_i<8;Display_i++)
	{
		Write_Max7219(Display_i+1,Display_Buffer[Display_i]);
	}
}

void LED_Msg_Display(u8 cmd,int Adata,int Bdata)
{
	u8 Display_Buffer[8] = {0,0,0,0,0,0,0,0};
	u8 Display_i = 0;
	
	if(cmd == 0)//分割
	{
		Display_Buffer[0] = 10;
		Display_Buffer[1] = 10;
		Display_Buffer[2] = 10;
		Display_Buffer[3] = 10;
		Display_Buffer[4] = 10;
		Display_Buffer[5] = 10;
		Display_Buffer[6] = 10;
		Display_Buffer[7] = 10;
	}
	else if(cmd == 1)//A - VERSION B - RED/BLUE
	{
		Display_Buffer[7] = (u8)(Adata / 1000);
		Display_Buffer[6] = (u8)((Adata - Display_Buffer[7]*1000)/100);
		Display_Buffer[5] = (u8)((Adata - Display_Buffer[7]*1000 - Display_Buffer[6]*100)/10);
		Display_Buffer[4] = (u8)((Adata - Display_Buffer[7]*1000 - Display_Buffer[6]*100 - Display_Buffer[5]*10));
		
		Display_Buffer[3] = 10;
		Display_Buffer[2] = 10;
		Display_Buffer[1] = 10;
		if(Bdata == 1)//RED
		{
			Display_Buffer[0] = 0x0C;//H
		}
		else
		{
			Display_Buffer[0] = 0x0D;//L
		}
	}
	else if(cmd == 2)//A - 1=Forecast/0=Training B - 1=Deubg mode
	{
		Display_Buffer[7] = 10;
		Display_Buffer[6] = 10;
		
		Display_Buffer[5] = Adata;
		
		Display_Buffer[4] = 10;
		
		Display_Buffer[3] = 10;
		Display_Buffer[2] = Bdata;
		Display_Buffer[1] = 10;
		Display_Buffer[0] = 10;                                    
	}
	
	//8个数码管显示
	Write_Max7219(0x0f,0x00);//结束LED测试，正常显示
	for(Display_i = 0;Display_i<8;Display_i++)
	{
		Write_Max7219(Display_i+1,Display_Buffer[Display_i]);
	}
}
