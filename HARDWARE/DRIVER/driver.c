#include "driver.h"
#include "sys.h"
#include "usart.h"

extern u8 g_Usart_Print_Flag;

void Motor_Init_Zero(void)//Z������ʼ����׼�����㣩
{
	g_Usart_Print_Flag = 3;//ָ������3
	printf("V\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//�ȴ�����3�������
	printf("SP3000\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//�ȴ�����3�������
	printf("AC10\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//�ȴ�����3�������
	printf("DEC10\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//�ȴ�����3�������
}

void Motor_V(int set_v)//�趨�ٶ�
{
	g_Usart_Print_Flag = 3;//ָ������3
	printf("V%d\r",set_v);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//�ȴ�����3�������
}

void Motor_Init_Pos(void)//λ�ÿ��Ƴ�ʼ����׼������λ�ÿ��ƣ�
{
	g_Usart_Print_Flag = 3;//ָ������3
	printf("V\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//�ȴ�����3�������
	printf("SP1500\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//�ȴ�����3�������
	printf("AC30\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//�ȴ�����3�������
	printf("DEC30\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//�ȴ�����3�������
	printf("HO\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//�ȴ�����3�������
}

unsigned int DegToLine(float deg)
{
	unsigned int Line = 0;
	
	Line = (ENCODER_LINE*4*REDUCTION_RATIO*REDUCTION_RATIO_BELT/360.0)*deg;
	
	return Line;
}

void Motor_LA(float deg)//����λ�ÿ���
{
	unsigned int line=0;
	g_Usart_Print_Flag = 3;//ָ������3
	line = DegToLine(deg+51.54);
	
	printf("LA%d\r",line);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//�ȴ�����3�������
	printf("M\r");
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//�ȴ�����3�������;
}
