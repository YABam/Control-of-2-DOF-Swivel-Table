//Control(set_x_angle.T_PWM);
#include "stm32f10x.h"
#include "string.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "exti.h"
#include "PushRod.h"
#include "led.h"
#include "driver.h"
#include "AX12.h" 
#include "timer.h"
#include "lcd.h"
#include "Math.h"
#include "Control.h"

extern float g_angle[3];
extern float dangle;
extern float dangle_l1;
extern float dangle_l2;
extern float set_angle;

extern u8 g_Usart_Print_Flag;
extern u8 g_State_Flag;
extern u8 g_Ready_Counter;

//MPU9250����ȫ��
extern unsigned char g_Re_buf[11],g_temp_buf[11];
extern unsigned char g_sign,g_t,g_he;

float Kp_Pusher = 1060;//285;//65.5;       // �������� Proportional Const  24  
float Kd_Pusher = 19650;//3115.44;//26750;//9;//5.6;       // ΢�ֳ��� Derivative Const 
float Ki_Pusher = 6.4;//58.43;//39;//0.85;//0.29
	
//float I_Index=0;//�����ϵ��
float integral=0;//�洢���ֻ���

extern u8 Friction_FLAG;
extern u8 g_Friction_Ready;

u8 g_Terrace_Lock_Flag = 0;

void Get_Data(void)
{
	unsigned char me_Temp[11];
	if(g_sign)
	{ 
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);		//���ж�
		memcpy(me_Temp,g_Re_buf,11);
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);     //���ж�
		g_sign=0;
		if(me_Temp[0]==0x55)       //���֡ͷ
		{  
			switch(me_Temp[1])
			{
			//case 0x51: //��ʶ������Ǽ��ٶȰ�
				//   a[0] = ((short)(Temp[3]<<8 | Temp[2]))/32768.0*16;      //X����ٶ�
				//   a[1] = ((short)(Temp[5]<<8 | Temp[4]))/32768.0*16;      //Y����ٶ�
				//   a[2] = ((short)(Temp[7]<<8 | Temp[6]))/32768.0*16;      //Z����ٶ�
				//   T    = ((short)(Temp[9]<<8 | Temp[8]))/340.0+36.25;      //�¶�
				//   break;
			//case 0x52: //��ʶ������ǽ��ٶȰ�
				//   w[0] = ((short)(Temp[3]<<8| Temp[2]))/32768.0*2000;      //X����ٶ�
				//   w[1] = ((short)(Temp[5]<<8| Temp[4]))/32768.0*2000;      //Y����ٶ�
				//   w[2] = ((short)(Temp[7]<<8| Temp[6]))/32768.0*2000;      //Z����ٶ�
				//T    = ((short)(Temp[9]<<8| Temp[8]))/340.0+36.25;      //�¶�
				//   break;
			case 0x53: //��ʶ������ǽǶȰ�
				g_angle[0] = ((short)(me_Temp[3]<<8| me_Temp[2]))/32768.0*180;   //X���ת�ǣ�x �ᣩ
				g_angle[1] = ((short)(me_Temp[5]<<8| me_Temp[4]))/32768.0*180;   //Y�ḩ���ǣ�y �ᣩ
				g_angle[2] = ((short)(me_Temp[7]<<8| me_Temp[6]))/32768.0*180;   //Z��ƫ���ǣ�z �ᣩ
				break;
			default:  break;
			}
		}
	}
}

void Control_X(float input_set_angle)
{
//	float Kp_fix = 0;
//	float Kd_fix = 0;
//	float Ki_fix = 0;
	
	Get_Data();

	dangle_l1=dangle;//������һ�εĲ�ֵ
	dangle=input_set_angle - g_angle[1];//����dangle
		
	if(dangle<=0.05 && dangle>=-0.05)
	{
		g_Ready_Counter++;
	}
	else if(dangle>0.05 || dangle<-0.05)
	{
		g_Ready_Counter=0;
		//Terrace_Unlock();
		g_State_Flag &= 0x1F;//��x��z����̨Ready��־
	}
	
	if(g_Ready_Counter>=20)
	{
		g_Ready_Counter --;
		g_State_Flag |= 0xE0;//����̨Ready��־
	}

	if(g_State_Flag & 0xE0)
	{
		integral = 0;//�������������
	}
	else
	{
		integral=Integral_Pusher(integral,dangle);
	}
	
	Active_Pusher(dangle,dangle_l1,Kp_Pusher,Kd_Pusher,Ki_Pusher,integral);	
}

void PWM_GPIO_Change(int input)
{
	GPIO_InitTypeDef   GPIO_InitStructure;   //������ΪGPIO_InitStructure�Ľṹ�����ͱ���
	if(input == 1)
	{
		//����PWM�������PA8
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else
	{
		//����PWM�������PA8
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
}

void Terrace_Lock()
{
	g_Terrace_Lock_Flag = 1;
	set_angle = g_angle[1];
	TIM1->CCR1 = 5000;
}

void Terrace_Unlock()
{
	g_Terrace_Lock_Flag = 0;
	integral = 0;
}

//ȫ�ֱ���
//const int min = 800;//Ħ������Сת��
//const int max = 1400;//Ħ��������ٶ�����

u8 Friction_Speed(int T_PWM)
{
	int temp_PWM=TIM2->CCR2;
	
	if(T_PWM > 1400)
	{
		T_PWM = 1400;
	}
	else if(T_PWM < 800)
	{
		T_PWM = 800;
	}
	
	if(Friction_FLAG == 1)//��Ҫ�ı��ٶ�
	{
		Friction_FLAG = 0;
		if(T_PWM < temp_PWM)//��ζ�ż���
		{
			temp_PWM -=1;
			if(temp_PWM < T_PWM)//�ٶȼ���
			{
				temp_PWM = T_PWM;
				TIM2->CCR2 = temp_PWM;
				return 1;
			}
			else
			{
				TIM2->CCR2 = temp_PWM;
				return 0;
			}
		}
		else if(T_PWM > temp_PWM)//��ζ�ż���
		{
			temp_PWM +=1;
			if(temp_PWM > T_PWM)//�ٶȼӹ�
			{
				temp_PWM = T_PWM;
				TIM2->CCR2 = temp_PWM;
				return 1;
			}
			else
			{
				TIM2->CCR2 = temp_PWM;
				return 0;
			}
		}
		else//�Ѿ��ﵽĿ���ٶ�
		{
			return 1;
		}
	}
	else//������ȷ���Ƿ�ﵽ
	{
		if(temp_PWM == T_PWM)
		{
			return 1;
		}
		else
		{
			return 0;
		}
 }
}

void AX_12_Reset(void)
{
	//�͵�
	//SetAngle(0x0a,350);
	//delay_ms(350);
	SetAngle(0x0a,630);     //0x01
	delay_ms(110);

	//�������
	SetAngle(0x01,750);
	delay_ms(80);
	SetAngle(0x03,230);
	delay_ms(10);
	SetAngle(0x28,230);
	delay_ms(120);
}

void AX_12_Active(float target)
{
	__disable_irq();// �ر����ж�
	
		//���ж�
	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE); //ʹ��ָ����TIM3�ж�,��������ж�
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE); //ʹ��ָ����TIM3�ж�,��������ж�
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//�����ж�
	USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);//�����ж�
	
			//���̧��
			
			SetAngle(0x03,470);
			delay_ms(7);
			SetAngle(0x28,470);
			delay_ms(20);
			
		
			
			//�͵�
			SetAngle(0x0a,350);
			delay_ms(350);
			SetAngle(0x0a,630);     //0x01
			delay_ms(130);
			
			//�������
			
			SetAngle(0x03,230);
			delay_ms(10);
			SetAngle(0x28,230);
			delay_ms(100);
			
			
			SetAngle(0x01,560);
			delay_ms(100);
			SetAngle(0x01,750);
			delay_ms(80);
	
//			//���̧��
//			SetAngle(0x28,590);
//			delay_ms(70);
//			SetAngle(0x03,620);
//			delay_ms(7);
//			SetAngle(0x01,530);
//			delay_ms(7);
//			
//			//�͵�
//			SetAngle(0x0a,350);
//			delay_ms(350);
//			SetAngle(0x0a,630);     //0x01
//			delay_ms(110);
//			
//			//�������
//			SetAngle(0x01,230);
//			delay_ms(100);
//			SetAngle(0x03,230);
//			delay_ms(7);
//			SetAngle(0x28,230);
//			delay_ms(120);
			
			__enable_irq();// �������ж�
			//���ж�
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //ʹ��ָ����TIM3�ж�,��������ж�
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //ʹ��ָ����TIM3�ж�,��������ж�
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�����ж�
}

void Control(float input_set_angle,int T_PWM)
{
	Control_X(input_set_angle);//����X
	if(g_State_Flag & 0x20 && g_Terrace_Lock_Flag == 0)
	{
		Terrace_Lock();//������̨
		
//		if(g_State_Flag & 0x30)
//		{
//			Friction_Speed(T_PWM);

//			AX_12_Active(0);

//			Friction_Speed(700);

//			g_State_Flag &= 0x0F; //��ǰ��λ��־
//			g_State_Flag |= 0x08;//��λ
//		}
	}

//	if(g_State_Flag & 0x08)//���������
//	{
//		//���ͱ�־
//		GPIO_ResetBits(GPIOD,GPIO_Pin_5);
//		delay_ms(50);
//		GPIO_SetBits(GPIOD,GPIO_Pin_5);
//		g_State_Flag = 0;
//			
//		Terrace_Unlock();
//	}
//	else
//	{
//		//���߱�־
//		GPIO_SetBits(GPIOB,GPIO_Pin_5);
//	}
}
