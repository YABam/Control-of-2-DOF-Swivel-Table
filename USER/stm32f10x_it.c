/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "stm32f10x.h"
#include "sys.h"
#include "string.h"
#include "usart.h"
#include "LED.h"

extern unsigned char g_Re_buf[11],g_temp_buf[11],g_counter;
extern unsigned char g_sign,g_t,g_he;
u8 USART1_RX_BUF[200];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART1_RX_STA=0;       //接收状态标记

u16 UART4_RX_STA=0;//4 - 接收状态标记
u8 UART4_RX_BUF[200]; //接收缓冲

u16 UART5_RX_STA=0;//4 - 接收状态标记
u8 UART5_RX_BUF[200]; //接收缓冲

static unsigned char Temp[11];

 
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
		printf("HardFault");
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
		printf("MemManage");
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
		printf("BusFault");
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
		printf("UsageFault");
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
void USART1_IRQHandler(void)		   //串口1全局中断服务函数
{
	u8 Res1;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res1 =USART_ReceiveData(USART1);
		if((USART1_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART1_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res1!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
				
				else USART1_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res1==0x0d)USART1_RX_STA|=0x4000;
				else
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res1 ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(200-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		  
	}  
} 	 

void USART2_IRQHandler(void)		   //串口2全局中断服务函数
{
	if(USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)
	{
		LED1=~LED1;
		USART_ClearITPendingBit(USART2,USART_IT_ORE);
		USART_ReceiveData(USART2);
	}
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断有效,若接收数据寄存器满
	{
		LED0=0;
		USART_ClearFlag(USART2, USART_FLAG_RXNE);  
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); 
		USART_ClearITPendingBit(USART2,USART_IT_ORE);	
		
		Temp[g_counter] = USART_ReceiveData(USART2);   //接收数据
		//网购给的程序
		if(g_counter == 0 && Temp[0] != 0x55) 
		{
			g_counter = 0;
			return;      //第 0 号数据不是帧头，跳过
		}
		g_counter++; 
		if(g_counter>=11) //接收到 11 个数据
		{ 
			memcpy(g_Re_buf,Temp,11);
			g_counter=0; //重新赋值，准备下一帧数据的接收
			g_sign=1;
		}
		LED0=1;
	}
}

void USART3_IRQHandler(void)		   //串口3全局中断服务函数
{
}

void EXTI15_10_IRQHandler(void)		   //外部中断10【15:10】中断服务函数
{
}

void RTCAlarm_IRQHandler(void)		   //联到EXTI的RTC闹钟中断服务函数
{
}

void USBWakeUp_IRQHandler(void)		   //联到EXTI的从USB待机唤醒中断服务函数
{
}

void TIM8_BRK_IRQHandler(void)		   //定时器8断开中断服务函数
{
}

void TIM8_UP_IRQHandler(void)		   //定时器8更新中断服务函数
{
}

void TIM8_TRG_COM_IRQHandler(void)	   //定时器8触发和通信中断服务函数
{
}

void TIM8_CC_IRQHandler(void)		   //定时器8捕获比较中断服务函数
{
}

void ADC3_IRQHandler(void)			   //ADC3全局中断服务函数
{
}

void FSMC_IRQHandler(void)			   //FSMC全局中断服务函数
{
}

void SDIO_IRQHandler(void)			   //SDIO全局中断服务函数
{
}

void TIM5_IRQHandler(void)			   //定时器5全局中断服务函数
{
}

void SPI3_IRQHandler(void)			   //SPI3全局中断服务函数
{
}

void UART4_IRQHandler(void)			   //串口4全局中断服务函数
{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(UART4);//(USART1->DR);	//读取接收到的数据
		
		if((UART4_RX_STA&0x8000)==0)//接收未完成
			{
			if(UART4_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)UART4_RX_STA=0;//接收错误,重新开始
				else UART4_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)UART4_RX_STA|=0x4000;
				else
					{
					UART4_RX_BUF[UART4_RX_STA&0X3FFF]=Res ;
					UART4_RX_STA++;
					if(UART4_RX_STA>(200-1))UART4_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
} 

void UART5_IRQHandler(void)			   //串口5全局中断服务函数
{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();    
#endif
	
	USART_ClearFlag(USART2, USART_FLAG_RXNE);  
	USART_ClearITPendingBit(USART2, USART_IT_RXNE); 
	USART_ClearITPendingBit(USART2,USART_IT_ORE);	
	
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		if(UART5_RX_STA < 11)
		{
			
			Res = USART_ReceiveData(UART5);//(USART1->DR);	//读取接收到的数据
			UART5_RX_BUF[UART5_RX_STA&0X3FFF]=Res;
			UART5_RX_STA++;//累加
			if(UART5_RX_STA == 11)
			{
				UART5_RX_STA |= 0x8000;
			}
		}
		else
		{
			if(!UART5_RX_STA & 0x8000)
			{
				UART5_RX_STA = 0;//错误
			}
		}
		
//		if((UART5_RX_STA&0x8000)==0)//接收未完成
//			{
//			if(UART5_RX_STA&0x4000)//接收到了0x0d
//				{
//					if(Res == 0x0a)UART5_RX_STA|=0x8000;//接收完成
//					else UART5_RX_STA|=0x8000;	//接收完成了 
//				}
//			else //还没收到0X0D
//				{	
//				if(Res==0x0d)UART5_RX_STA|=0x4000;
//				else
//					{
//					UART5_RX_BUF[UART5_RX_STA&0X3FFF]=Res ;
//					UART5_RX_STA++;
//					if(UART5_RX_STA>(200-1))UART5_RX_STA=0;//接收数据错误,重新开始接收	  
//					}		 
//				}
//			}   		 
     } 
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif	 
}

void TIM6_IRQHandler(void)			   //定时器6全局中断服务函数
{
}

void TIM7_IRQHandler(void)			   //定时器7全局中断服务函数
{
}
