#include "exti.h"
#include "key.h"
#include "delay.h"
#include "usart.h"

extern float g_Position;
extern u8 g_State_Flag;

void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  //KEY_Init();	 //	按键端口初始化

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 
	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
 	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
 	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

 	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//使能所在的外部中断通道
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
 	NVIC_Init(&NVIC_InitStructure); 
	
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource2); 
//	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
// 	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
// 	//EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

// 	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//使能所在的外部中断通道
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
// 	//NVIC_Init(&NVIC_InitStructure); 
}

//外部中断0服务程序 
void EXTI0_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(KEY3==1)	 	 //WK_UP按键
	{	
		g_Position = -45.000;
		EXTI->IMR&=~(1<<0);//屏蔽line0上的中断，禁用中断
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位  
}

//void EXTI2_IRQHandler(void)
//{
//	delay_ms(10);//消抖
//	if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2)==1)	 	 //来自底盘中断
//	{	
//		g_State_Flag |= 0x10;//置底盘Ready标志
//	}
//	EXTI_ClearITPendingBit(EXTI_Line2); //清除LINE0上的中断标志位  
//}


