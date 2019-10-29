//Stm32_Init()
#include "stm32f10x.h"
#include "string.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "exti.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "Math.h"
#include "init.h"

//#include "switch.h"

//======================================
extern u8 g_SWITCH_Debug_Mode_Flag;
extern u8 g_SWITCH_LED_Display_Flag;

extern u8 g_SWITCH_Training_Flag;
extern u8 g_SWITCH_Forecast_Flag;

//extern u8 g_SWITCH_BLUE_Flag;
extern u8 g_SWITCH_RED_Flag;
//======================================

//����ϵͳʱ��
void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;                    //�����ⲿ���پ�������״̬ö�ٱ���

	RCC_DeInit();                                    //��λRCC�ⲿ�豸�Ĵ�����Ĭ��ֵ
	RCC_HSEConfig(RCC_HSE_ON);                       //���ⲿ���پ���
	HSEStartUpStatus = RCC_WaitForHSEStartUp();      //�ȴ��ⲿ����ʱ��׼����
	if(HSEStartUpStatus == SUCCESS)                  //�ⲿ����ʱ���Ѿ�׼���
	{
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //����FLASHԤ�����幦�ܣ�����FLASH�Ķ�ȡ�����г����б�����÷�.λ�ã�RCC��ʼ���Ӻ������棬ʱ������֮��
		FLASH_SetLatency(FLASH_Latency_2);                    //flash��������ʱ
			
		RCC_HCLKConfig(RCC_SYSCLK_Div1);               //����AHB(HCLK)ʱ�ӵ���==SYSCLK
		RCC_PCLK2Config(RCC_HCLK_Div1);                //����APB2(PCLK2)��==AHBʱ��
		RCC_PCLK1Config(RCC_HCLK_Div2);                //����APB1(PCLK1)��==AHB1/2ʱ��
			 
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);  //����PLLʱ�� == �ⲿ���پ���ʱ�� * 9 = 72MHz
		RCC_PLLCmd(ENABLE);                                   //ʹ��PLLʱ��
   
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)    //�ȴ�PLLʱ�Ӿ���
		{
		}
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);            //����ϵͳʱ�� = PLLʱ��
		while(RCC_GetSYSCLKSource() != 0x08)                  //���PLLʱ���Ƿ���Ϊϵͳʱ��
		{
		}
	}
}

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;   //������ΪGPIO_InitStructure�Ľṹ�����ͱ���

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //��GPIOA��ʱ�ӣ���ʹ�ܡ�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);  //��GPIOB��GPIOD��ʱ�ӣ���ʹ�ܡ�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);  //��GPIOE��ʱ�ӣ���ʹ�ܡ�
	
	//����PWM�������PA8
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//�Ƹ������̵�����������	PB6
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//��ʼ��״̬���� PE 8 9 10 11 12 13 14 15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
}

 /*************************************************
����: void TIM1_Configuration(void)
����: ���ö�ʱ��1�����ڷ���pwm��
����: ��
����: ��
**************************************************/
void  TIM1_Configuration(void) 
{
	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
 	//TIM1�������������ã�����PWMƵ�ʣ� 
 	//Ƶ��=TIM1_CLK/(ARR+1) 
 	TIM_BaseInitStructure.TIM_Period = 10000;
 	TIM_BaseInitStructure.TIM_Prescaler = 72;
 	TIM_BaseInitStructure.TIM_ClockDivision = 0;
 	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
 	TIM_BaseInitStructure.TIM_RepetitionCounter = 0;
 	TIM_TimeBaseInit(TIM1, &TIM_BaseInitStructure);
 	//����ARR��Ӱ�ӼĴ�����ֱ�����������¼��Ÿ������ã� 
 	TIM_ARRPreloadConfig(TIM1, ENABLE);
 	//TIM1_OC1ģ�����ã�����1ͨ��ռ�ձȣ� 
 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
 	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
 	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
 	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	 
 	TIM_OCInitStructure.TIM_Pulse = 500;//PA8
 	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
 	//����CCR1�Ĵ�����Ӱ�ӼĴ�����ֱ�����������¼��Ÿ������ã� 
 	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
// 	//TIM1_OC4ģ�����ã�����4ͨ��ռ�ձȣ� 	
//	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
// 	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
// 	//����CCR4�Ĵ�����Ӱ�ӼĴ�����ֱ�����������¼��Ÿ������ã� 
// 	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
 	//TIM1���� 
 	TIM_Cmd(TIM1, ENABLE);
 	//�ر�TIM1_OCͨ�����PWM��һ��Ҫ�ӣ� 
 	TIM_CtrlPWMOutputs(TIM1, DISABLE);
 }

void Stm32_Init()
{
	RCC_Configuration();
	GPIO_Configuration();      //��ʼ��IO�˿�
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	LED_Init(); //LED��ʼ��
	USART1_Config();   //��ʼ�� ����USART1
  USART2_Config();   //��ʼ�� ����USART2 �����ڽ���MPU6050���ݣ�
	USART3_Config();   //��ʼ�� ����USART3  �����ڿ�����������
	UART4_Config();    //��ʼ�� ����UART4  ����λ��ͨ�ţ�
	UART5_Config();    //��ʼ�� ����UART5 ����λ��ͨ�ţ�
	//LCD_Init();//����LCD��ʾ������
	TIM1_Configuration();
	TIM2_PWM_Init(14399,99); //����Ƶ��PWMƵ��=72000000/14400=50hz
	KEY_Init();         	//��ʼ���밴�����ӵ�Ӳ���ӿ�
	EXTIX_Init();		 	//�ⲿ�жϳ�ʼ��
	
	//PE8 = 1/N - �����쳡 0 - ��������
	//PE9 = 1/N - ����Ԥ�� 0 - ����ѵ��
	//PE10 = 1/N - ��������� 0 - �ر������
	//PE11 = 1/N - �ر�debug 0 - ����Debug
	if(PEin(8) == 1)
	{
		g_SWITCH_RED_Flag = 1;
		//g_SWITCH_BLUE_Flag = 0;
	}
	else
	{
		g_SWITCH_RED_Flag = 0;
		//g_SWITCH_BLUE_Flag = 1;
	}
	
	if(PEin(9) == 1)
	{
		g_SWITCH_Forecast_Flag = 1;
		g_SWITCH_Training_Flag = 0;
	}
	else
	{
		g_SWITCH_Forecast_Flag = 0;
		g_SWITCH_Training_Flag = 1;
	}
	
	if(PEin(10) == 1)
	{
		g_SWITCH_LED_Display_Flag = 1;
	}
	
	else
	{
		g_SWITCH_LED_Display_Flag = 0;
	}
	
	if(PEin(11) == 1)
	{
		g_SWITCH_Debug_Mode_Flag = 0;
	}
	else
	{
		g_SWITCH_Debug_Mode_Flag = 1;
	}
}
