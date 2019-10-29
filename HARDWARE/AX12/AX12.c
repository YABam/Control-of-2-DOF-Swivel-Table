#include "usart.h"
#include <stdlib.h>
#include "stm32f10x_usart.h"
#include "stm32f10x.h"
#include "timer.h"

u8 DataPack[20];
u8 i;
extern u8 g_Usart_Print_Flag;

	
/* ��������SetAngle
   ���ܣ����ö��ת��Ŀ��Ƕ�
   ���������ںţ����ID,Ŀ��Ƕ�
   ˵����Ŀ��Ƕ����Ϊ300��,0xFE���������ж��*/
	 
void SetAngle(u8 ID,double angle)
{
  uint16_t Goalangle=0;
	g_Usart_Print_Flag = 1;
	DataPack[0]=0xff;
	DataPack[1]=0xff;
	DataPack[2]=ID;
	DataPack[3]=0x05;
	DataPack[4]=0x03;
	DataPack[5]=0x1E;
	//Goalangle=((int)((angle/300)*1024)-1);
	Goalangle = angle;
	DataPack[6]=Goalangle;
	DataPack[7]=Goalangle>>8;
	DataPack[8]= (~(DataPack[2]+DataPack[3]+DataPack[4]+DataPack[5]+DataPack[6]+DataPack[7]));
	
	for(i=0;i<9;i++)
	{
	   USART_SendData(USART1,DataPack[i]);
		 while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}
}

/* ��������SetAngleWithVelocity
   ���ܣ����ö���Թ涨�ٶ�ת��Ŀ��Ƕ�
   ���������ںţ����ID,Ŀ��Ƕȣ����ٶ�
   ˵����ת�ٵ�λΪrpm,���ֵΪ114rpm��Ŀ��Ƕ����Ϊ300��*/

void SetAngleWithVelocity( USART_TypeDef* USARTx,uint8_t ID,double angle,int velocity)//���ö��ת���Ƕȣ���ΧΪ0-300�� �ٶȷ�ΧΪ0-114rpm
{

	uint16_t Goalangle=0;
	uint16_t veloc=0;
	DataPack[0]=0xff;
	DataPack[1]=0xff;
	DataPack[2]=ID;
	DataPack[3]=0x07;
	DataPack[4]=0x03;
	DataPack[5]=0x1E;
	Goalangle=((int)((angle/300)*1024)-1);
	DataPack[6]=Goalangle;
	DataPack[7]=Goalangle>>8;
	veloc=((int)((velocity/114)*1024)-1);
	DataPack[8]=veloc;
	DataPack[9]=veloc>>8;
	DataPack[10]= (~(DataPack[2]+DataPack[3]+DataPack[4]+DataPack[5]+DataPack[6]+DataPack[7]+DataPack[8]+DataPack[9]));
	for(i=0;i<11;i++)
	{
	   USART_SendData(USARTx,DataPack[i]);
		 while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
	}
}

void SetLed (uint8_t ID,int on_off)//���ö��LED
{
	DataPack[0]=0xff;
	DataPack[1]=0xff;
	DataPack[2]=ID;
	DataPack[3]=0x04;
	DataPack[4]=0x03;
	DataPack[5]=0x19;
	DataPack[6]=on_off;
	DataPack[7]=(~(DataPack[2]+DataPack[3]+DataPack[4]+DataPack[5]+DataPack[6]));
	for(i=0;i<8;i++)
	{
	   USART_SendData(USART1,DataPack[i]);
		 while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}
}

