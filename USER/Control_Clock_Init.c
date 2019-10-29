//Control_Clock_Init()
#include "stm32f10x.h"
#include "sys.h"
#include "exti.h"
#include "led.h"
#include "timer.h"
#include "init.h"

/*************************************************
����: void TIM3_Int_Init(u16 arr, u16 psc)
����: ���ö�ʱ��3������ͬ������
����: arr psc
����: ��
**************************************************/
void TIM3_Int_Init(u16 arr, u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStructure; 		
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
 														 //��ʱ��TIM3��ʼ��
 	TIM_TimeBaseStructure.TIM_Period = arr - 1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
 	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
 	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
 	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
 	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 	//TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
  NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
 
  TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx			 
}

void TIM4_Int_Init(u16 arr, u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStructure; 		
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
 														 //��ʱ��TIM3��ʼ��
 	TIM_TimeBaseStructure.TIM_Period = arr - 1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
 	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
 	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
 	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
 	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�0��
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
  NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
 
  TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx			 
}



void Control_Clock_Init(void)
{
	TIM3_Int_Init(10000, 72);
	TIM4_Int_Init(1000,72);//ms
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//ʹ��TIM3�жϿ�������
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//ʹ��TIM4�ж�
}
