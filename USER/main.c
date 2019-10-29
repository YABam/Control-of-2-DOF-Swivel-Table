#define _VERSION_ 5001 //ע���һλ������0

#include "switch.h"

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
//Forecast
#include "set_command.h"

//LED_Display
#include "LED_Digital_Tube.h"

#ifdef _LED_State_Display_
#include "Big_Digital_Tube.h"
#endif

//global value
//======================================
extern u8 g_SWITCH_Debug_Mode_Flag;
extern u8 g_SWITCH_LED_Display_Flag;

extern u8 g_SWITCH_Training_Flag;
extern u8 g_SWITCH_Forecast_Flag;

//extern u8 g_SWITCH_BLUE_Flag;
extern u8 g_SWITCH_RED_Flag;
//======================================

float g_Position=0;

extern u8 g_Robot_Position;
extern u8 g_Target_Number;
extern u8 g_DeffenceFlag;

Shoot_DataStructure g_ShootData[5][7];
Shoot_DataStructure g_DeffenceData;
Shoot_DataStructure g_HitBall[7];

extern Shoot_DataStructure g_ShootData_Blue[5][7];
extern Shoot_DataStructure g_DeffenceData_Blue;
extern Shoot_DataStructure g_HitBall_Blue[7];

extern Shoot_DataStructure g_ShootData_Red[5][7];
extern Shoot_DataStructure g_DeffenceData_Red;
extern Shoot_DataStructure g_HitBall_Red[7];

u8 g_push_reseting = 0;

u8 g_Change_X_Flag = 0;
u8 g_Change_Z_Flag = 0;

//LED_Display
u8 g_Change_S_Flag = 0;

float g_angle[3];
float dangle=0;
float dangle_l1=0;
float dangle_l2=0;

float set_angle=20;

//Training
u8 g_Training_TP = 0;
u8 g_Training_TS = 0;

//Forecast
u8 g_Forecast_Questing_Flag = 0;

//Debug Mode
int g_Timer_Counter = 0;

//LED_Display
u8 g_Display_State_Flag = 0;

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
	
	//Forecast
	float forecast_x_angle = 0;
	float forecast_z_angle = 0;
	u8 forecast_flag = 0;
	int forecast_speed = 0;
	
	//Training
	float SaveX=20;
	float SaveZ=0;
	int SaveS=0;
	
	int temp_set_speed = 0 ;
	int set_speed = 800;
	int T_PWM = 700;
	Stm32_Init();//���������ʼ��
	
	Push_Disable();
	
	if(g_SWITCH_RED_Flag == 1)//���ú쳡
	{
		memcpy(g_ShootData,g_ShootData_Red,sizeof(g_ShootData));
		memcpy(g_HitBall,g_HitBall_Red,sizeof(g_HitBall));
		g_DeffenceData = g_DeffenceData_Red;
	}
	else//��������
	{
		memcpy(g_ShootData,g_ShootData_Blue,sizeof(g_ShootData));
		memcpy(g_HitBall,g_HitBall_Blue,sizeof(g_HitBall));
		g_DeffenceData = g_DeffenceData_Blue;
	}
	
	if(g_SWITCH_LED_Display_Flag == 1)
	{
		Init_MAX7219();//����ܳ�ʼ��
		delay_ms(1000);
		MAX7219_Ready();//LED����ܾ���
		
		LED_Msg_Display(1,_VERSION_,g_SWITCH_RED_Flag);
		delay_ms(1000);
		LED_Msg_Display(2,g_SWITCH_Forecast_Flag,g_SWITCH_Debug_Mode_Flag);
		delay_ms(1000);
		LED_Msg_Display(0,0,0);
	}
	
	#ifdef _LED_State_Display_
	Big_LED_Init();
	me_LED_Display_Test();//������ܲ���
	#endif
	
	AX_12_Reset();
	
	Z_Rotate_Init();//Z����ת��ʼ����Driver��ʼ����
	
	Control_Clock_Init();//����ʱ�ӳ�ʼ����TIM3���жϳ�ʼ����`sc
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);//TIM1���PWM��
	TIM1->CCR1 = 5000;
		
	Push_Enable();
	while(1)
	{
		//����Ħ����
		g_Friction_Ready = Friction_Speed(set_speed);
		
		if(UART5_RX_STA & 0x8000)
		{
			if(g_SWITCH_Forecast_Flag == 1)
			{
				if(UART5_RX_BUF[0] == 0xAA)
				{
					if(Command_Decode(UART5_RX_BUF,&forecast_x_angle,&forecast_z_angle,&forecast_speed)==1)//�����ɹ�
					{
						forecast_flag = 1;
					}
				}
			}
			UART5_RX_STA = 0;
		}
		
		if(UART4_RX_STA&0x8000)
		{
			//ͨ��Э�飬�������4���յ���Ϣ
			//UART4_RX_STA = Decode(UART4_RX_STA&0x3fff, UART4_RX_BUF, &set_x_angle, &set_z_angle);
			Remote_Control_State = Remote_Decode(UART4_RX_BUF,UART4_RX_STA&0x3fff,&set_x_angle,&set_z_angle,&temp_set_speed);
			
			if(g_SWITCH_Training_Flag == 1)
			{
				if(UART4_RX_BUF[0] == 'T' || g_Training_TP ==1 || g_Training_TS == 1)
				{
					if(UART4_RX_BUF[1] == 'P' || g_Training_TP ==1)//TP����
					{
						USART_SendData(UART5,0xA5);//��������
						while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
					}
					else if(UART4_RX_BUF[1] == 'S' || g_Training_TS == 1)//TSȷ������
					{
						if((g_Robot_Position>=1 && g_Robot_Position<=5)&&(g_Target_Number>=1 && g_Target_Number<=7)&&(SaveS>=1010&&SaveS<=1400))
						{	//У�鵱ǰλ�ú���Ŀ��λ�ú���ת�ٺ���
							//�������ݰ�
							USART_SendData(UART5,0xAA);//����ͷ
							while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
					
							USART_SendData(UART5,(u8)SaveX);//���ݸ�������
							while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
					
							USART_SendData(UART5,(u8)((SaveX - (u8)SaveX) * 100));//���ݸ���С��
							while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
					
							USART_SendData(UART5,(u8)(SaveZ + 45));//����ƫ������
							while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
					
							USART_SendData(UART5,(u8)(((SaveZ + 45) - ((u8)(SaveZ + 45))) * 100));//����ƫ��С��
							while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
					
							USART_SendData(UART5,g_Robot_Position);//���ݵ�ǰλ��
							while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
					
							USART_SendData(UART5,g_Target_Number);//����Ŀ��λ��
							while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
					
							USART_SendData(UART5,(u8)(SaveS / 100));//����ת�ٸ�λ
							while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
					
							USART_SendData(UART5,(u8)(SaveS - (((u8)(SaveS / 100)) * 100)));//����ת�ٵ�λ
							while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
						}
					}
				
					g_Training_TP = 2;
					g_Training_TS = 2;
				}
			}
			
			if(g_SWITCH_Debug_Mode_Flag == 1)
			{
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
			}
			
			#ifdef _LED_State_Display_//��������˴�LED״̬����ʾ
			if(UART4_RX_STA >= 4)//���״̬��������
			{
				LED_Display_Number(UART4_RX_BUF[3]);//�������������ʾ
			}
			#endif
			
			UART4_RX_STA = 0;
			
			//======================================================================================================
			
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
				
				if(g_SWITCH_Forecast_Flag == 1)
				{
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
				}
			}
			else if(Remote_Control_State == 2)//����ֵ������
			{
				if(set_x_angle != 0)
				{
					g_Change_X_Flag = 1;
					g_Change_Z_Flag = 0;
				}
				else if(set_z_angle != 0)
				{
					g_Change_Z_Flag = 1;
					g_Change_X_Flag = 0;
				}
				
				if(g_SWITCH_LED_Display_Flag == 1)
				{
					if(temp_set_speed != 0)//����޸���ת��
					{
						g_Change_S_Flag = 1;
					}
					g_Display_State_Flag = 0;
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
				if(g_Friction_Ready == 1 && g_Robot_Position != 6 && g_Robot_Position != 7)//����
				{
					//��ֹ��ν���
					g_Shoot_Flag =1;
					Terrace_Lock();
					set_angle = g_angle[1];
					//Friction_Speed(set_speed);
					if(g_DeffenceFlag == 0)//���Ƿ���״̬
					{
						if(g_SWITCH_Training_Flag == 1)
						{
							//����״̬
							SaveX = g_angle[1];
							SaveZ = g_Position;
							SaveS = TIM2->CCR2;
						}
						
						AX_12_Active(0);
					
						if(g_SWITCH_Forecast_Flag == 1)
						{
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
						}
					}
					else//����״̬
					{
						if(g_DeffenceFlag == 1)
						{
							AX_12_Active(0);
							g_Position = -5.2;
							Motor_LA(g_Position);
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
					if(g_SWITCH_Forecast_Flag == 1)
					{
						g_Forecast_Questing_Flag = 0;
					}
					Terrace_Unlock();
					g_push_reseting = 0;//�Ƹ����reset
				}
				
				if(g_SWITCH_Forecast_Flag == 1)
				{
					if(g_Forecast_Questing_Flag == 1)
					{
						g_Forecast_Questing_Flag = 0;
					}
				}
				g_Shoot_Flag = 0;
				
				g_Change_S_Flag = 0;
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
		
		if(g_SWITCH_Forecast_Flag == 1)
		{			
			if(forecast_flag == 1 && Remote_Control_State != 3)//Ԥ��ֵ����
			{
				forecast_flag = 0;
				//�Ϸ��Լ��
				if(Angle_Check(20,forecast_x_angle,forecast_z_angle,g_Robot_Position,g_Target_Number))
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
		}
		
		if(g_Control_Timer_Flag)//�����ʱ���жϴ�����
		{
			if(g_SWITCH_Debug_Mode_Flag == 1)
			{
				if(g_Timer_Counter>=20)
				{
					g_Timer_Counter=0;
					g_Usart_Print_Flag = 4;
					printf("Z%4.2fX%4.2fS%4d",g_Position,g_angle[1],TIM2->CCR2);
					//printf("S%d",TIM2->CCR2);
				}
			}

			g_Control_Timer_Flag = 0;//���ñ�־
			Control(set_angle,T_PWM);
			
			if(g_SWITCH_LED_Display_Flag == 1)
			{
				if(g_Display_State_Flag == 1)
				{
					LED_Display(g_angle[1],(float)TIM2->CCR2);
				}
				else
				{
					if(g_Change_S_Flag != 0)
					{
						LED_Display(g_angle[1],(float)TIM2->CCR2);
					}
					else
					{
						LED_Display(g_angle[1],g_Position);
					}
				}
			}
		}
	}
}

void TIM3_IRQHandler(void)			   //��ʱ��3ȫ���жϷ�����
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{		
		g_Control_Timer_Flag = 1;//��ʱ���жϴ�����־
		
		if(g_SWITCH_Debug_Mode_Flag == 1)
		{
			g_Timer_Counter++;
			if(g_Timer_Counter>=254)
			{
				g_Timer_Counter=254;
			}
		}
		
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
