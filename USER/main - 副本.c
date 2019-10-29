#define _Debug_Mode_ //�����Ƿ񷵻ؽǶ�����
#define _LED_Display_ //�����Ƿ���СLED�������ʾ�Ƕ�
#define _Training_ //����ѵ��ģʽ
//#define _Forecast_ //����Ԥ��ģʽ

#ifdef _Training_
	#ifdef _Forecast_
		#error "Cannot use both Training mode and Forecast mode."
	#endif
#endif
//#define _LED_State_Display_ //�����Ƿ��ڴ�LED�������ʾ״̬��

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
#include "init.h"
#include "Remote_Control.h"
#include "Control.h"

#ifdef _Forecast_
#include "set_command.h"
#endif

#ifdef _LED_Display_
#include "LED_Digital_Tube.h"
#endif

#ifdef _LED_State_Display_
#include "Big_Digital_Tube.h"
#endif

//global value
float g_Position=0;

extern u8 g_Robot_Position;
extern u8 g_Target_Number;
extern u8 g_DeffenceFlag;

u8 g_push_reseting = 0;

u8 g_Change_X_Flag = 0;
u8 g_Change_Z_Flag = 0;

float g_angle[3];
float dangle=0;
float dangle_l1=0;
float dangle_l2=0;

float set_angle=20;

#ifdef _Forecast_
u8 g_Forecast_Questing_Flag = 0;
#endif

#ifdef _Debug_Mode_
int g_Timer_Counter = 0;
#endif

u8 g_Control_Timer_Flag = 0;//���ƶ�ʱ����־
u8 g_Usart_Print_Flag = 1;//printf�򴮿�1����

u8 g_State_Flag = 0;//_    _     _          _         _        _  _  _
                  //  0    0     0          0         0        0  0  0
                 //  x��λ z��λ ��̨Ready  ����Ready �������
u8 g_Ready_Counter=0;

u8 g_Shoot_Flag = 0;//1-�Ѿ��ӵ����� 0-�ɿ�����

extern float integral;//�洢���ֻ���

extern u8 USART1_RX_BUF[200];   
extern u16 USART1_RX_STA;  //����״̬���

extern u8 UART4_RX_BUF[200];
extern u16 UART4_RX_STA;

extern u8 UART5_RX_BUF[200];
extern u16 UART5_RX_STA;
extern u8 Receive_Command_Flag;

u8 Friction_FLAG = 0;
u8 g_Friction_Ready = 0;

//MPU9250����ȫ��
unsigned char g_Re_buf[11],g_temp_buf[11],g_counter=0;
unsigned char g_sign,g_t,g_he;

int main()
{
	u8 Remote_Control_State = 0;
	
	float set_x_angle = 0;
	float set_z_angle = 0;
	
	#ifdef _Forecast_
	float forecast_x_angle = 0;
	float forecast_z_angle = 0;
	u8 forecast_flag = 0;
	int forecast_speed = 0;
	#endif
	
	int temp_set_speed = 0 ;
	int set_speed = 800;
	int T_PWM = 700;
	Stm32_Init();//���������ʼ��
	
	Push_Disable();
	
	#ifdef _LED_Display_
	Init_MAX7219();//����ܳ�ʼ��
	#endif
	
	#ifdef _LED_State_Display_
	Big_LED_Init();
	me_LED_Display_Test();//������ܲ���
	#endif
	
	AX_12_Reset();
	
	Z_Rotate_Init();//Z����ת��ʼ����Driver��ʼ����
	
	Control_Clock_Init();//����ʱ�ӳ�ʼ����TIM3���жϳ�ʼ����`sc
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);//TIM1���PWM��
	TIM1->CCR1 = 5000;
	
	#ifdef _LED_Display_
	MAX7219_Ready();//LED����
	#endif
	
	Push_Enable();
	
	while(1)
	{
		//����Ħ����
		g_Friction_Ready = Friction_Speed(set_speed);
		
		if(UART5_RX_STA & 0x8000)
		{
			#ifdef _Training_

			#endif
			#ifdef _Forecast_
			if(UART5_RX_BUF[0] == 0xAA)
			{
				if(Command_Decode(UART5_RX_BUF,&forecast_x_angle,&forecast_z_angle,&forecast_speed)==1)//�����ɹ�
				{
					forecast_flag = 1;
				}
			}
			#endif 
			UART5_RX_STA = 0;
		}
		
		if(UART4_RX_STA&0x8000)
		{
			//ͨ��Э�飬�������4���յ���Ϣ
			//UART4_RX_STA = Decode(UART4_RX_STA&0x3fff, UART4_RX_BUF, &set_x_angle, &set_z_angle);
			if(UART4_RX_BUF[0] == 'S')
			{
				if(UART4_RX_BUF[1] == 'U')
				{
					set_speed +=2;
					if(set_speed>=1400)
					{
						set_speed = 1400;
					}
				}
				else if(UART4_RX_BUF[1] == 'D')
				{
					set_speed-=2;
					if(set_speed<=800)
					{
						set_speed = 800;
					}
				}
			}
			else
			{
				Remote_Control_State = Remote_Decode(UART4_RX_BUF,UART4_RX_STA&0x3fff,&set_x_angle,&set_z_angle,&temp_set_speed);
			}
			
			#ifdef _LED_State_Display_//��������˴�LED״̬����ʾ
			if(UART4_RX_STA >= 4)//���״̬��������
			{
				LED_Display_Number(UART4_RX_BUF[3]);//�������������ʾ
			}
			#endif
			
			UART4_RX_STA = 0;
			
			if(Remote_Control_State == 1 || Remote_Control_State == 100)//�����Ǿ��ԽǶȣ�˵��������λ�û���Ŀ��
			{
				g_DeffenceFlag = 0;
				//ʲô��������ֱ�Ӹ�ֵ����
				if(temp_set_speed >= 800 )
				{
					set_speed = temp_set_speed;
				}
				
				g_Change_X_Flag = 1;
				g_Change_Z_Flag = 1;
				
				g_Shoot_Flag = 0;
				
				#ifdef _Forecast_ //����һ�����̵�Ԥ������
				if(Remote_Control_State == 1)//���������޸�Ŀ���ˣ���ô������
				{
					if(g_Robot_Position <=5 && g_Robot_Position >=1 && g_Forecast_Questing_Flag == 0)
					{
						if(g_Target_Number >=1 && g_Target_Number <=7)
						{
							USART_SendData(UART5,0xAA);//������ͷ
							while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
						
							//printf("%d%d",g_Robot_Position,g_Target_Number);
							USART_SendData(UART5,g_Robot_Position);
							while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
							USART_SendData(UART5,g_Target_Number);
							while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
						
							USART_SendData(UART5,Receive_Command_Flag);//���ͱ��
							while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
						
							forecast_flag = 0;
							g_Forecast_Questing_Flag = 1;
						}
					}
				}
				#endif
			}
			else if(Remote_Control_State == 2)//����ֵ������
			{
				if(set_x_angle == 0)
				{
					g_Change_X_Flag = 0;
					g_Change_Z_Flag = 1;
				}
				else if(set_z_angle == 0)
				{
					g_Change_Z_Flag = 0;
					g_Change_X_Flag = 1;
				}
				
				set_x_angle = set_angle + set_x_angle;
				set_z_angle = g_Position + set_z_angle;
				//�ı�ת��
				if(set_speed + temp_set_speed >= 1400)
				{
					set_speed = 1400;
				}
				else if(set_speed + temp_set_speed <= 800)
				{
					set_speed = 800;
				}
				else
				{
					set_speed += temp_set_speed;
				}
				g_Shoot_Flag = 0;
			}
			else if(Remote_Control_State == 111)//������Reset�Ƹ�
			{
				g_push_reseting = 1;//�Ƹ˿�ʼ����
				Push_Disable();//�رռ̵���
			}
			else if(((Remote_Control_State == 3 ) && (g_Shoot_Flag == 0)) || (g_DeffenceFlag == 2))//�����Ƿ��䣬����̨Ready�����ǵ�һ�ν��յ�����ָ�
			{
				if(g_Friction_Ready == 1)//����
				{
					//��ֹ��ν���
					g_Shoot_Flag =1;
					Terrace_Lock();
					set_angle = g_angle[1];
					//Friction_Speed(set_speed);
					if(g_DeffenceFlag == 0)//���Ƿ���״̬
					{
						AX_12_Active(0);
					
						#ifdef _Forecast_ //����һ�����̵�Ԥ������
						if(g_Robot_Position <=5 && g_Robot_Position >=1)
						{
							if(g_Target_Number >=1 && g_Target_Number <=7)
							{
								USART_SendData(UART5,0xAA);//������ͷ
								while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
						
								//printf("%d%d",g_Robot_Position,g_Target_Number);
								USART_SendData(UART5,g_Robot_Position);
								while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
								USART_SendData(UART5,g_Target_Number);
								while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
							
								USART_SendData(UART5,Receive_Command_Flag);//���ͱ��
								while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
							
								forecast_flag = 0;
							
							}
						}
					#endif
					}
					else//����״̬
					{
						if(g_DeffenceFlag == 1)
						{
							g_Position = 4;
							Motor_LA(g_Position);
							AX_12_Active(0);
							g_DeffenceFlag = 2;
						}
						else if(g_DeffenceFlag == 2)
						{
							delay_ms(20);
							AX_12_Active(0);
							g_DeffenceFlag = 0;
						}
					}
					
					//Terrace_Unlock();

					g_State_Flag &= 0x0F; //��ǰ��λ��־s
				}
			}
			else if(Remote_Control_State == 4)//����
			{
				//ʲô��������ֱ�Ӹ�ֵ����
				if(temp_set_speed >= 800 )
				{
					set_speed = temp_set_speed;
				}
				
				g_Change_X_Flag = 1;
				g_Change_Z_Flag = 1;
				
				g_Shoot_Flag = 0;
			}
			else//��������
			{
				if(g_push_reseting ==1)//�Ƹ���������ʱ�ɿ���reset
				{
					Push_Enable();//��ͨ�̵���
					set_angle = 20;
					integral = 0;
					#ifdef _Forecast_
					g_Forecast_Questing_Flag = 0;
					#endif
					Terrace_Unlock();
					g_push_reseting = 0;//�Ƹ����reset
				}
				
				#ifdef _Forecast_
				if(g_Forecast_Questing_Flag == 1)
				{
					g_Forecast_Questing_Flag = 0;
				}
				#endif
				g_Shoot_Flag = 0;
			}
			
			if(Remote_Control_State == 1 || Remote_Control_State == 2 || Remote_Control_State == 4)//�Ƕ���Ҫ�ı�
			{
				//�Ϸ��Լ��
				if(set_z_angle>=-45&&set_z_angle<=53)
				{
					g_State_Flag &= 0x9F;//��z��λ��־
					g_Position = set_z_angle;
					Motor_LA(g_Position);
				}

				if(set_x_angle>=-1&&set_x_angle<=38)
				{
					g_State_Flag &= 0x5F;//��x��λ��־
					set_angle=set_x_angle;
				}
				
				if(g_Change_X_Flag == 1)
				{
					Terrace_Unlock();
				}
				g_Change_X_Flag = 0;
				g_Change_Z_Flag = 0;
			}
		}
		
					
		#ifdef _Forecast_
		if(forecast_flag == 1 && Remote_Control_State != 3)//Ԥ��ֵ����
		{
			forecast_flag = 0;
			//�Ϸ��Լ��
			if(Angle_Check(2,forecast_x_angle,forecast_z_angle,g_Robot_Position,g_Target_Number))
			{
				//�ݲ���֤ͨ��
				if(forecast_z_angle>=-45&&forecast_z_angle<=53)
				{
					g_State_Flag &= 0x9F;//��z��λ��־
					g_Position = forecast_z_angle;
					Motor_LA(g_Position);
				}
				if(forecast_x_angle>=0&&forecast_x_angle<=40)
				{
					g_State_Flag &= 0x5F;//��x��λ��־
					set_angle=forecast_x_angle;
					Terrace_Unlock();
				}
				
				set_speed = forecast_speed;
			}
		}
		#endif
		
		if(g_Control_Timer_Flag)//�����ʱ���жϴ�����
		{
			#ifdef _Debug_Mode_
			if(g_Timer_Counter>=20)
			{
				g_Timer_Counter=0;
				g_Usart_Print_Flag = 4;
				printf("Z%4.2fX%4.2fS%4d",g_Position,g_angle[1],TIM2->CCR2);
				//printf("S%d",TIM2->CCR2);
			}
			#endif

			g_Control_Timer_Flag = 0;//���ñ�־
			Control(set_angle,T_PWM);
			
			#ifdef _LED_Display_
			LED_Display(g_angle[1],g_Position);
			#endif
		}
	}
}

void TIM3_IRQHandler(void)			   //��ʱ��3ȫ���жϷ�����
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{		
		g_Control_Timer_Flag = 1;//��ʱ���жϴ�����־
		
#ifdef _Debug_Mode_
		g_Timer_Counter++;
		if(g_Timer_Counter>=254)
		{
			g_Timer_Counter=254;
		}
#endif
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

void TIM4_IRQHandler(void)			   //��ʱ��3ȫ���жϷ�����
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{		
		Friction_FLAG = 1;//��ʱ���жϴ�����־
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}
