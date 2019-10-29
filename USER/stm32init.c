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

//配置系统时钟
void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;                    //定义外部高速晶体启动状态枚举变量

	RCC_DeInit();                                    //复位RCC外部设备寄存器到默认值
	RCC_HSEConfig(RCC_HSE_ON);                       //打开外部高速晶振
	HSEStartUpStatus = RCC_WaitForHSEStartUp();      //等待外部高速时钟准备好
	if(HSEStartUpStatus == SUCCESS)                  //外部高速时钟已经准别好
	{
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //开启FLASH预读缓冲功能，加速FLASH的读取。所有程序中必须的用法.位置：RCC初始化子函数里面，时钟起振之后
		FLASH_SetLatency(FLASH_Latency_2);                    //flash操作的延时
			
		RCC_HCLKConfig(RCC_SYSCLK_Div1);               //配置AHB(HCLK)时钟等于==SYSCLK
		RCC_PCLK2Config(RCC_HCLK_Div1);                //配置APB2(PCLK2)钟==AHB时钟
		RCC_PCLK1Config(RCC_HCLK_Div2);                //配置APB1(PCLK1)钟==AHB1/2时钟
			 
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);  //配置PLL时钟 == 外部高速晶体时钟 * 9 = 72MHz
		RCC_PLLCmd(ENABLE);                                   //使能PLL时钟
   
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)    //等待PLL时钟就绪
		{
		}
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);            //配置系统时钟 = PLL时钟
		while(RCC_GetSYSCLKSource() != 0x08)                  //检查PLL时钟是否作为系统时钟
		{
		}
	}
}

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;   //定义名为GPIO_InitStructure的结构体类型变量

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //打开GPIOA的时钟，并使能。
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);  //打开GPIOB和GPIOD的时钟，并使能。
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);  //打开GPIOE的时钟，并使能。
	
	//配置PWM输出引脚PA8
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//推杆驱动继电器控制引脚	PB6
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//初始化状态输入 PE 8 9 10 11 12 13 14 15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
}

 /*************************************************
函数: void TIM1_Configuration(void)
功能: 配置定时器1，用于发送pwm波
参数: 无
返回: 无
**************************************************/
void  TIM1_Configuration(void) 
{
	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
 	//TIM1基本计数器设置（设置PWM频率） 
 	//频率=TIM1_CLK/(ARR+1) 
 	TIM_BaseInitStructure.TIM_Period = 10000;
 	TIM_BaseInitStructure.TIM_Prescaler = 72;
 	TIM_BaseInitStructure.TIM_ClockDivision = 0;
 	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
 	TIM_BaseInitStructure.TIM_RepetitionCounter = 0;
 	TIM_TimeBaseInit(TIM1, &TIM_BaseInitStructure);
 	//启用ARR的影子寄存器（直到产生更新事件才更改设置） 
 	TIM_ARRPreloadConfig(TIM1, ENABLE);
 	//TIM1_OC1模块设置（设置1通道占空比） 
 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
 	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
 	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
 	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	 
 	TIM_OCInitStructure.TIM_Pulse = 500;//PA8
 	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
 	//启用CCR1寄存器的影子寄存器（直到产生更新事件才更改设置） 
 	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
// 	//TIM1_OC4模块设置（设置4通道占空比） 	
//	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
// 	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
// 	//启用CCR4寄存器的影子寄存器（直到产生更新事件才更改设置） 
// 	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
 	//TIM1开启 
 	TIM_Cmd(TIM1, ENABLE);
 	//关闭TIM1_OC通道输出PWM（一定要加） 
 	TIM_CtrlPWMOutputs(TIM1, DISABLE);
 }

void Stm32_Init()
{
	RCC_Configuration();
	GPIO_Configuration();      //初始化IO端口
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	LED_Init(); //LED初始化
	USART1_Config();   //初始化 配置USART1
  USART2_Config();   //初始化 配置USART2 （用于接收MPU6050数据）
	USART3_Config();   //初始化 配置USART3  （用于控制驱动器）
	UART4_Config();    //初始化 配置UART4  （上位机通信）
	UART5_Config();    //初始化 配置UART5 （上位机通信）
	//LCD_Init();//用于LCD显示屏调试
	TIM1_Configuration();
	TIM2_PWM_Init(14399,99); //不分频。PWM频率=72000000/14400=50hz
	KEY_Init();         	//初始化与按键连接的硬件接口
	EXTIX_Init();		 	//外部中断初始化
	
	//PE8 = 1/N - 开启红场 0 - 开启蓝场
	//PE9 = 1/N - 开启预测 0 - 开启训练
	//PE10 = 1/N - 开启数码管 0 - 关闭数码管
	//PE11 = 1/N - 关闭debug 0 - 开启Debug
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
