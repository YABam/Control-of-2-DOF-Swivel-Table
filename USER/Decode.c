

//Decode(UART4_RX_STA&0x3fff, UART4_RX_BUF, &set_x_angle, &set_z_angle);
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

u16 Decode(u8 len, u8 UART4_RX_BUF[], float *p_set_x_angle, float *p_set_z_angle)
{
	u8 com_count=0;
	u8 flag_command_Z_first=0; // 0 - ZX  1 - XZ
	u8 Z_len=0;
	u8 X_len=0;
	u8 flag_command_only=0;
	u8 flag_command_Integer=0;
	u8 command_counter=0;

	//Ѱ��Z��X����
	flag_command_only=1;
	if(UART4_RX_BUF[0]=='Z')
	{
		flag_command_Z_first=1;//��Z
		for(com_count=0;com_count<len;com_count++)
		{
			if(UART4_RX_BUF[com_count]=='X')
			{
				flag_command_only=0;//����ֻ��һ��
				break;
			}
		}
		Z_len = com_count - 1;
		X_len = len - com_count -1;
	}
	else if(UART4_RX_BUF[0]=='X')
	{
		flag_command_Z_first=0;//��X
		for(com_count=0;com_count<len;com_count++)
		{
			if(UART4_RX_BUF[com_count]=='Z')
			{
				flag_command_only=0;//����ֻ��һ��
				break;
			}
			X_len = com_count - 1;
			Z_len = len - com_count -1;
		}
	}
	else//error
	{
		goto meEND;
	}
	//����ָ��
	flag_command_Integer = 1;
	if(flag_command_Z_first==1)//�����Z
	{
		if(flag_command_only==1)//���ֻ��Z
		{
			flag_command_Integer = 1;
			//����Z
			*p_set_z_angle=0;
			for(com_count=1;com_count<Z_len+1;com_count++)
			{
				if(UART4_RX_BUF[com_count]=='.')
				{
					flag_command_Integer = 0;//��������
					break;
				}
			}
			if(flag_command_Integer == 1)//������
			{
				if(UART4_RX_BUF[1]=='-')//����
				{
					for(com_count=2;com_count<Z_len+1;com_count++)
					{
						*p_set_z_angle+=(float)((UART4_RX_BUF[com_count]-0x30)*pow(10,Z_len-com_count));
					}
					*p_set_z_angle*=-1;
				}
				else//����
				{
					for(com_count=1;com_count<Z_len+1;com_count++)
					{
						*p_set_z_angle+=(float)((UART4_RX_BUF[com_count]-0x30)*pow(10,Z_len-com_count));
					}
				}
			}
			else//��С��
			{
				if(UART4_RX_BUF[1]=='-')//����
				{
					for(command_counter=2;command_counter<com_count;command_counter++)
					{
						*p_set_z_angle+=(float)((UART4_RX_BUF[command_counter]-0x30)*pow(10,com_count-command_counter-1));//������������
					}
					for(command_counter=com_count+1;command_counter<Z_len+1;command_counter++)//����С������
					{
						*p_set_z_angle+=(float)((UART4_RX_BUF[command_counter]-0x30)*pow(10,(int)(com_count-command_counter)));
					}
					*p_set_z_angle*=-1;
				}
				else//����
				{
					for(command_counter=1;command_counter<com_count;command_counter++)
					{
						*p_set_z_angle+=(float)((UART4_RX_BUF[command_counter]-0x30)*pow(10,com_count-command_counter-1));//������������
					}
					for(command_counter=com_count+1;command_counter<Z_len+1;command_counter++)
					{
						*p_set_z_angle+=(float)((UART4_RX_BUF[command_counter]-0x30)*pow(10,(int)(com_count-command_counter)));
					}
				}
			}
		}
		else//ZX��ʽ
		{
			flag_command_Integer = 1;
			//����Z
			*p_set_z_angle=0;
			for(com_count=1;com_count<Z_len+1;com_count++)
			{
				if(UART4_RX_BUF[com_count]=='.')
				{
					flag_command_Integer = 0;//��������
					break;
				}
			}
			if(flag_command_Integer == 1)//������
			{
				if(UART4_RX_BUF[1]=='-')//����
				{
					for(com_count=2;com_count<Z_len+1;com_count++)	
					{
						*p_set_z_angle+=(float)((UART4_RX_BUF[com_count]-0x30)*pow(10,Z_len-com_count));
					}
					*p_set_z_angle*=-1;
				}
				else//����
				{
					for(com_count=1;com_count<Z_len+1;com_count++)
					{
						*p_set_z_angle+=(float)((UART4_RX_BUF[com_count]-0x30)*pow(10,Z_len-com_count));
					}
				}
			}
			else//��С��
			{
				if(UART4_RX_BUF[1]=='-')//����
				{
					for(command_counter=2;command_counter<com_count;command_counter++)
					{
						*p_set_z_angle+=(float)((UART4_RX_BUF[command_counter]-0x30)*pow(10,com_count-command_counter-1));//������������
					}
					for(command_counter=com_count+1;command_counter<Z_len+1;command_counter++)//����С������
					{
						*p_set_z_angle+=(float)((UART4_RX_BUF[command_counter]-0x30)*pow(10,(int)(com_count-command_counter)));
					}
					*p_set_z_angle*=-1;
				}
				else//����
				{
					for(command_counter=1;command_counter<com_count;command_counter++)
					{
						*p_set_z_angle+=(float)((UART4_RX_BUF[command_counter]-0x30)*pow(10,com_count-command_counter-1));//������������
					}
					for(command_counter=com_count+1;command_counter<Z_len+1;command_counter++)
					{
						*p_set_z_angle+=(float)((UART4_RX_BUF[command_counter]-0x30)*pow(10,(int)(com_count-command_counter)));
					}
				}
			}
					
			flag_command_Integer = 1;
			//����X
			*p_set_x_angle=0;
			for(com_count=Z_len+2;com_count<len;com_count++)
			{
				if(UART4_RX_BUF[com_count]=='.')
				{
					flag_command_Integer = 0;//��������
					break;
				}
			}
			if(flag_command_Integer == 1)//������
			{
				for(com_count=Z_len+2;com_count<len;com_count++)
				{
					*p_set_x_angle+=(float)((UART4_RX_BUF[com_count]-0x30)*pow(10,len-1-com_count));
				}
			}
			else//��С��
			{
				for(command_counter=Z_len+2;command_counter<com_count;command_counter++)
				{
					*p_set_x_angle+=(float)((UART4_RX_BUF[command_counter]-0x30)*pow(10,com_count-command_counter-1));//������������
				}
				for(command_counter=com_count+1;command_counter<len;command_counter++)
				{
					*p_set_x_angle+=(float)((UART4_RX_BUF[command_counter]-0x30)*pow(10,(int)(com_count-command_counter)));
				}
			}
		}
	}
	else//�����X ֱ�ӱ���д����̫�鷳�˲�д��
	{
		X_len=0;
		if(X_len==0)
		{
			Z_len++;
		}
		goto meEND;
	}
	
meEND:return 0;
}
