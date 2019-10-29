#include "driver.h"
#include "sys.h"
#include "usart.h"

extern u8 g_Usart_Print_Flag;

void Motor_Init_Zero(void)//Z轴回零初始化（准备回零）
{
	g_Usart_Print_Flag = 3;//指定串口3
	printf("V\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//等待串口3发送完毕
	printf("SP3000\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//等待串口3发送完毕
	printf("AC10\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//等待串口3发送完毕
	printf("DEC10\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//等待串口3发送完毕
}

void Motor_V(int set_v)//设定速度
{
	g_Usart_Print_Flag = 3;//指定串口3
	printf("V%d\r",set_v);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//等待串口3发送完毕
}

void Motor_Init_Pos(void)//位置控制初始化（准备进行位置控制）
{
	g_Usart_Print_Flag = 3;//指定串口3
	printf("V\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//等待串口3发送完毕
	printf("SP1500\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//等待串口3发送完毕
	printf("AC30\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//等待串口3发送完毕
	printf("DEC30\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//等待串口3发送完毕
	printf("HO\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//等待串口3发送完毕
}

unsigned int DegToLine(float deg)
{
	unsigned int Line = 0;
	
	Line = (ENCODER_LINE*4*REDUCTION_RATIO*REDUCTION_RATIO_BELT/360.0)*deg;
	
	return Line;
}

void Motor_LA(float deg)//绝对位置控制
{
	unsigned int line=0;
	g_Usart_Print_Flag = 3;//指定串口3
	line = DegToLine(deg+51.54);
	
	printf("LA%d\r",line);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//等待串口3发送完毕
	printf("M\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//等待串口3发送完毕;
}
