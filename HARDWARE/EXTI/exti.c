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

  //KEY_Init();	 //	�����˿ڳ�ʼ��

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 
	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
 	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
 	EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

 	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//ʹ�����ڵ��ⲿ�ж�ͨ��
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//�����ȼ�3
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
 	NVIC_Init(&NVIC_InitStructure); 
	
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource2); 
//	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
// 	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
// 	//EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

// 	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//ʹ�����ڵ��ⲿ�ж�ͨ��
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//�����ȼ�3
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
// 	//NVIC_Init(&NVIC_InitStructure); 
}

//�ⲿ�ж�0������� 
void EXTI0_IRQHandler(void)
{
	delay_ms(10);//����
	if(KEY3==1)	 	 //WK_UP����
	{	
		g_Position = -45.000;
		EXTI->IMR&=~(1<<0);//����line0�ϵ��жϣ������ж�
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ  
}

//void EXTI2_IRQHandler(void)
//{
//	delay_ms(10);//����
//	if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2)==1)	 	 //���Ե����ж�
//	{	
//		g_State_Flag |= 0x10;//�õ���Ready��־
//	}
//	EXTI_ClearITPendingBit(EXTI_Line2); //���LINE0�ϵ��жϱ�־λ  
//}


