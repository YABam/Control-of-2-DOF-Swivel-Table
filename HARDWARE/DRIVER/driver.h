#ifndef _DRIVER_H_
#define _DRIVER_H_

#define ENCODER_LINE 1024 //编码器线数
#define REDUCTION_RATIO 26 //减速箱减速比
#define REDUCTION_RATIO_BELT 3 //带轮减速比

void Motor_Init_Zero(void);
void Motor_V(int set_v);
void Motor_Init_Pos(void);//void Motor_HO(void);
void Motor_LA(float deg);
unsigned int DegToLine(float deg);

#endif
