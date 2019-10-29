#include "Big_Digital_Tube.h"
#include "stm32f10x.h"

u8 LED_Lib[10] = {0x3F,0x0C,0x5B,0x5E,0x6C,0x76,0x77,0x1C,0x7F,0x7E};//字库

void me_LED_Display_Test()
{
	LED_Pin_Control(LED_Display_Test);
}
void me_LED_Display_Error()
{
	LED_Pin_Control(LED_Display_Error);
}

void LED_Display_Number(u8 input)
{
	if(input>9)
	{
		me_LED_Display_Error();
	}
	else
	{
		LED_Pin_Control(LED_Lib[input]);
	}
}

void LED_Pin_Control(u8 input)
{
//	u8 i = 0;
//	for(i = 0;i<7;i++)
//	{
//		LED_Out(i) = input&0x01;
//		input = input >> 1;
//	}
	LED_Port->ODR &= 0xFFFFFF80;
	input = ~input;
	LED_Port->ODR |= input;
}

void Big_LED_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;   //定义名为GPIO_InitStructure的结构体类型变量

	LED_IO_RCC;//打开管脚时钟
	
	//配置PWM输出引脚PX 0 1 2 3 4 5 6
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(LED_Port, &GPIO_InitStructure);
}
