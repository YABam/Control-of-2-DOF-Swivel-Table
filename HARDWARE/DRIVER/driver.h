#ifndef _DRIVER_H_
#define _DRIVER_H_

#define ENCODER_LINE 1024 //����������
#define REDUCTION_RATIO 26 //��������ٱ�
#define REDUCTION_RATIO_BELT 3 //���ּ��ٱ�

void Motor_Init_Zero(void);
void Motor_V(int set_v);
void Motor_Init_Pos(void);//void Motor_HO(void);
void Motor_LA(float deg);
unsigned int DegToLine(float deg);

#endif
