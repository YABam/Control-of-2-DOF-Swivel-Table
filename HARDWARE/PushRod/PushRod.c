#define _Debug_PID_

#include "PushRod.h"

#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"

#include "string.h"

extern u8 g_Terrace_Lock_Flag;

void PWM_Pusher(int pwm)
{
	if(g_Terrace_Lock_Flag == 0)//���û������
	{
		int ADD_Bal=5000;
		//�������
		if(pwm>=4999)
		{
			pwm = 4999;
		}
		else if(pwm <= -5000)
		{
			pwm = -5000;
		}
 	
		pwm+=ADD_Bal;
	
		if (pwm > 9950)  pwm = 9950;
		if( pwm < 50) pwm = 50;

		//�ı�PWM��
		TIM1->CCR1 = pwm;
	}
	else//�������
	{
		TIM1->CCR1 = 5000;
	}
	
  //printf("%d\t",pwm);
	//delay_ms(20);
}

float Integral_Pusher(float integral,float dangle)
{
	float temp_integral=integral;
	if(dangle<2&&dangle>-2)
	{
		temp_integral=temp_integral + dangle;//�������ս��
	}
	else
	{
		return 0;
	}
	
	if(temp_integral>=800)
	{
		temp_integral=800;//��������
	}
	else if(temp_integral<=-800)
	{
		temp_integral=-800;
	}

	return temp_integral;
}

void Active_Pusher(float dangle,float wdangle_l1,float Kp,float Kd,float Ki,float integral)
{
	float Index = 0;
	//PID����
	int pwm = dangle * Kp + (dangle-wdangle_l1) * Kd; //����΢��

	#ifdef _Debug_PID_
	
	#endif
	
	//����ֲ���
	if(dangle>=2||dangle<=-2)//�����Զ������
	{
		Index=0;
		//temp_integral = 0;
	}
	else if(dangle<=1&&dangle>=-1)//1��������ȫ����
	{
		Index=1;
		//temp_integral+=dangle;
		//printf("I\t");
	}
	else//֮��ı����ϵ��
	{
		if(dangle>0)
		{
			Index=2 - dangle;
		}
		else
		{
			Index=2 + dangle;
		}
	}
	
	pwm += Ki * Index * integral; //���ֻ���
	
	PWM_Pusher(pwm);
}

void Push_Enable(void)
{
	//�̵�������
	GPIO_SetBits(GPIOB,GPIO_Pin_6);
}
void Push_Disable(void)
{
	//�̵����Ͽ�
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);
}

float Push_Reset()
{
	Push_Disable();
	delay_ms(20);
	Push_Enable();
	return 20.0;
}
