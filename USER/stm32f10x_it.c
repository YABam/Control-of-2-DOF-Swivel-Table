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
u8 USART1_RX_BUF[200];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART1_RX_STA=0;       //����״̬���

u16 UART4_RX_STA=0;//4 - ����״̬���
u8 UART4_RX_BUF[200]; //���ջ���

u16 UART5_RX_STA=0;//4 - ����״̬���
u8 UART5_RX_BUF[200]; //���ջ���

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
void USART1_IRQHandler(void)		   //����1ȫ���жϷ�����
{
	u8 Res1;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res1 =USART_ReceiveData(USART1);
		if((USART1_RX_STA&0x8000)==0)//����δ���
		{
			if(USART1_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res1!=0x0a)USART1_RX_STA=0;//���մ���,���¿�ʼ
				
				else USART1_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res1==0x0d)USART1_RX_STA|=0x4000;
				else
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res1 ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(200-1))USART1_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		  
	}  
} 	 

void USART2_IRQHandler(void)		   //����2ȫ���жϷ�����
{
	if(USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)
	{
		LED1=~LED1;
		USART_ClearITPendingBit(USART2,USART_IT_ORE);
		USART_ReceiveData(USART2);
	}
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж���Ч,���������ݼĴ�����
	{
		LED0=0;
		USART_ClearFlag(USART2, USART_FLAG_RXNE);  
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); 
		USART_ClearITPendingBit(USART2,USART_IT_ORE);	
		
		Temp[g_counter] = USART_ReceiveData(USART2);   //��������
		//�������ĳ���
		if(g_counter == 0 && Temp[0] != 0x55) 
		{
			g_counter = 0;
			return;      //�� 0 �����ݲ���֡ͷ������
		}
		g_counter++; 
		if(g_counter>=11) //���յ� 11 ������
		{ 
			memcpy(g_Re_buf,Temp,11);
			g_counter=0; //���¸�ֵ��׼����һ֡���ݵĽ���
			g_sign=1;
		}
		LED0=1;
	}
}

void USART3_IRQHandler(void)		   //����3ȫ���жϷ�����
{
}

void EXTI15_10_IRQHandler(void)		   //�ⲿ�ж�10��15:10���жϷ�����
{
}

void RTCAlarm_IRQHandler(void)		   //����EXTI��RTC�����жϷ�����
{
}

void USBWakeUp_IRQHandler(void)		   //����EXTI�Ĵ�USB���������жϷ�����
{
}

void TIM8_BRK_IRQHandler(void)		   //��ʱ��8�Ͽ��жϷ�����
{
}

void TIM8_UP_IRQHandler(void)		   //��ʱ��8�����жϷ�����
{
}

void TIM8_TRG_COM_IRQHandler(void)	   //��ʱ��8������ͨ���жϷ�����
{
}

void TIM8_CC_IRQHandler(void)		   //��ʱ��8����Ƚ��жϷ�����
{
}

void ADC3_IRQHandler(void)			   //ADC3ȫ���жϷ�����
{
}

void FSMC_IRQHandler(void)			   //FSMCȫ���жϷ�����
{
}

void SDIO_IRQHandler(void)			   //SDIOȫ���жϷ�����
{
}

void TIM5_IRQHandler(void)			   //��ʱ��5ȫ���жϷ�����
{
}

void SPI3_IRQHandler(void)			   //SPI3ȫ���жϷ�����
{
}

void UART4_IRQHandler(void)			   //����4ȫ���жϷ�����
{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(UART4);//(USART1->DR);	//��ȡ���յ�������
		
		if((UART4_RX_STA&0x8000)==0)//����δ���
			{
			if(UART4_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)UART4_RX_STA=0;//���մ���,���¿�ʼ
				else UART4_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)UART4_RX_STA|=0x4000;
				else
					{
					UART4_RX_BUF[UART4_RX_STA&0X3FFF]=Res ;
					UART4_RX_STA++;
					if(UART4_RX_STA>(200-1))UART4_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
} 

void UART5_IRQHandler(void)			   //����5ȫ���жϷ�����
{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
	
	USART_ClearFlag(USART2, USART_FLAG_RXNE);  
	USART_ClearITPendingBit(USART2, USART_IT_RXNE); 
	USART_ClearITPendingBit(USART2,USART_IT_ORE);	
	
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		if(UART5_RX_STA < 11)
		{
			
			Res = USART_ReceiveData(UART5);//(USART1->DR);	//��ȡ���յ�������
			UART5_RX_BUF[UART5_RX_STA&0X3FFF]=Res;
			UART5_RX_STA++;//�ۼ�
			if(UART5_RX_STA == 11)
			{
				UART5_RX_STA |= 0x8000;
			}
		}
		else
		{
			if(!UART5_RX_STA & 0x8000)
			{
				UART5_RX_STA = 0;//����
			}
		}
		
//		if((UART5_RX_STA&0x8000)==0)//����δ���
//			{
//			if(UART5_RX_STA&0x4000)//���յ���0x0d
//				{
//					if(Res == 0x0a)UART5_RX_STA|=0x8000;//�������
//					else UART5_RX_STA|=0x8000;	//��������� 
//				}
//			else //��û�յ�0X0D
//				{	
//				if(Res==0x0d)UART5_RX_STA|=0x4000;
//				else
//					{
//					UART5_RX_BUF[UART5_RX_STA&0X3FFF]=Res ;
//					UART5_RX_STA++;
//					if(UART5_RX_STA>(200-1))UART5_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
//					}		 
//				}
//			}   		 
     } 
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif	 
}

void TIM6_IRQHandler(void)			   //��ʱ��6ȫ���жϷ�����
{
}

void TIM7_IRQHandler(void)			   //��ʱ��7ȫ���жϷ�����
{
}
