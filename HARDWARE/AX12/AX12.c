#include "usart.h"
#include <stdlib.h>
#include "stm32f10x_usart.h"
#include "stm32f10x.h"
#include "timer.h"

u8 DataPack[20];
u8 i;
extern u8 g_Usart_Print_Flag;

	
/* 函数名：SetAngle
   功能：设置舵机转到目标角度
   参数：串口号，舵机ID,目标角度
   说明：目标角度最大为300°,0xFE可设置所有舵机*/
	 
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

/* 函数名：SetAngleWithVelocity
   功能：设置舵机以规定速度转到目标角度
   参数：串口号，舵机ID,目标角度，角速度
   说明：转速单位为rpm,最大值为114rpm，目标角度最大为300°*/

void SetAngleWithVelocity( USART_TypeDef* USARTx,uint8_t ID,double angle,int velocity)//设置舵机转动角度，范围为0-300° 速度范围为0-114rpm
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

void SetLed (uint8_t ID,int on_off)//设置舵机LED
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

